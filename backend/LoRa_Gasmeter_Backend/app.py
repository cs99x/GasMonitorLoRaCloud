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
from flasgger import Swagger
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
# Use flexible database path that works in different environments
basedir = os.path.abspath(os.path.dirname(__file__))
db_path = os.environ.get('DATABASE_PATH') or os.path.join(os.path.dirname(basedir), 'instance', 'sensor_data.db')
app.config['SQLALCHEMY_DATABASE_URI'] = f'sqlite:///{db_path}?check_same_thread=False'
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
            time.sleep(1)

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

@app.route('/receive-json', methods=['POST'])
def receive_json():
    """
    Endpoint to receive and store sensor data.
    ---
    tags:
      - Sensor Data
    summary: Receive and store sensor data
    description: Accepts sensor data from IoT devices and stores it in the database. The system automatically adds a server timestamp upon receipt.
    consumes:
      - application/json
    produces:
      - application/json
    parameters:
      - in: body
        name: body
        description: Sensor data payload
        required: true
        schema:
          type: object
          required:
            - pct
            - id
          properties:
            pct:
              type: integer
              description: Battery percentage (0-100)
              example: 90
            id:
              type: string
              description: Device identifier
              example: "handheld 1"
            ts:
              type: integer
              description: Client-provided timestamp (Unix timestamp)
              example: 1673049150
            sensors:
              type: array
              description: Array of sensor readings
              items:
                type: object
                properties:
                  type:
                    type: string
                    description: Type of gas sensor (CH4, CO, LPG, H2)
                    example: "CH4"
                  val:
                    type: integer
                    description: Sensor value in ppm
                    example: 200
    responses:
      200:
        description: Data successfully received and stored
        schema:
          type: object
          properties:
            received:
              type: object
              description: Echo of the received data
            system_time:
              type: integer
              description: Server timestamp when data was received
      400:
        description: Invalid request (missing required fields or no JSON payload)
        schema:
          type: object
          properties:
            error:
              type: string
              example: "Missing required fields: 'pct' or 'id'"
      500:
        description: Server error
        schema:
          type: object
          properties:
            error:
              type: string
              example: "An unexpected error occurred."
    """
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

@app.route('/get-last-entity', methods=['GET'])
def get_last_entity():
    """
    Retrieve the last sensor data entry.
    ---
    tags:
      - Sensor Data
    summary: Get the latest sensor data entry
    description: Retrieves the most recent sensor data entry from the database, including battery level, device ID, timestamp, and all sensor readings.
    produces:
      - application/json
    responses:
      200:
        description: Latest sensor data retrieved successfully
        schema:
          type: object
          properties:
            pct:
              type: integer
              description: Battery percentage
              example: 90
            device_id:
              type: string
              description: Device identifier
              example: "handheld 1"
            system_time:
              type: integer
              description: Server timestamp (Unix timestamp)
              example: 1673049150
            system_datetime:
              type: string
              description: Server timestamp in ISO 8601 format
              example: "2023-01-06T20:45:50"
            sensors:
              type: array
              description: Array of sensor readings
              items:
                type: object
                properties:
                  type:
                    type: string
                    description: Type of gas sensor
                    example: "CH4"
                  val:
                    type: integer
                    description: Sensor value in ppm
                    example: 200
      404:
        description: No data found in the database
        schema:
          type: object
          properties:
            error:
              type: string
              example: "No data found"
      500:
        description: Server error
        schema:
          type: object
          properties:
            error:
              type: string
              example: "An unexpected error occurred."
    """
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

@app.route('/get-data-from-timestamp', methods=['GET'])
def get_data_from_timestamp():
    """
    Retrieve sensor data from a specific timestamp.
    ---
    tags:
      - Sensor Data
    summary: Get historical sensor data from a specific timestamp
    description: Retrieves all sensor data entries from the specified timestamp onwards. If the provided timestamp is earlier than the first entry, data is returned from the first available entry.
    produces:
      - application/json
    parameters:
      - in: query
        name: system_time
        type: integer
        required: true
        description: Unix timestamp to start retrieving data from
        example: 1673049150
    responses:
      200:
        description: Sensor data retrieved successfully
        schema:
          type: array
          items:
            type: object
            properties:
              pct:
                type: integer
                description: Battery percentage
                example: 90
              device_id:
                type: string
                description: Device identifier
                example: "handheld 1"
              system_time:
                type: integer
                description: Server timestamp (Unix timestamp)
                example: 1673049150
              system_datetime:
                type: string
                description: Server timestamp in ISO 8601 format
                example: "2023-01-06T20:45:50"
              sensors:
                type: array
                description: Array of sensor readings
                items:
                  type: object
                  properties:
                    type:
                      type: string
                      description: Type of gas sensor
                      example: "CH4"
                    val:
                      type: integer
                      description: Sensor value in ppm
                      example: 200
      400:
        description: Missing or invalid query parameter
        schema:
          type: object
          properties:
            error:
              type: string
              example: "System time parameter is required"
      404:
        description: No data found for the provided timestamp
        schema:
          type: object
          properties:
            error:
              type: string
              example: "No data found for the provided system time"
      500:
        description: Server error
        schema:
          type: object
          properties:
            error:
              type: string
              example: "An unexpected error occurred."
    """
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
@brief Initialize Swagger for API documentation.
"""
swagger_template = {
    "swagger": "2.0",
    "info": {
        "title": "Gas Monitor LoRa Cloud API",
        "description": "REST API for real-time sensor data monitoring. This API allows you to send sensor data from IoT devices, retrieve the latest readings, and query historical data.",
        "version": "1.0.0",
        "contact": {
            "name": "GasMonitorLoRaCloud Project",
            "url": "https://github.com/cs99x/GasMonitorLoRaCloud"
        }
    },
    "basePath": "/",
    "schemes": ["http", "https"]
}

swagger = Swagger(app, template=swagger_template)

"""
@brief Initialize the database and run the app.
"""
if __name__ == '__main__':
    with app.app_context():
        try:
            logger.info("Checking for existing database and tables...")
            db_dir = os.path.dirname(db_path)
            if not os.path.exists(db_dir):
                os.makedirs(db_dir)
                logger.info(f"Created database directory: {db_dir}")
            if not os.path.exists(db_path):
                logger.info("Database file not found. Creating a new database.")
            db.create_all()
            logger.info("Database and tables created successfully.")
        except Exception as e:
            logger.error(f"Error during database initialization: {e}")
            exit(1)

    # Use socketio.run to initialize Flask with WebSocket
    socketio = SocketIO(app, cors_allowed_origins="*", ping_timeout=30, ping_interval=10, transports=["websocket"])
