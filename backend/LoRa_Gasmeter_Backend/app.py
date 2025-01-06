from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import datetime
import logging
import json
import os

# App and Database Configuration
app = Flask(__name__)
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
    device_id = db.Column(db.String(100))  # Device ID (e.g., "99% von euch sind RATTEN!")
    timestamp = db.Column(db.Integer)  # Timestamp
    sensors = db.Column(db.Text)  # JSON string for sensors

    def __init__(self, pct, device_id, timestamp, sensors):
        self.pct = pct
        self.device_id = device_id
        self.timestamp = timestamp
        self.sensors = sensors

@app.before_request
def log_request_info():
    logger.info('%s %s', request.method, request.url)

# Root Endpoint
@app.route('/')
def index():
    return jsonify({"message": "Welcome to the Sensor Data API. Use /receive-json to POST data or /get-last-entity to GET the last entry."})

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
        ts = data.get('ts')
        sensors = json.dumps(data.get('sensors', []), ensure_ascii=False)  # Ensure valid JSON

        if pct is None or not device_id or ts is None:
            return jsonify({"error": "Missing required fields: 'pct', 'id', or 'ts'"}), 400

        # Add data to the database
        sensor_data = SensorData(pct, device_id, ts, sensors)
        db.session.add(sensor_data)
        db.session.commit()

        return jsonify({"received": data}), 200
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500


# Retrieve Last Entity
@app.route('/get-last-entity', methods=['GET'])
def get_last_entity():
    try:
        last_entity = SensorData.query.order_by(SensorData.id.desc()).first()
        if last_entity:
            try:
                raw_sensors = last_entity.sensors
                # Fix extra escaping by applying json.loads() twice
                sensors = json.loads(raw_sensors.replace('\\"', '"'))
            except json.JSONDecodeError as e:
                logger.error(f"JSON parsing error for sensors: {raw_sensors}")
                return jsonify({"error": "Invalid JSON in sensors field"}), 500

            data = {
                "pct": last_entity.pct,
                "device_id": last_entity.device_id,
                "timestamp": last_entity.timestamp,  # Raw integer timestamp
                "sensors": sensors
            }
            return jsonify(data), 200
        else:
            return jsonify({"error": "No data found"}), 404
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
