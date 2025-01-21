"""
@file app.py
@brief Flask application with SocketIO and SQLAlchemy for real-time sensor data monitoring.
"""

from flask import Flask, request, jsonify, render_template
from flask_socketio import SocketIO
from threading import Thread
import time
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import datetime
import logging
import json
import os

app = Flask(__name__)

"""
@brief Initialize Flask-SocketIO.
"""
socketio = SocketIO(app, ping_timeout=30, ping_interval=10)

"""
@brief App and Database Configuration.
"""
app.config['SQLALCHEMY_DATABASE_URI'] = f'sqlite:///{"/home/csuser/GasMonitorLoRaCloud/backend/instance/sensor_data.db"}?check_same_thread=False'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
CORS(app, resources={r"/*": {"origins": "*"}})
app.config['TEMPLATES_AUTO_RELOAD'] = True
db = SQLAlchemy(app)

"""
@brief Initializing logger.
"""
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

"""
@class SensorData
@brief ORM model for sensor data storage.
"""
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

"""
@brief Log incoming requests.
"""
@app.before_request
def log_request_info():
    logger.info('%s %s', request.method, request.url)
    app.jinja_env.cache = {}

"""
@brief Handle WebSocket client connection.
"""
@socketio.on('connect')
def handle_connect():
    logger.info("Client connected.")

"""
@brief Handle WebSocket client disconnection.
"""
@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected.")

"""
@brief Background task to periodically send the latest sensor data.
"""
def send_latest_entity():
    with app.app_context():
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
            time.sleep(0.5)

Thread(target=send_latest_entity, daemon=True).start()

"""
@brief Root endpoint rendering the index page.
@return Rendered HTML template.
"""
@app.route('/')
def index():
    return render_template('index.html')

"""
@brief Endpoint to render the graph page.
@return Rendered HTML template.
"""
@app.route('/graph')
def graph():
    return render_template('graph.html')

"""
@brief Endpoint to receive and store sensor data.
@return JSON response.
"""
@app.route('/receive-json', methods=['POST'])
def receive_json():
    try:
        data = request.get_json()
        if not data:
            return jsonify({"error": "No JSON payload provided"}), 400

        pct = data.get('pct')
        device_id = data.get('id')
        ts = data.get('ts')
        sensors = json.dumps(data.get('sensors', []), ensure_ascii=False)
        system_time = int(datetime.datetime.now().timestamp())

        if pct is None or not device_id:
            return jsonify({"error": "Missing required fields: 'pct' or 'id'"}), 400

        sensor_data = SensorData(pct, device_id, ts, system_time, sensors)
        db.session.add(sensor_data)
        db.session.commit()

        return jsonify({"received": data, "system_time": system_time}), 200
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

"""
@brief Retrieve the last sensor data entry.
@return JSON response with the latest entry.
"""
@app.route('/get-last-entity', methods=['GET'])
def get_last_entity():
    try:
        last_entity = SensorData.query.order_by(SensorData.id.desc()).first()
        if last_entity:
            try:
                raw_sensors = last_entity.sensors
                sensors = json.loads(raw_sensors.replace('\\"', '"'))
            except json.JSONDecodeError:
                logger.error(f"JSON parsing error for sensors: {raw_sensors}")
                return jsonify({"error": "Invalid JSON in sensors field"}), 500

            system_iso_datetime = datetime.datetime.fromtimestamp(last_entity.system_time).isoformat()

            data = {
                "pct": last_entity.pct,
                "device_id": last_entity.device_id,
                "system_time": last_entity.system_time,
                "system_datetime": system_iso_datetime,
                "sensors": sensors
            }
            return jsonify(data), 200
        return jsonify({"error": "No data found"}), 404
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

"""
@brief Retrieve sensor data from a specific timestamp.
@return JSON response with filtered data entries.
"""
@app.route('/get-data-from-timestamp', methods=['GET'])
def get_data_from_timestamp():
    try:
        system_time = request.args.get('system_time', type=int)
        if system_time is None:
            return jsonify({"error": "System time parameter is required"}), 400

        first_entity = SensorData.query.order_by(SensorData.system_time.asc()).first()
        if first_entity is None:
            return jsonify({"error": "No data found"}), 404

        if system_time < first_entity.system_time:
            system_time = first_entity.system_time

        data_query = SensorData.query.filter(SensorData.system_time >= system_time).order_by(SensorData.system_time.asc()).all()
        if not data_query:
            return jsonify({"error": "No data found for the provided system time"}), 404

        data_list = []
        for entity in data_query:
            try:
                raw_sensors = entity.sensors
                sensors = json.loads(raw_sensors.replace('\\"', '"'))
            except json.JSONDecodeError:
                logger.error(f"JSON parsing error for sensors: {raw_sensors}")
                sensors = []

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

"""
@brief Teardown the session after each request.
"""
@app.teardown_request
def remove_session(exception=None):
    db.session.remove()

"""
@brief Initialize the database and run the app.
"""
if __name__ == '__main__':
    with app.app_context():
        try:
            logger.info("Checking for existing database and tables...")
            if not os.path.exists('/home/csuser/GasMonitorLoRaCloud/backend/instance/sensor_data.db'):
                logger.info("Database file not found. Creating a new database.")
            db.create_all()
            logger.info("Database and tables created successfully.")
        except Exception as e:
            logger.error(f"Error during database initialization: {e}")
            exit(1)

    app.run(debug=True, host='0.0.0.0', port=8000)
    socketio.run(app, debug=True, host='0.0.0.0', port=8000)
