#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and
2 line display
#define SensorPin A0 // pH meter Analog output to Arduino Analog Input 0
#define LED 13 //led screen connected to digital pin 13
#define ArrayLenth 40 // times of collection
int pHArray[ArrayLenth]; // Store the average value of the sensor feedback
int pHArrayIndex = 0;
const int PUMP_PIN = 12; // pump is connected to digital pin 12
const int MOTOR_PIN = 11; // stepper motor is connected to digital pin 11
int duty; //value from 0-255, indicates strength of signal given to pump
float target_pH = 7.0; //the pH we want the biofermenter to maintain
float last_pH;
float error; //current pH - target pH
float last_error;
float delta_error; //change in error
float control_variable; //aka the duty
float control = 0;
float fuzzyController(float error, float delta_error, float ph_act) {
//define membership functions
//membership functions that tell us how large the error is
float NL = max(0, min(1, -error / 2.0)); //negative large
float NS = max(0, min(1, -error)); //negative small
float Z = max(0, 1 - abs(error)); //zero
float PS = max(0, min(1, error)); //positive small
float PL = max(0, min(1, error / 2.0)); //positive large
//membership functions which tell us how much our error has changed since the last
step
float N = max(0, min(1, -delta_error)); //negative
float ZD = max(0, 1 - abs(delta_error)); //zero difference
float P = max(0, min(1, delta_error)); //positive
// define fuzzy rules
float DF = max(NL * N, NL * ZD); //decrease fast (not possible with our setup)
float DS = max(NL * P, NS * N); //decrease slow (not possible with our setup)
float NC = max(NS * ZD, Z * ZD); //no change
float IS = max(Z * P, PS * N); //increase slow
float IF = max(PS * ZD, PL * N); //increase fast
//defuzzify by adding up all the contributions to find our duty (control) value
float sum = IF + IS + NC + DS + DF;
if (sum != 0) {
control = (180 * IF + 100 * IS + 0 * NC + 0 * DS + 0 * DF) / sum; //180 = ds, 120 =
df
} else {
control = 0;
}
// Implement deadband logic, we dont want to have the pump on while we're super close
to our target pH
const float pH_TOLERANCE = 0.1; // target_ph - pH_TOLERANCE = the point at which we
want to shut off the pump to prevent any overshoot
if (fabs(error) <= pH_TOLERANCE || ph_act >= 7) {
control = 0;
} else {
const int MIN_DUTY_CYCLE = 91; //the pump only spins if duty is ~ >90
if (control < MIN_DUTY_CYCLE) {
control = MIN_DUTY_CYCLE;
}
}
return constrain(control, 0, 255); //can only analogwrite values from 0-255
}
void setup(void) {
pinMode(LED, OUTPUT);
pinMode(PUMP_PIN, OUTPUT);
Serial.begin(9600);
Serial.println("initialized"); // Test the serial monitor
//initialize the lcd
lcd.init();
lcd.backlight();
lcd.setCursor(0,0);
}
void loop(void) {
static float voltage;
//get pH value
float calibration_value = 27.87; //change this value to calibrate the pH sensor
unsigned long int avgval;
int buffer_arr[10], temp;
//take the average analogRead value from 10 readings
for (int i = 0; i < 10; i++) {
buffer_arr[i] = analogRead(SensorPin);
delay(30);
}
for (int i = 0; i < 9; i++) {
for (int j = i + 1; j < 10; j++) {
if (buffer_arr[i] > buffer_arr[j]) {
temp = buffer_arr[i];
buffer_arr[i] = buffer_arr[j];
buffer_arr[j] = temp;
}
}
}
avgval = 0;
for (int i = 2; i < 8; i++){
avgval += buffer_arr[i];
}
float volt = (float)avgval * 5.0 / 1024 / 6; //find average voltage
float ph_act = -5.70 * volt + calibration_value; //then use that voltage to find our
pH value
//call on fuzzy logic function to adjust pH to our target value
error = target_pH - ph_act;
delta_error = error - last_error;
control_variable = fuzzyController(error, delta_error, ph_act);
analogWrite(PUMP_PIN, 255);
last_error = error;
//print all useful information to the serial monitor
Serial.print("{'pH':");
Serial.print(ph_act);
Serial.print(", ");
Serial.print("duty: ");
Serial.print(control_variable);
Serial.print(", ");
Serial.print("Error:");
Serial.print(error);
Serial.print("}");
Serial.println();
// print the pH Value to the LCD screen
lcd.setCursor(0,1);
lcd.print("pH: ");
lcd.print(ph_act);
delay(500);
}
