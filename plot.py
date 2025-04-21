import pandas as pd
import matplotlib.pyplot as plt

# Read data
data = pd.read_csv("./water_data_log.csv")

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
plt.show()
