// #include <Wire.h>

// void setup() {
//   Serial.begin(9600);
//   Wire.begin();

//   Serial.println("\nI2C Scanner Starting...");
// }

// void loop() {
//   byte error, address;
//   int count = 0;

//   for (address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("Device found at 0x");
//       Serial.println(address, HEX);
//       count++;
//     }
//   }

//   if (count == 0) Serial.println("No I2C devices found");
//   else Serial.println("Scan complete");

//   delay(3000);
// }

// #include "Adafruit_VL53L0X.h"

// Adafruit_VL53L0X lox;

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   Serial.println(F("VL53L0X booting..."));

//   if (!lox.begin()) {
//     Serial.println(F("INIT FAILED"));
//     while (1); // stop here
//   }

//   Serial.println(F("INIT OK"));
// }

// void loop() {
//   VL53L0X_RangingMeasurementData_t measure;

//   lox.rangingTest(&measure, false);

//   if (measure.RangeStatus != 4) {
//     Serial.print(F("mm: "));
//     Serial.println(measure.RangeMilliMeter);
//   } else {
//     Serial.println(F("OUT"));
//   }

//   delay(250);
// }

// #include "Adafruit_VL53L0X.h"

// Adafruit_VL53L0X lox;

// #define XSHUT 2

// void setup() {
//   Serial.begin(9600);
//   delay(1500);

//   pinMode(XSHUT, OUTPUT);
//   digitalWrite(XSHUT, HIGH);   // <-- THIS IS REQUIRED

//   delay(100);

//   Serial.println("start");

//   if (!lox.begin()) {
//     Serial.println("fail");
//     while (1);
//   }

//   Serial.println("ok");
// }

// void loop() {
//   VL53L0X_RangingMeasurementData_t m;
//   lox.rangingTest(&m, false);

//   Serial.println(m.RangeMilliMeter);
//   delay(200);
// }

///////////////////
// void setup() {
//   Serial.begin(9600);
//   delay(2000);
//   Serial.println("BOOT OK");
// }

// void loop() {
//   Serial.println("RUN");
//   delay(1000);
// }

#include <Wire.h>
#include <VL53L0X.h>  // Pololu library

VL53L0X sensor;

#define XSHUT_PIN 2

void setup() {
  Serial.begin(57600);
  delay(3000);

  Serial.println(F("A: START"));

  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, LOW);
  delay(50);
  digitalWrite(XSHUT_PIN, HIGH);
  delay(200);

  Serial.println(F("B: XSHUT HIGH"));

  Wire.begin();

  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println(F("C: INIT FAIL"));
    while (1);
  }

  sensor.startContinuous();
  Serial.println(F("D: INIT OK"));
}

void loop() {
  uint16_t dist = sensor.readRangeContinuousMillimeters();

  if (!sensor.timeoutOccurred()) {
    Serial.print(F("DIST mm: "));
    Serial.println(dist);
  } else {
    Serial.println(F("TIMEOUT"));
  }

  delay(300);
}