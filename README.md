# mini-bioreactor-arduino
This repository is the Arduino code associated with the USU BENG 3000 final project group for Emilia Huff, Isaac Smart, Davis Sineath, and Seth Wilkinson.

## Overview
This project uses the Arduino IDE an Python to read and save data points for pH, Temperature, and Dissolved Oxygen in the mini-bioreactor. 

### Workflow
##### Arduino probes set up: 
- pH calibration is done with pH standards and scaling the code output to match the standards<br />
- Dissolved Oxygen calibration is done using the script in do_calibration/ to find the saturation value at a recorded room temperature  
  
##### Data Collection: <br />
- In the Arduino IDE, open "./dissolved-oxygen-temperature.ino" and select the correct Arduino board. Likewise open "./pH.ino" and select the correct arduino board. Upload the scripts to the respective Arduinos. Do not open the serial monitor.
- In the "./save_csv.py" file, edit the end_time variable to the data collection length in seconds. Also edit the filename to name the output data and plots. 
- Run the "./save_csv.py"
- Once the script is complete, a csv file and png file will be saved into data/ 

### Mini-PC integration
- We downloaded the Arduino IDE and Visual Studio Code to run the scripts on the Mini-PC
- The Mini-PC used the same workflow as above
- To access the plots and data from the Mini-PC we emailed the files to the lab group



