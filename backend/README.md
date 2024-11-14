author: Lukas Bierlein

techs used:
Django 5.1.3
Python 3.11

django backend locally at 
C:\Users\DeCHEF\PycharmProjects\GasMonitorLoRaCloud\backend

Pros and Cons for Cloud Backend Solutions
1. FastAPI
Pros:
FastAPI could process even a lot of requests very quickly, often in under 100 ms, which is ideal for real-time sensor data. This is particularly important for the gas detection project, where quick notifications are needed when CO2 levels exceed safe thresholds.
FastAPI can process requests very fast (about 100ms), which is needed for the real-time sensor data.
Asynchronous Programming is supported, which means it can handle multiple requests simultaneously without waiting for one request to be completed before starting the next one. The detection should not be too slow, even when processing large volumes of sensor data in real time.
The documentation and tutorials are comprehensive, but still easy to understand. The basic functionalities required for this project are well documented and supported.
Cons:
The Framework itself is relatively new and does not have a large community like Flask or django. For the basic needs when encountering problems in this project it still should suffice.
Asynchronous Programming and concepts like async/await are pitfalls and will be a challenge. To use these it will take some time, but will be manageable.
Summary: FastAPI is a good choice for this project due to its speed, mostly ease of use, and support for real-time data processing. The most important needs of quick notification when gas levels become above a threshold and sending data fast continuously are fulfilled by FastAPI.

2. Flask
Pros:
The low amount of built-in features and simplicity make Flask an easy to use solution, especially for the smaller project (in terms of users and features needed).
As an already older framework it has a big and active community, which can provide support via the extensive documentation and tutorials. This makes it easy to find help when you encounter problems.
 Especially for smaller projects like the gasmeter its simplicity is ideal. Setting up a basic REST API for gas sensor data is straightforward and possible without too much time needed.
Cons:
Flask does not natively support asynchronous operations, meaning it may not be as fast as FastAPI when dealing with multiple concurrent requests. Even though for bigger data volume and high amount of requests this could be a problem and should be kept in mind if the code of the project wants to be reused for bigger projects (medium amount of scalability).
Summary: Flask is simple and easy to use, making it a solid choice for a learning project. Although it may not offer the performance of FastAPI, it should be sufficient for handling a small number of sensors in a basic setup.

3. Django
Pros:
Django provides a complete web framework with lots of built-in tools, including an ORM for easy database management and a ready-to-use admin interface. This could make data handling simpler for the gas meter project by automating database interactions and providing a way to review data without extra coding. It also includes built-in authentication, which could be useful if user login is needed in future versions. Django is well-documented and has a large community, making it easy to find help and resources.
Cons:
Compared to Flask and FastAPI, Django has a steeper learning curve, especially for small, specific projects like ours. Its full-featured design could also mean extra overhead that might slow down response times slightly, as it is less focused on lightweight API responses. Since the project is small, we may end up with more features than needed, which could make it harder to customize.
Summary:
Django offers a lot of built-in functionality that could simplify database management and security for the gas meter project, but it might be heavier and more complex than necessary for real-time notifications and small data volumes. Although the last point may be possible to diminish effect with focused work plus it would add scalability to the project.

For the database choice in general the focus was to work as smoothly as possible with the already safe other compartments of the project, especially Docker, bw 
4. MQTT (Message Queuing Telemetry Transport)
Pros:
MQTT has minimal delay, which makes it ideal for real-time data transmission. The frequent transmission of values by the sensors will be ensured by MQTTs near-instantaneous communication possibility.
It is optimized for devices with limited bandwidth or low power, which makes it suitable for low-cost sensors or devices.
Cons:
Using MQTT requires setting up a broker (e.g. Mosquitto) and configuring clients. This adds complexity to the project.
MQTT is not a standalone Cloud Backend Solution , it is solely a messaging protocol and does not handle database management or data storage. A separate backend framework, such as the previously mentioned, would still be necessary to process and store the data.
Summary: MQTT has real low-latency update times, though - up to a limit - this is not a priority.  Also, it is not a complete backend solution on its own, and other frameworks like FastAPI or Flask would still be needed to handle data storage and processing.

5. SQLite
SQLite is a relational database, which is sufficient for simple, structured data like the gasmeter project data.
Pros:
Lightweight and simple to set up
No server required
Ideal for small, local projects
Cons:
Limited scalability for larger projects - still usable up to a threshold of data, users and features - for current project size it’s perfectly fitting
only single write operation at a time
Not ideal for handling many users or high volumes of data
Summary:
SQLite is a great choice for small, simple projects that don't require high scalability or concurrency. It's perfect for the current project size. The data can be stored in a single file, which is very useful for local setups, especially when using the above mentioned Docker containers.

6. PostgreSQL
PostgreSQL is a relational database, which is sufficient for simple, structured data like the gasmeter project data.
Pros:
More scalable than SQLite
Handles multiple connections and concurrent writes
It has official Docker support
Cons:
Requires more resources and setup than SQLite
will be way more complex to configure initially
Summary:
PostgreSQL is a solid choice for projects that need scalability, real-time data storage, and support for complex queries. It would be still suitable if the project  grows but requires more setup than SQLite.

7. Firebase (Firestore)
Firestore is a NoSQL database, which is sufficient for simple, structured data like the gasmeter project data.
Pros:
Real-time synchronization of data
Fully managed service
no server setup is required
Built-in push notifications for mobile apps
Firestore supports real-time data syncing, making it ideal for mobile apps that need immediate updates. Since it’s fully managed, it doesn’t require any server maintenance, and it also integrates well with Firebase’s push notification services.
Cons:
Cloud-only solution (requires internet).
Learning curve for Firebase-specific tools.
Summary:
Firestore is a good option for real-time, cloud-based projects, especially if push notifications are needed and don’t want to manage server infrastructure. However, it’s best for cloud-based setups and is not  suitable for offline use.

8. Cassandra
PostgreSQL is a relational database, which is sufficient for simple, structured data like the gasmeter project data.
Pros:
Highly scalable, handles large data volumes
Supports high availability
Ideal for distributed systems with many sensors
Cons:
Complex to set up and manage
Overkill for this project
Not ideal for traditional SQL queries
Summary:
Cassandra’s setup and management require more effort compared to simpler databases like the above mentioned. It’s best suited for very large, distributed systems, so it is too complex for this small-scale project.
