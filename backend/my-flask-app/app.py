from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
import datetime

app = Flask(__name__)

# Database configuration
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor_data.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

# Define the model
class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(50))
    timestamp = db.Column(db.DateTime)
    battery_pct = db.Column(db.Integer)
    battery_chg = db.Column(db.Boolean)
    sensors = db.Column(db.JSON)
    temperature = db.Column(db.JSON)
    status = db.Column(db.JSON)

    def __init__(self, device_id, timestamp, battery_pct, battery_chg, sensors, temperature, status):
        self.device_id = device_id
        self.timestamp = timestamp
        self.battery_pct = battery_pct
        self.battery_chg = battery_chg
        self.sensors = sensors
        self.temperature = temperature
        self.status = status

@app.route('/receive-json', methods=['POST'])
def receive_json():
    data = request.get_json()
    
    # Parse the JSON data
    device_id = data['id']
    timestamp = datetime.datetime.strptime(data['ts'], '%Y-%m-%dT%H:%M:%SZ')
    battery_pct = data['batt']['pct']
    battery_chg = data['batt']['chg']
    sensors = data['sensors']
    temperature = data['temp']
    status = data['stat']
    
    # Create a new SensorData object
    sensor_data = SensorData(device_id, timestamp, battery_pct, battery_chg, sensors, temperature, status)
    
    # Add to the session and commit
    db.session.add(sensor_data)
    db.session.commit()
    
    return jsonify({"received": data}), 200

if __name__ == '__main__':
    with app.app_context():
        db.create_all()  # Create tables if they don't exist
    app.run(debug=True)