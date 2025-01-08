from flask import Flask, request, jsonify, render_template
from flask_socketio import SocketIO, emit
from threading import Thread
import time
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import requests
import datetime
import logging
import json
import os

app = Flask(__name__)

# Initialize Flask-SocketIO
socketio = SocketIO(app)

# App and Database Configuration
app.config['SQLALCHEMY_DATABASE_URI'] = f'sqlite:///{"/home/csuser/GasMonitorLoRaCloud/backend/instance/sensor_data.db"}?check_same_thread=False'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
CORS(app, resources={r"/*": {"origins": "*"}})

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)
db = SQLAlchemy(app)

# Define the SensorData Model
class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    pct = db.Column(db.Integer)  # Battery percentage
    device_id = db.Column(db.String(100))  # Device ID
    timestamp = db.Column(db.Integer)  # Client-provided timestamp
    system_time = db.Column(db.Integer)  # Server's system time
    sensors = db.Column(db.Text)  # JSON string for sensors

    def __init__(self, pct, device_id, timestamp, system_time, sensors):
        self.pct = pct
        self.device_id = device_id
        self.timestamp = timestamp
        self.system_time = system_time
        self.sensors = sensors

# Logger
@app.before_request
def log_request_info():
    logger.info('%s %s', request.method, request.url)

# Web socket
# WebSocket route to push real-time updates
@socketio.on('connect')
def handle_connect():
    logger.info("Client connected.")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected.")

# Periodically send the latest entity
def send_latest_entity():
    with app.app_context():  # Push the application context
        while True:
            last_entity = SensorData.query.order_by(SensorData.id.desc()).first()
            if last_entity:
                raw_sensors = last_entity.sensors
                sensors = json.loads(raw_sensors.replace('\\"', '"'))
                system_iso_datetime = datetime.datetime.fromtimestamp(last_entity.system_time).isoformat()

                data = {
                    "pct": last_entity.pct,
                    "device_id": last_entity.device_id,
                    "system_time": last_entity.system_time,
                    "system_datetime": system_iso_datetime,
                    "sensors": sensors
                }
                socketio.emit('update_data', data)
            time.sleep(0.5)  # Adjust interval as needed

# Start the background task to push updates
Thread(target=send_latest_entity, daemon=True).start()

# Root Endpoint
@app.route('/')
def index():
    return render_template('index.html')

# Graph Endpoint
@app.route('/graph')
def graph():
    """
    Render the graph.html template.
    """
    return render_template('graph.html')

# Add Data to Database
@app.route('/receive-json', methods=['POST'])
def receive_json():
    try:
        data = request.get_json()
        if not data:
            return jsonify({"error": "No JSON payload provided"}), 400

        # Parse JSON data
        pct = data.get('pct')
        device_id = data.get('id')
        ts = data.get('ts')  # Client-provided timestamp
        sensors = json.dumps(data.get('sensors', []), ensure_ascii=False)  # Ensure valid JSON

        # Server's system time
        system_time = int(datetime.datetime.now().timestamp())

        if pct is None or not device_id:
            return jsonify({"error": "Missing required fields: 'pct' or 'id'"}), 400

        # Add data to the database
        sensor_data = SensorData(pct, device_id, ts, system_time, sensors)
        db.session.add(sensor_data)
        db.session.commit()

        return jsonify({"received": data, "system_time": system_time}), 200
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

# Retrieve Last Entity
@app.route('/get-last-entity', methods=['GET'])
def get_last_entity():
    try:
        # Query the last entity from the database
        last_entity = SensorData.query.order_by(SensorData.id.desc()).first()
        
        if last_entity:
            try:
                # Retrieve and parse the `sensors` field as JSON
                raw_sensors = last_entity.sensors
                sensors = json.loads(raw_sensors.replace('\\"', '"'))  # Handle escaping issues
            except json.JSONDecodeError as e:
                logger.error(f"JSON parsing error for sensors: {raw_sensors}")
                return jsonify({"error": "Invalid JSON in sensors field"}), 500

            # Convert system timestamp to ISO 8601 format
            system_iso_datetime = datetime.datetime.fromtimestamp(last_entity.system_time).isoformat()

            # Prepare the response
            data = {
                "pct": last_entity.pct,
                "device_id": last_entity.device_id,
                "system_time": last_entity.system_time,  # Server-side system timestamp
                "system_datetime": system_iso_datetime,  # ISO 8601 formatted datetime
                "sensors": sensors
            }
            return jsonify(data), 200
        else:
            # No data in the database
            return jsonify({"error": "No data found"}), 404
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500


# Retrieve Data From a Specific Timestamp
@app.route('/get-data-from-timestamp', methods=['GET'])
def get_data_from_timestamp():
    try:
        # Get the system_time parameter
        system_time = request.args.get('system_time', type=int)
        if system_time is None:
            return jsonify({"error": "System time parameter is required"}), 400

        # Find the first entity if the system_time is earlier than any record
        first_entity = SensorData.query.order_by(SensorData.system_time.asc()).first()
        if first_entity is None:
            return jsonify({"error": "No data found"}), 404

        if system_time < first_entity.system_time:
            system_time = first_entity.system_time

        # Retrieve records from the given system_time onward
        data_query = SensorData.query.filter(SensorData.system_time >= system_time).order_by(SensorData.system_time.asc()).all()
        if not data_query:
            return jsonify({"error": "No data found for the provided system time"}), 404

        # Format the data
        data_list = []
        for entity in data_query:
            try:
                raw_sensors = entity.sensors
                sensors = json.loads(raw_sensors.replace('\\"', '"'))  # Parse sensor data
            except json.JSONDecodeError:
                logger.error(f"JSON parsing error for sensors: {raw_sensors}")
                sensors = []  # Default to empty list on error

            system_iso_datetime = datetime.datetime.fromtimestamp(entity.system_time).isoformat()
            data_list.append({
                "pct": entity.pct,
                "device_id": entity.device_id,
                "system_time": entity.system_time,
                "system_datetime": system_iso_datetime,
                "sensors": sensors
            })

        return jsonify(data_list), 200
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

# Teardown Session
@app.teardown_request
def remove_session(exception=None):
    db.session.remove()

# Initialize and Run the App
if __name__ == '__main__':
    with app.app_context():
        try:
            logger.info("Checking for existing database and tables...")
            if not os.path.exists('/home/csuser/GasMonitorLoRaCloud/backend/instance/sensor_data.db'):
                logger.info("Database file not found. Creating a new database.")
            db.create_all()  # Create tables if they don't exist
            logger.info("Database and tables created successfully.")
        except Exception as e:
            logger.error(f"Error during database initialization: {e}")
            exit(1)

    app.run(debug=True, host='0.0.0.0', port=8000)
    socketio.run(app, debug=True, host='0.0.0.0', port=8000)
