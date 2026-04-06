int ledFade = 9;     // fading LED
int ledOutput = 8;   // digital LED
int buttonPin = 2;
int ldrPin = A0;

int brightness = 0;
int fadeAmount = 5;

void setup() {
  pinMode(ledFade, OUTPUT);
  pinMode(ledOutput, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {

  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    analogWrite(ledFade, brightness);
    brightness += fadeAmount;

    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
    int ldrValue = analogRead(ldrPin);
    Serial.println(ldrValue);

    if (ldrValue < 600) {
      digitalWrite(ledOutput, HIGH);
    } else {
      digitalWrite(ledOutput, LOW);
    }

  } else {

    analogWrite(ledFade, 0);
    digitalWrite(ledOutput, LOW);
  }

  delay(30);
}