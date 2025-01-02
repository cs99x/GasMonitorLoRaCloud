from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS  # Import CORS
from sqlalchemy.exc import OperationalError
import datetime
import logging

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})  # Enable CORS for all routes

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

# Database configuration for SQLite
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor_data.db'  # Use SQLite
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['SQLALCHEMY_ECHO'] = True

db = SQLAlchemy(app)

# Define the model
class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(50))
    timestamp = db.Column(db.DateTime)
    battery_pct = db.Column(db.Integer)
    battery_chg = db.Column(db.Boolean)
    sensors = db.Column(db.Text)  # Store JSON as string
    temperature = db.Column(db.Text)  # Store JSON as string
    status = db.Column(db.Text)  # Store JSON as string

    def __init__(self, device_id, timestamp, battery_pct, battery_chg, sensors, temperature, status):
        self.device_id = device_id
        self.timestamp = timestamp
        self.battery_pct = battery_pct
        self.battery_chg = battery_chg
        self.sensors = sensors
        self.temperature = temperature
        self.status = status

# Enable request logs like Flask's development server
if not app.debug:
    handler = logging.StreamHandler()
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)

    app.logger.handlers = [handler]
    app.logger.setLevel(logging.DEBUG)

@app.before_request
def log_request_info():
    app.logger.info('%s %s', request.method, request.url)

# Root endpoint
@app.route('/')
def index():
    return jsonify({"message": "Welcome to the Sensor Data API. Use /receive-json to POST data or /get-last-entity to GET the last entry."})

@app.route('/receive-json', methods=['POST'])
def receive_json():
    try:
        data = request.get_json()
        # Parse the JSON data
        device_id = data['id']
        timestamp = datetime.datetime.strptime(data['ts'], '%Y-%m-%dT%H:%M:%SZ')
        battery_pct = data['batt']['pct']
        battery_chg = data['batt']['chg']
        sensors = str(data['sensors'])  # Convert to string
        temperature = str(data['temp'])  # Convert to string
        status = str(data['stat'])  # Convert to string

        # Create a new SensorData object
        sensor_data = SensorData(device_id, timestamp, battery_pct, battery_chg, sensors, temperature, status)

        # Add to the session and commit
        db.session.add(sensor_data)
        db.session.commit()

        return jsonify({"received": data}), 200
    except OperationalError as e:
        app.logger.error(f"Database error: {e}")
        return jsonify({"error": "Database error. Please check logs."}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

@app.route('/get-last-entity', methods=['GET'])
def get_last_entity():
    try:
        last_entity = SensorData.query.order_by(SensorData.id.desc()).first()
        if last_entity:
            data = {
                "device_id": last_entity.device_id,
                "timestamp": last_entity.timestamp,
                "battery_pct": last_entity.battery_pct,
                "battery_chg": last_entity.battery_chg,
                "sensors": eval(last_entity.sensors),  # Convert back to dictionary
                "temperature": eval(last_entity.temperature),  # Convert back to dictionary
                "status": eval(last_entity.status)  # Convert back to dictionary
            }
            return jsonify(data), 200
        else:
            return jsonify({"error": "No data found"}), 404
    except OperationalError as e:
        app.logger.error(f"Database error: {e}")
        return jsonify({"error": "Database error. Please check logs."}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {e}")
        return jsonify({"error": "An unexpected error occurred."}), 500

if __name__ == '__main__':
    # Initialize the database and create tables
    with app.app_context():
        try:
            app.logger.info("Ensuring database tables exist...")
            db.create_all()  # Create tables if they don't exist
            app.logger.info("Database and tables created successfully.")
        except OperationalError as e:
            app.logger.error(f"Database error during initialization: {e}")
            exit(1)
        except Exception as e:
            app.logger.error(f"Unexpected error during database initialization: {e}")
            exit(1)

    # Run the Flask application
    app.run(debug=True, host='0.0.0.0', port=8000)
