// HC-SR04 + LED
const int TRIG = 9;
const int ECHO = 10;
const int LED  = 13;

float readDistance() {
  // Trigger a 10 µs pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Measure echo pulse width
  long duration = pulseIn(ECHO, HIGH, 30000); // 30 ms timeout

  // No echo received
  if (duration == 0) return -1;

  // Convert time to distance in cm
  return (duration * 0.034f) / 2.0f;
}

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  float distance = readDistance();

  if (distance != -1) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 10.0) {
      digitalWrite(LED, HIGH);   // Turn LED on
    } else {
      digitalWrite(LED, LOW);    // Turn LED off
    }
  } else {
    Serial.println("No object detected");
    digitalWrite(LED, LOW);      // Keep LED off if no reading
  }

  delay(200);
}