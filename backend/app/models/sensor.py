from sqlalchemy import Column, Integer, Float, DateTime
from app.db.base import Base
from datetime import datetime

class SensorData(Base):
    __tablename__ = "sensor_data"

    id = Column(Integer, primary_key=True, index=True)
    timestamp = Column(DateTime, default=datetime.utcnow)
    sensor_value = Column(Float, nullable=False)
