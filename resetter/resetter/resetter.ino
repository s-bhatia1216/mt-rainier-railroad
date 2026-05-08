void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // Print a message to the serial monitor:
  Serial.println("Hello World!");
  
  // Delay for 1 second (1000 milliseconds) to avoid flooding the monitor:
  delay(1000);
}