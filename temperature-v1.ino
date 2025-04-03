#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int SENSOR_PIN = A0; //temperature sensor connected to analog pin 0
const int PWM_PIN = 12; //heater mostfet connected to digital pin 12

//set up variables for PID control
float Kc = 12.06; //non rigorously defined values that also sort of worked (just in case): 
Kc = 10, Ti = 170, Td = 0
float Ti = 152.703;
float Td = 38.175;
float MV = 0; //manipulated variable: 0-100% duty cycle
float lastMV = MV;
float dMV = 0; //'delta MV'
float duty = 0;
float CVinit = 22.00; //initial temp value of solution in biofermenter flask
float CVm = CVinit; //controlled variable: temperature
float lastCVm = CVinit;
float lastlastCVm = CVinit;
//initialize values for PID calculation
float currentTime;
float previousTime = -1;
float elapsedTime = 1;
float lastError = 0;
float error = 0;
float SP_val = 21.11; // desired temperature
float tempCelsius;
float tempFahrenheit;

//PID function
float calcPID(float CV_input, float SP_input) {
currentTime = millis();
elapsedTime = float(currentTime - previousTime);
error = SP_input - CV_input;
dMV = Kc * (error - lastError + elapsedTime / Ti * error - Td / elapsedTime *
(CV_input - 2 * lastCVm + lastlastCVm)); //calculate velocity form for MV
float MV_out = lastMV + dMV; //define the MV to be passed to our control system
lastError = error; //update previous values
lastCVm = CV_input;
lastlastCVm = lastCVm;
return MV_out;
}

OneWire oneWire(SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);
void setup() {
dMV = 0;
lcd.init(); //initilize LED screen
lcd.backlight();
lcd.setCursor(0, 0);
tempSensor.begin(); //initialize temp sensor
pinMode(PWM_PIN, OUTPUT);
tempSensor.requestTemperatures(); //initialize variables for PID control
tempCelsius = tempSensor.getTempCByIndex(0);
CVm = tempCelsius;
lastCVm = CVm;
lastlastCVm = CVm;
Serial.begin(9600);
}

void loop() {
//calculate MV
MV = calcPID(CVm, SP_val);
//constrain our MV to values between 0-100
if (MV < 0) {
MV = 0;
}
if (MV > 100) {
MV = 100;
}
lastMV = MV;
duty = MV / 100 * 255;
analogWrite(PWM_PIN, duty); //adjust the heater accordingly

//find the temperature value
tempSensor.requestTemperatures();
tempCelsius = tempSensor.getTempCByIndex(0);
CVm = tempCelsius;
tempFahrenheit = (tempCelsius * (9/5)) + 32

//print information on the LCD screem
lcd.setCursor(0, 0);
lcd.print("Temperature: ");
lcd.setCursor(0, 1);
lcd.print(tempCelsius);
lcd.setCursor(8, 1);
lcd.print("C");
lcd.setCursor(0, 2);
lcd.setCursor(0, 3);
lcd.print(tempFahrenheit);
lcd.setCursor(6, 3);
lcd.print("F");
Serial.print("{'MV_pct':"); // Print the MV value in % sent to the final control
element
Serial.print(MV);
Serial.print(", 'dMV':"); // Print the dMV value (change in MV calculated by PID
algorithm)
Serial.print(dMV);
Serial.print(", 'sensor val':"); // Print the sensor value connected to sensor input
Serial.print(CVm);
Serial.print(", 'set point':"); // Print the set point value
Serial.print(SP_val);
Serial.print(", 'duty':"); // Print the duty value (0-255) sent to the heater
Serial.print(duty);
Serial.print(", 'error':"); // Print the calculated error between set point and CV
(SP-CV)
Serial.print(error);
Serial.print("}");
Serial.println();
previousTime = currentTime;
delay(500);
}














