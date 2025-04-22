import serial
import pandas as pd
import time
import pandas as pd
import matplotlib.pyplot as plt

# Serial connections 
## These connections can be found in the Arduino IDE or using command line (They must be set to your computer)
arduino_do_temp = serial.Serial('/dev/tty.usbmodem1301', 9600, timeout=2)
arduino_ph = serial.Serial('/dev/tty.usbmodem1201', 9600, timeout=2)

time.sleep(2) 
start = time.time()

# User Defined Variables
end_time = time.time() + 600  # Edit this variable for the data collection period in seconds
filename = "task3" # Edit to name the outputted files, the name will be formatted to the path and file type automatically

data = []
while time.time() < end_time:
    do_temp_data = None
    ph_data = None

    # Keep reading until we get both DO/Temp and pH readings
    while do_temp_data is None or ph_data is None:
        line_a = arduino_do_temp.readline().decode().strip()
        line_b = arduino_ph.readline().decode().strip()

        # Parse DO/Temp line
        if line_a.startswith("DO"):
            parts = line_a.split(",")
            do_temp_data = {
                'DO': float(parts[1]),
                'Temp': float(parts[2])
            }

        # Parse pH line
        if line_b.startswith("pH"):
            parts = line_b.split(",")
            ph_data = {
                'pH': float(parts[1])
            }

        # In case they arrive in reverse
        if line_b.startswith("DO") and do_temp_data is None:
            parts = line_b.split(",")
            do_temp_data = {
                'DO': float(parts[1]),
                'Temp': float(parts[2])
            }

        if line_a.startswith("pH") and ph_data is None:
            parts = line_a.split(",")
            ph_data = {
                'pH': float(parts[1])
            }

    timestamp = time.time() - start

    # Merge the data into one row
    row = {
        'DO': do_temp_data['DO'],
        'Temp': do_temp_data['Temp'],
        'pH': ph_data['pH'],
        'Time': timestamp
    }

    data.append(row)

# Save the final data into a data frame
data = pd.DataFrame(data)
csv_file = "./data/" + filename + ".csv"
data.to_csv(csv_file, index=False)
print("Recording complete. File saved as: ", csv_file)

# Sample data
x = data['Time']
temp = data['Temp']
do = data['DO']
pH = data['pH']

# Create subplots
fig, axs = plt.subplots(3, sharex=True, figsize=(10, 8))

# Plot each parameter
axs[0].plot(x, temp)
axs[0].set_ylabel('Temp (°C)')
axs[0].set_title('Temperature')

axs[1].plot(x, do, color='tab:orange')
axs[1].set_ylabel('DO (microgram/L)')
axs[1].set_title('Dissolved Oxygen')

axs[2].plot(x, pH, color='tab:green')
axs[2].set_ylabel('pH')
axs[2].set_title('pH')
axs[2].set_xlabel('Time (s)')

plt.setp(axs[2].xaxis.get_majorticklabels(), rotation=45)

# Add grid to all
for ax in axs:
    ax.grid(True)

plt.tight_layout()

plot_name = "./data/plots/" + filename + ".png"
plt.savefig(plot_name)

print("Plot created. File saved as: ", plot_name)

plt.show()
