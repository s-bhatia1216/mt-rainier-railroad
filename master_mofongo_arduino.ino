#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

// ---------------- SENSORS ----------------
VL53L0X outerSensor;
VL53L0X innerSensor;

#define XSHUT_PIN_1 3 //inner is digital pin 2
#define XSHUT_PIN_2 2 //outer is digital pin 3

// ---------------- SERVOS ----------------
Servo innerServo;
Servo outerServo;

// ---------------- STATE ----------------
int innerState = 1;
int outerState = 1;

unsigned long innerTimer = 0;
unsigned long outerTimer = 0;

unsigned long innerInterval = 5000;
unsigned long outerInterval = 5000;

bool started = false;
unsigned long startTime = 0;

// ---------------- SEMANTIC FLAGS ----------------
bool inner_tree_lowered = true;
bool outer_tree_lowered = true;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  delay(3000);

  Serial.println("A: START");

  // servos
  innerServo.attach(11);
  outerServo.attach(10);

  innerServo.write(90); // lowered (inner)
  outerServo.write(0);  // lowered (outer)

  // sensors
  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(500);
  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();

  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(500);
  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();

  Serial.println("F: BOTH SYSTEMS RUNNING");

  startTime = millis();
}

// ---------------- LOOP ----------------
void loop() {

  // ================= SENSOR READ =================
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  bool outer_tree_fallen = (outerDist >= 130 && outerDist <= 170);
  bool inner_tree_fallen = (innerDist >= 80 && innerDist <= 120);
  bool both_tree_fallen = outer_tree_fallen && inner_tree_fallen;

  // ================= HOLD PHASE =================
  bool hold_phase = (!started && (millis() - startTime < 5000));

  if (!started && hold_phase) {

    Serial.print("hold_phase=1");

    return;
  }

  if (!started) {
    innerTimer = millis();
    outerTimer = millis();

    innerInterval = random(1000, 5000);
    outerInterval = random(5000, 10000);

    started = true;
  }

  // ================= INNER SERVO =================
  if (millis() - innerTimer >= innerInterval) {
    innerTimer = millis();

    innerState = !innerState;
    innerServo.write(innerState ? 90 : 0);

    inner_tree_lowered = innerState;

    innerInterval = random(5000, 10000);
  }

  // ================= OUTER SERVO =================
  if (millis() - outerTimer >= outerInterval) {
    outerTimer = millis();

    outerState = !outerState;

    // IMPORTANT: inverted mapping
    outerServo.write(outerState ? 0 : 90);

    outer_tree_lowered = outerState;

    outerInterval = random(5000, 10000);
  }

  // ================= FULL TELEMETRY OUTPUT =================
  Serial.print("inner_dist=");
  Serial.print(innerDist);

  Serial.print(" outer_dist=");
  Serial.print(outerDist);

  Serial.print(" | INNER_FALLEN=");
  Serial.print(inner_tree_fallen);

  Serial.print(" OUTER_FALLEN=");
  Serial.print(outer_tree_fallen);

  Serial.print(" | INNER_ACTUATED=");
  Serial.print(inner_tree_lowered);

  Serial.print(" OUTER_ACTUATED=");
  Serial.print(outer_tree_lowered);

  Serial.print(" BOTH FALLEN =");
  Serial.println(both_tree_fallen);
}