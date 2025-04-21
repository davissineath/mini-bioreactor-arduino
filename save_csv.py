import serial
import csv
import time
from datetime import datetime, timedelta

# Update these to match your actual ports
arduino_do_temp = serial.Serial('COM3', 9600, timeout=2)
arduino_ph = serial.Serial('COM4', 9600, timeout=2)

time.sleep(2)  # Allow time for reset

end_time = time.time() + 600  # 10 minutes
filename = "water_quality_log.csv"

with open(filename, "w", newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Sensor", "Arduino Timestamp (ms)", "Value 1", "Value 2 (if any)", "PC Timestamp (s)"])

    while time.time() < end_time:
        now = time.time()
        line_a = arduino_do_temp.readline().decode().strip()
        line_b = arduino_ph.readline().decode().strip()

        def write_line(line):
            if line and (line.startswith("DO") or line.startswith("PH")):
                parts = line.split(",")
                if len(parts) >= 3:
                    parts.append(now)  # Add PC timestamp
                    writer.writerow(parts)
                    print(parts)

        write_line(line_a)
        write_line(line_b)

print("✅ Logging complete. File saved as:", filename)
