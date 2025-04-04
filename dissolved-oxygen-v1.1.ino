#include <Arduino.h>

#define DO_PIN A1

#define VREF 5000    //VREF (mv)
#define ADC_RES 1024 //ADC Resolution

//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 0

#define READ_TEMP (25) //Current water temperature ℃, Or temperature sensor function

//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (1600) //mv
#define CAL1_T (25)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Temperaturet = (uint8_t)READ_TEMP;
  ADC_Raw = analogRead(DO_PIN);
  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;

  Serial.print("Temperaturet:\t" + String(Temperaturet) + "\t");
  Serial.print("ADC RAW:\t" + String(ADC_Raw) + "\t");
  Serial.print("ADC Voltage:\t" + String(ADC_Voltage) + "\t");
  Serial.println("DO:\t" + String(readDO(ADC_Voltage, Temperaturet)) + "\t");

  delay(1000);
}


//DO2 CODE GIVEN BELOW
// #include <OneWire.h> // what does this do?
// #include <LiquidCrystal_I2C.h> //for the monitor
// #include <Wire.h> // what does this do?

// LiquidCrystal_I2C lcd(0x27, 20, 4); // defines the monitor values

// const int SENSOR_PIN = A1; //DO2 sensor connected to analog pin 1
// const int PWM_PIN = 12; //digital pin 12, could be used depending on probe? 

// //set up variables for PID control 
// float Kc = 12.06; //non rigorously defined values that also sort of worked (just in case):
// Kc = 10, Ti = 170, Td = 0
// float Ti = 152.703;
// float Td = 38.175;
// float MV = 0; //manipulated variable: 0-100% duty cycle
// float lastMV = MV;
// float dMV = 0; //'delta MV'
// float duty = 0;
// float CVinit = 22.00; //initial temp value of solution in biofermenter flask
// float CVm = CVinit; //controlled variable: temperature
// float lastCVm = CVinit;
// float lastlastCVm = CVinit;
// //initialize values for PID calculation
// float currentTime;
// float previousTime = -1;
// float elapsedTime = 1;
// float lastError = 0;
// float error = 0;
// float SP_val = 21.11; // desired temperature
// float tempCelsius;
// float tempFahrenheit;

// //PID function
// // A PID controller seeks to keep some input variable close to a desired setpoint by adjusting an output. The way in which it does this can be 'tuned' by adjusting three parameters (P,I,D).
// float calcPID(float CV_input, float SP_input) {
// currentTime = millis();
// elapsedTime = float(currentTime - previousTime);
// error = SP_input - CV_input;
// dMV = Kc * (error - lastError + elapsedTime / Ti * error - Td / elapsedTime *
// (CV_input - 2 * lastCVm + lastlastCVm)); //calculate velocity form for MV
// float MV_out = lastMV + dMV; //define the MV to be passed to our control system
// lastError = error; //update previous values
// lastCVm = CV_input;
// lastlastCVm = lastCVm;
// return MV_out;
// }

// OneWire oneWire(SENSOR_PIN);
// DallasTemperature tempSensor(&oneWire); //check what the DallasTemperature library does

// void setup() {
// dMV = 0;

// //initilize LED screen
// lcd.init(); 
// lcd.backlight();
// lcd.setCursor(0, 0);
// tempSensor.begin(); //initialize temp sensor 
// pinMode(PWM_PIN, OUTPUT);
// tempSensor.requestTemperatures(); //initialize variables for PID control
// tempCelsius = tempSensor.getTempCByIndex(0);
// CVm = tempCelsius;
// lastCVm = CVm;
// lastlastCVm = CVm;
// Serial.begin(9600);
// }

// void loop() {
// //calculate MV
// MV = calcPID(CVm, SP_val);
// //constrain our MV to values between 0-100
// if (MV < 0) {
// MV = 0;
// }
// if (MV > 100) {
// MV = 100;
// }
// lastMV = MV;
// duty = MV / 100 * 255;
// analogWrite(PWM_PIN, duty); //adjust the heater accordingly

// //find the temperature value
// tempSensor.requestTemperatures();
// tempCelsius = tempSensor.getTempCByIndex(0);
// CVm = tempCelsius;
// tempFahrenheit = (tempCelsius * (9/5)) + 32

// //print information on the LCD screem
// lcd.setCursor(0, 0);
// lcd.print("Temperature: ");
// lcd.setCursor(0, 1);
// lcd.print(tempCelsius);
// lcd.setCursor(8, 1);
// lcd.print("C");
// lcd.setCursor(0, 2);
// lcd.setCursor(0, 3);
// lcd.print(tempFahrenheit);
// lcd.setCursor(6, 3);
// lcd.print("F");
// Serial.print("{'MV_pct':"); // Print the MV value in % sent to the final control
// element
// Serial.print(MV);
// Serial.print(", 'dMV':"); // Print the dMV value (change in MV calculated by PID
// algorithm)
// Serial.print(dMV);
// Serial.print(", 'sensor val':"); // Print the sensor value connected to sensor input
// Serial.print(CVm);
// Serial.print(", 'set point':"); // Print the set point value
// Serial.print(SP_val);
// Serial.print(", 'duty':"); // Print the duty value (0-255) sent to the heater
// Serial.print(duty);
// Serial.print(", 'error':"); // Print the calculated error between set point and CV
// (SP-CV)
// Serial.print(error);
// Serial.print("}");
// Serial.println();
// previousTime = currentTime;
// delay(500);
// }
