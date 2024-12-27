from fastapi import FastAPI
from app.db.base import Base, engine
from app.routes import sensor
import sys
print("Python Path:", sys.path)

# Create the database tables
Base.metadata.create_all(bind=engine)

# Initialize FastAPI app
app = FastAPI()

# Include routes
app.include_router(sensor.router, prefix="/api/v1", tags=["Sensor Data"])
