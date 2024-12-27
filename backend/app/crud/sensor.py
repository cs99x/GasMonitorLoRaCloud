from sqlalchemy.orm import Session
from app.models.sensor import SensorData
from app.schemas.sensor import SensorDataCreate

def create_sensor_data(db: Session, data: SensorDataCreate):
    new_data = SensorData(sensor_value=data.sensor_value)
    db.add(new_data)
    db.commit()
    db.refresh(new_data)
    return new_data

def get_all_sensor_data(db: Session):
    return db.query(SensorData).all()
