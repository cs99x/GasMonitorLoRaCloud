from fastapi import APIRouter, Depends, UploadFile, File, HTTPException
from sqlalchemy.orm import Session
import json
from app.db.base import get_db
from app.schemas.sensor import SensorDataCreate, SensorDataResponse
from app.crud.sensor import create_sensor_data, get_all_sensor_data

router = APIRouter()

@router.post("/sensor/upload", response_model=dict)
async def upload_sensor_data(file: UploadFile = File(...), db: Session = Depends(get_db)):
    try:
        # Load the JSON file
        data = json.loads(await file.read())
        sensor_values = data.get("sensor_values", [])
        
        # Validate and store each sensor value
        for value in sensor_values:
            create_sensor_data(db, SensorDataCreate(sensor_value=value))
        
        return {"message": "Data uploaded successfully"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error processing file: {str(e)}")

@router.get("/sensor", response_model=list[SensorDataResponse])
def get_sensor_data(db: Session = Depends(get_db)):
    return get_all_sensor_data(db)
