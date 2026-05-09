#include <Wire.h>
#include <VL53L0X.h>

VL53L0X outerSensor;
VL53L0X innerSensor;

#define XSHUT_PIN_1 3 //outer sensor is at pin 3
#define XSHUT_PIN_2 2 //inner sensor is at pin 2

void setup() {
  Serial.begin(57600);
  delay(3000);

  Serial.println(F("A: START"));

  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  // Bring up sensor 1 alone, assign 0x30
  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(500);
  Serial.println(F("B: OUTER SENSOR UP"));
  outerSensor.setTimeout(500);
  if (!outerSensor.init()) {
    Serial.println(F("OUTER SENSOR FAIL"));
    while (1);
  }
  outerSensor.setAddress(0x30);
  Serial.println(F("C: OUTER SENSOR AT 0x30"));

  // Bring up sensor 2 alone, assign 0x31
  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(500);
  Serial.println(F("D: INNER SENSOR UP"));
  innerSensor.setTimeout(500);
  if (!innerSensor.init()) {
    Serial.println(F("INNER SENSOR FAIL"));
    while (1);
  }
  innerSensor.setAddress(0x31);
  Serial.println(F("E: INNER SENSOR AT 0x31"));

  outerSensor.startContinuous();
  innerSensor.startContinuous();
  Serial.println(F("F: BOTH RUNNING"));
}

void loop() {
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  // ---- FLAGS ----
  bool outer_tree_fallen = (outerDist >= 130 && outerDist <= 170);
  bool inner_tree_fallen = (innerDist >= 80 && innerDist <= 120);
  bool both_tree_fallen = outer_tree_fallen && inner_tree_fallen;

  // ---- PRINT SENSOR VALUES ----
  Serial.print(F("Outer Sensor 1: "));
  Serial.print(outerDist);
  Serial.print(F("mm  |  Inner Sensor 2: "));
  Serial.print(innerDist);
  Serial.print(F("mm"));

  // ---- OPTIONAL TEXT FLAGS ----
  if (outer_tree_fallen) {
    Serial.print(F("  |  OUTER TREE FALLEN"));
  }

  if (inner_tree_fallen) {
    Serial.print(F("  |  INNER TREE FALLEN"));
  }

  // ---- BOOLEAN DEBUG OUTPUT ----
  Serial.print(F("  |  outer_tree_fallen="));
  Serial.print(outer_tree_fallen);

  Serial.print(F(" inner_tree_fallen="));
  Serial.print(inner_tree_fallen);

  Serial.print(F(" both_tree_fallen="));
  Serial.print(both_tree_fallen);

  Serial.println();

  delay(300);
}