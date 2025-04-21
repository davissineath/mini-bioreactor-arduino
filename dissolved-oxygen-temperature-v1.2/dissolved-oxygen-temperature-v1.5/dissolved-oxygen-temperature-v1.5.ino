//Sources
//https://github.com/DFRobot/Gravity_Analog_Dissolved_Oxygen_Sensor
//dO2 code is from: https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237#target_3
//ChatGPT combined temp and dO2 scripts
//Description
//Combined script of dO2 (dissolved oxygen) and temperature readings for a single screen output which displays both


#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// === LCD Setup ===
LiquidCrystal_I2C lcd(0x27, 20, 4);

// === Pin Definitions ===
const int TEMP_SENSOR_PIN = A0;
const int DO_SENSOR_PIN = A1;
const int HEATER_PIN = 12;

// === PID Parameters ===
float Kc = 10.0;
float Ti = 170.0;
float Td = 0.0;
float MV = 0, lastMV = 0, dMV = 0;
float duty = 0;
float CVinit = 22.00;
float CVm = CVinit, lastCVm = CVinit, lastlastCVm = CVinit;
float currentTime = 0, previousTime = -1, elapsedTime = 1;
float lastError = 0, error = 0;
float SP_val = 21.11;

// === DO Calibration ===

//VREF is 3300 instead of suggested 5000
#define VREF 3300    // mV
#define ADC_RES 1024 // ADC Resolution
#define TWO_POINT_CALIBRATION 0 //This means we use one point calibration

//#define READ_TEMP (25) //This is removed in combined function

#define CAL1_V (1600) // mV
#define CAL1_T (25)   // °C
#define CAL2_V (1300) // mV
#define CAL2_T (15)   // °C

const uint16_t DO_Table[41] = {
14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
};

// === Sensor Setup ===
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

// === Functions ===
float calcPID(float CV_input, float SP_input) {
currentTime = millis();
elapsedTime = float(currentTime - previousTime);
error = SP_input - CV_input;
dMV = Kc * (error - lastError + elapsedTime / Ti * error - Td / elapsedTime * (CV_input - 2 * lastCVm + lastlastCVm));
float MV_out = lastMV + dMV;
lastError = error;
lastCVm = CV_input;
lastlastCVm = lastCVm;
return MV_out;
}

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c) {
#if TWO_POINT_CALIBRATION == 0
//We can use two point calibration by changing the definition to 1 instead of 0
uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

// === Arduino Setup ===
void setup() {
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
tempSensor.begin();
pinMode(HEATER_PIN, OUTPUT);
tempSensor.requestTemperatures();
CVm = tempSensor.getTempCByIndex(0);
lastCVm = CVm;
lastlastCVm = CVm;
Serial.begin(9600);
}

void loop() {
// === Temperature Reading ===
tempSensor.requestTemperatures();
float tempCelsius = tempSensor.getTempCByIndex(0);
CVm = tempCelsius;
float tempFahrenheit = (tempCelsius * 9.0 / 5.0) + 32.0;

// === PID Heater Control ===
MV = calcPID(CVm, SP_val);
MV = constrain(MV, 0, 100);
lastMV = MV;
duty = MV / 100.0 * 255;
analogWrite(HEATER_PIN, duty);

// === DO Sensor Reading ===
uint16_t adc_raw = analogRead(DO_SENSOR_PIN);
// uint16_t adc_mv = uint32_t(VREF) * adc_raw / ADC_RES;
// uint16_t DO_value = readDO(adc_mv, uint8_t(tempCelsius));


float DO_value = adc_raw; //* calibration_slope + calibration_offset;


// === LCD Output ===
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Temp: "); lcd.print(tempCelsius); lcd.print(" C");
lcd.setCursor(0, 1);
lcd.print("DO: "); lcd.print(DO_value); lcd.print(" ug/L");

// === Serial Output ===
Serial.print("DO,"); Serial.print(DO_value); Serial.print(","); Serial.print(tempCelsius);
Serial.println();


previousTime = currentTime;
delay(1000);
}
