import serial
import pandas as pd
import time
from datetime import datetime, timedelta

# Update these to match your actual ports
arduino_do_temp = serial.Serial('/dev/tty.usbmodem1301', 9600, timeout=2)
arduino_ph = serial.Serial('/dev/tty.usbmodem1201', 9600, timeout=2)

time.sleep(2)  # Allow time for reset

start = time.time()
end_time = time.time() + 32  # 10 minutes
filename = "water_data_log.csv"
data = pd.DataFrame(columns=['DO', 'Temp', 'pH', 'Time'])
index = 0 


while time.time() < end_time:
    index += 1
    now = time.time() - start
    line_a = arduino_do_temp.readline().decode().strip()
    line_b = arduino_ph.readline().decode().strip()

    if line_a.startswith("DO"):    
        parts = line_a.split(",")
        data.loc[index, 'DO'] = parts[1]
        data.loc[index, 'Temp'] = parts[2]
    if line_b.startswith("pH"):
        parts = line_b.split(",")
        data.loc[index, 'pH'] = parts[1]

    if line_b.startswith("DO"):    
        parts = line_b.split(",")
        data.loc[index, 'DO'] = parts[1]
        data.loc[index, 'Temp'] = parts[2]
    if line_a.startswith("pH"):
        parts = line_a.split(",")
        data.loc[index, 'pH'] = parts[1]


data.to_csv(filename)
print("✅ Logging complete. File saved as:", filename)
