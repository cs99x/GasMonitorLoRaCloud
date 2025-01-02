from LoRa_Gasmeter_Backend.app import app, db

with app.app_context():
    db.create_all()  # Ensure tables are created
