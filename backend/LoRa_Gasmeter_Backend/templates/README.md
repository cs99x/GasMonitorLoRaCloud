# Gas Meter Dashboard and Historical Data Viewer

## Overview

This project contains two HTML files designed for monitoring and visualizing gas sensor data in real-time and as historical data. These dashboards are built using Bootstrap for styling, Chart.js for graphical data representation, and Socket.IO for real-time updates.

### Files

1. **Historical Data Viewer**
2. **Gas Meter Dashboard**

---

## Historical Data Viewer

### Description

The Historical Data Viewer displays historical sensor data for various gases. Users can choose different time ranges to view the data and analyze trends for the following gases:
- Methane (CH4)
- Carbon Monoxide (CO)
- Liquefied Petroleum Gas (LPG)
- Hydrogen (H2)

### Features

- **Time Range Selection:** Buttons to fetch data for the last 1 hour, last 6 hours, or all available data.
- **Dynamic Chart Updates:** Data is dynamically fetched and displayed using Chart.js.
- **Error Handling:** Alerts for cases when no data is available or an API error occurs.

### Technologies Used

- **Bootstrap:** For responsive styling.
- **Chart.js:** For visualizing sensor data in a line chart.
- **Fetch API:** To request data from the API endpoint.

### File Details

- **`chart-container` Divs:** Displays individual charts for each gas.
- **Time Range Buttons:** Trigger data fetching for specific time ranges.

---

## Gas Meter Dashboard

### Description

The Gas Meter Dashboard provides a real-time view of sensor readings. It connects to a backend server via Socket.IO to display live updates of gas levels and their respective thresholds.

### Features

- **Real-Time Data Updates:** Updates gas levels dynamically via WebSockets.
- **Interactive Sensor Cards:** Flip-card design displaying gas details (front) and descriptions (back).
- **Progress Bars:** Visualizes gas levels as a percentage of the danger threshold.
- **Battery Status Display:** Shows the current battery level of the device.

### Technologies Used

- **Bootstrap:** For responsive styling.
- **Socket.IO:** For real-time communication.
- **Dynamic Card Generation:** Sensor cards are dynamically added or updated as new data is received.

### File Details

- **Gas Info Cards:** Each card represents a gas sensor with current values and threshold progress.
- **Back of Cards:** Displays detailed gas descriptions and danger levels.
- **Navbar:** Navigation buttons for switching between pages and viewing additional graphs.

---

# API Endpoints

- **Historical Data Viewer:**
Endpoint: http://stuttgart-int.de/get-data-from-timestamp
Parameters:
    system_time: UNIX timestamp to fetch data from.

- **Gas Meter Dashboard:**
Uses Socket.IO for real-time updates.

## Historical Data Viewer

Displays interactive line charts for Methane (CH4), Carbon Monoxide (CO), LPG, and Hydrogen (H2).

## Gas Meter Dashboard

Real-time gas level updates with color-coded progress bars and flip-card design for detailed information.

## License

This project is open source and available under the MIT License.

## Sources
- https://getbootstrap.com/
- https://www.chartjs.org/
- https://socket.io/
- https://www.djangoproject.com/
- https://developer.mozilla.org/
