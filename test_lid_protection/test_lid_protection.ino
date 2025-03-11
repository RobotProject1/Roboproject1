#include <Wire.h>
#include <PID_v1.h>
#include <Servo.h>
#include "VEGA_MLX90614.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Servo myservo;
VEGA_MLX90614 mlx(18, 19);
Adafruit_SSD1306 display(4);

// Pin
const int touchpad1 = 0;
const int touchpad2 = 1;
const int touchpad3 = 2;
const int buzz = 3;

// Initial values
int valtp1 = 0;
int stat1 = 0;
int valtp3 = 0;
int stat3 = 0;

int valtp2 = 0;
int stat2 = 0;
int seropen = 135;
int serclose = 7.5;

const int SAMPLES = 10;
float s_val[SAMPLES];
float kt = 2;

// // LID PID variables and setup
// double lSetpoint, lInput, lOutput;
// double lKp = 2, lKi = 5, lKd = 1;
// PID lidPID(&lInput, &lOutput, &lSetpoint, lKp, lKi, lKd, DIRECT);

// // TEMP PID variables and setup
// double Setpoint, Input, Output;
// double Kp = 2, Ki = 5, Kd = 1;
// PID tempPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

float thresholdup = 1.2; // ไว้มาแก้
float thresholddown = 1.5; // ไว้มาแก้

void setup() {
  Serial.begin(9600);

  // Servo setup
  myservo.attach(6);
  myservo.write(serclose);

  // Pin modes
  pinMode(touchpad1, INPUT);
  pinMode(touchpad2, INPUT);
  // pinMode(touchpad3, INPUT);
  // pinMode(buzz, OUTPUT);

  // // Initial PID inputs
  // Input = mlx.mlx90614ReadTargetTempC(); // Temp input
  // // lInput = myservo.read();               // Servo current position

  // // PID mode activation
  // // lidPID.SetMode(AUTOMATIC);
  // tempPID.SetMode(AUTOMATIC);
}

void loop() {
  checkTouchpad2();       // Check touchpad and toggle state
  updateServoState();    // Update servo position based on state
}

// // Function to check touchpad and toggle state
// void checkTouchpad2() {
//   valtp2 = digitalRead(touchpad2);
//   if (valtp2 == 1) {
//     tone(buzz, 3000, 100);
//     stat2 = !stat2;
//     delay(100);
//   }
// }

void checkTouchpad2() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read input until newline
    command.trim(); // Remove extra spaces/newlines

    if (command == "toggle") { // Simulate button press
      tone(buzz, 3000, 100);
      stat2 = !stat2;
      Serial.print("Button Press Simulated! stat2 is : ");
      Serial.println(stat2);
    }
  }
}

void moveServo(int targetPos) {
  int currentPos = myservo.read();

  Serial.println("Moving servo...");

  if (currentPos < targetPos) {
    for (int pos = currentPos; pos <= targetPos-10; pos++) {
      myservo.write(pos);
      delay(8);  
    }
  } 
  if (currentPos > targetPos) {
    for (int pos = currentPos; pos >= targetPos; pos--) {
      myservo.write(pos);
      delay(12);
    }
  }
}


// float calibratethreshold(float mean, float sd) {
//   float threshold = mean + (kt * sd);
//   return threshold;
// }

// Function to read current from sensor
float readCurrent() {
  int adc = analogRead(A1);
  float voltage = adc * 5.0 / 1023.0;
  float current = (voltage - 2.5) / 0.185;  // Adjust 0.185 based on your sensor specs
  return current;
}

// float calculateMean(float arr[], int size) {
//   float sum = 0;
//   for (int i = 0; i < size; i++) {
//     sum += arr[i];
//   }
//   return sum / size;
// }

// float calculateSD(float arr[], int size, float mean) {
//   float sumSquaredDiffs = 0;
//   for (int i = 0; i < size; i++) {
//     sumSquaredDiffs += pow(arr[i] - mean, 2);
//   }
//   return sqrt(sumSquaredDiffs / size);  // Population SD (use (size-1) for sample SD)
// }

// Function to update servo state based on stat
void updateServoState() {
  float cur = readCurrent();  // Read the current value

  if (stat2 == 1) { // Lid is open
    if (cur < thresholddown && myservo.read() != seropen) {
      Serial.println("Closing lid");
      moveServo(serclose);
    } 
    else if (myservo.read() != seropen) {
      Serial.println("Opening lid due to obstacle...");
      moveServo(seropen);
    }
  } 
  else { // Lid is closed
    if (cur < thresholdup && myservo.read() != serclose) {
      Serial.println("Opening lid");
      moveServo(seropen);
    } 
    else if (myservo.read() != serclose) {
      Serial.println("Closing lid due to obs");
      moveServo(serclose);
    }
  }
}