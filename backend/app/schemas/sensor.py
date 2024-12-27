from pydantic import BaseModel

class SensorDataCreate(BaseModel):
    sensor_value: float

class SensorDataResponse(BaseModel):
    id: int
    timestamp: str
    sensor_value: float

    class Config:
        from_attributes = True  # Replace 'orm_mode' with 'from_attributes'

