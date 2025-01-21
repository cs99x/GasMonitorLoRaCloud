
# Real-Time Sensor Data Monitoring Application

This project is a Flask application designed to monitor sensor data in real-time using Socket.IO and SQLAlchemy. It serves as a backend system for collecting, storing, and visualizing data from gas monitoring sensors.

## Features

- Real-time communication with WebSocket using Socket.IO.
- REST API for receiving, storing, and querying sensor data.
- Database management with SQLAlchemy and SQLite.
- Interactive front-end for visualizing real-time and historical sensor data.
- Configurable with CORS support for cross-origin requests.

## Technology Stack

- **Backend Framework**: Flask
- **WebSocket**: Flask-SocketIO
- **Database**: SQLAlchemy with SQLite
- **Front-End**: HTML, CSS (Bootstrap), and JavaScript (Chart.js, Socket.IO)
- **API Integration**: REST API for sensor data
- **Deployment**: Configured to run on `0.0.0.0:8000`

## Installation

### Prerequisites

- Python 3.8+
- pip (Python package installer)

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo-name/real-time-sensor-monitor.git
   cd real-time-sensor-monitor
   ```

2. Create a virtual environment and activate it:
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

3. Install the required packages:
   ```bash
   pip install -r requirements.txt
   ```

4. Set up the database:
   - Ensure the database path is correct in `app.py`:
     ```python
     app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///path/to/sensor_data.db'
     ```
   - Initialize the database:
     ```bash
     python app.py
     ```

5. Run the application:
   ```bash
   python app.py
   ```

6. Access the application in your browser at `http://localhost:8000`.

## Usage

### Endpoints

1. **Root Page (`/`)**: Displays the real-time sensor dashboard.
2. **Graph Page (`/graph`)**: Displays historical data visualizations.
3. **POST `/receive-json`**: Receives sensor data in JSON format.
   - Request Body Example:
     ```json
     {
       "pct": 90,
       "id": "handheld 1",
       "ts": 1673049150,
       "sensors": [{"type": "CH4", "val": 200}, {"type": "CO", "val": 1280}, {"type": "CO", "val": 726}, {"type": "CO", "val": 3856}]
     }
     ```
4. **GET `/get-last-entity`**: Retrieves the latest sensor data entry.
5. **GET `/get-data-from-timestamp`**: Retrieves data between the current and a specific timestamp.
   - Query Parameter: `system_time=<timestamp>`

### Real-Time Communication
WebSocket events:
- **`connect`**: Logs a client connection.
- **`disconnect`**: Logs a client disconnection.
- **`update_data`**: Sends real-time updates of the latest sensor data.

## Architecture

- **`app.py`**: Main application file with routes, WebSocket handlers, and background tasks.
- **Database**: SQLite database stores sensor data with a `SensorData` ORM model.
- **Templates**:
  - `index.html`: Displays real-time sensor data.
  - `graph.html`: Visualizes historical sensor data.

## Deployment

To deploy the application:
1. Set the `FLASK_ENV` to `production`.
2. Use a WSGI server like Gunicorn:
   ```bash
   gunicorn -w 4 -b 0.0.0.0:8000 app:app
   ```

## Future Improvements

- Implement user authentication for data access.
- Enhanced error handling and logging.
- Add features for further data processing like csv export and dark mode
- Add unit and integration tests.

## License

This project is licensed under the MIT License.

---

**Author**: [Maceo Pohl, Romeo Massoud, Christoph Schmid/LoRa-Gasmeter]
