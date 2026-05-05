// ================== MOTOR PINS ==================
#include <IRremote.hpp>

const int ain1Pin = 3;
const int ain2Pin = 4;
const int pwmAPin = 5;

const int bin1Pin = 8;
const int bin2Pin = 7;
const int pwmBPin = 6;

// ================== ULTRASONIC ==================
const int trigPin = 9;
const int echoPin = 10;

// ================== LED ==================
const int ledPin = 13;

// ================== IR ==================
const int irPin = 2;

// ================== STATE ==================
int currentMode = 0;
bool paused = false;

bool handPresent = false;
unsigned long handStartTime = 0;

int waveCount = 0;
unsigned long lastWaveTime = 0;
int lastSentMode = -1;
// ================== SMOOTHING ==================
const int SMOOTH_SAMPLES = 5;
int distBuffer[SMOOTH_SAMPLES] = {0};
int distIndex = 0;

// ================== SETUP ==================
void setup() {
  pinMode(ain1Pin, OUTPUT);
  pinMode(ain2Pin, OUTPUT);
  pinMode(pwmAPin, OUTPUT);

  pinMode(bin1Pin, OUTPUT);
  pinMode(bin2Pin, OUTPUT);
  pinMode(pwmBPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(irPin, OUTPUT);

  Serial.begin(9600);
  
}

// ================== DISTANCE ==================
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

// --- SMOOTHED DISTANCE ---
int getSmoothedDistance() {
  distBuffer[distIndex] = getDistance();
  distIndex = (distIndex + 1) % SMOOTH_SAMPLES;

  long sum = 0;
  for (int i = 0; i < SMOOTH_SAMPLES; i++) {
    sum += distBuffer[i];
  }

  return sum / SMOOTH_SAMPLES;
}

// ================== MOTOR ==================
void stopMotors() {
  analogWrite(pwmAPin, 0);
  analogWrite(pwmBPin, 0);
  
}

void moveForward(int speedVal) {
  digitalWrite(ain1Pin, HIGH);
  digitalWrite(ain2Pin, LOW);
  digitalWrite(bin1Pin, HIGH);
  digitalWrite(bin2Pin, LOW);

  analogWrite(pwmAPin, speedVal);
  analogWrite(pwmBPin, speedVal);

  Serial.print("Forward speed: ");
  
}

void moveBackward(int speedVal) {
  digitalWrite(ain1Pin, LOW);
  digitalWrite(ain2Pin, HIGH);
  digitalWrite(bin1Pin, LOW);
  digitalWrite(bin2Pin, HIGH);

  analogWrite(pwmAPin, speedVal);
  analogWrite(pwmBPin, speedVal);

  Serial.print("Backward speed: ");
  
}

// ================== IR ==================
void sendIRSignal(uint32_t hexCode) {
  IrSender.sendNEC(0x00, hexCode, 0);

  Serial.print("IR:");
  
}

// ================== MODES ==================
void chill() {
  
  sendIRSignal(0xFC03EF00);

  if (paused) return;
  moveForward(70);
}

void focus() {
  
  sendIRSignal(0xFC0367A1);

  stopMotors();
}

void party() {
  
  sendIRSignal(0xFC03FF0A);

  if (paused) return;
  moveForward(200);
  delay(200);

  if (paused) return;
  stopMotors();
  delay(100);

  if (paused) return;
  moveBackward(200);
}

void rain() {
  
  sendIRSignal(0xFC03C3F0);

  if (paused) return;
  moveForward(150);
}

void wind() {
  
  sendIRSignal(0xFC03C3F0);

  if (paused) return;
  moveForward(120);
}

void eerie() {
  
  sendIRSignal(0xFC03FF0A);

  if (paused) return;
  moveForward(200);
  delay(200);

  if (paused) return;
  stopMotors();

  if (paused) return;
  moveBackward(200);
  delay(200);

  if (paused) return;
  moveForward(200);
  delay(200);

  stopMotors();
}

void sonder() {
  
  sendIRSignal(0xFC13DFC0);

  if (paused) return;
  moveBackward(250);
}

void hireath() {
  
  sendIRSignal(0xFC13DFC0);

  if (paused) return;
  moveForward(20);
}

void glitch() {
  
  sendIRSignal(0xFC03FF0A);

  if (paused) return;
  moveForward(100);
  delay(200);

  if (paused) return;
  moveForward(200);
  delay(50);

  if (paused) return;
  moveBackward(200);
  delay(200);
}

// ================== RUN MODE ==================
void runMode() {
  if (paused) {
    stopMotors();
    return;
  }

  // ✅ SEND MODE ONLY WHEN IT CHANGES
  if (currentMode != lastSentMode) {
    Serial.print("MODE:");
    
    lastSentMode = currentMode;
  }

  switch (currentMode) {
    case 0: chill(); break;
    case 1: focus(); break;
    case 2: party(); break;
    case 3: rain(); break;
    case 4: wind(); break;
    case 5: eerie(); break;
    case 6: sonder(); break;
    case 7: hireath(); break;
    case 8: glitch(); break;
  }
}

// ================== GESTURES ==================
void handleGestures(int distance) {
  unsigned long now = millis();

  // ✅ Always print distance
  Serial.print("Distance: ");
  Serial.print(distance);
  

  // ✅ HYSTERESIS FIX (prevents LED blinking)
  static bool ledState = false;

  if (distance > 0) {
    if (distance < 10) {
      ledState = true;
    } 
    else if (distance > 12) {
      ledState = false;
    }
  }

  digitalWrite(ledPin, ledState ? HIGH : LOW);

  bool inRange = ledState;

  // ================= HAND ENTER =================
  if (inRange && !handPresent) {
    handPresent = true;
    handStartTime = now;

    if (now - lastWaveTime > 150 && now - lastWaveTime < 800) {
      waveCount++;
    } else {
      waveCount = 1;
    }

    lastWaveTime = now;

    Serial.print("Wave count: ");
    
  }

  // ================= HAND HOLD =================
  if (inRange && handPresent) {
    if ((now - handStartTime > 2000) && (now - lastWaveTime > 1500)) {

      if (paused) {
        paused = false;
        
      } else {
        paused = true;
        
      }

      waveCount = 0;
      delay(800);
    }
  }

  // ================= HAND EXIT =================
  if (!inRange && handPresent) {
    handPresent = false;
  }

  // ================= WAVE ACTION =================
  if (waveCount >= 3) {
    waveCount = 0;

    currentMode = (currentMode + 1) % 9;

    if (paused) {
      paused = false;
      
    } else {
      
    }

    Serial.print("Mode -> ");
    

    delay(800);
  }
}

// ================== SERIAL ==================
void handleSerial() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');

    Serial.print("Received: ");
    

    if (cmd.startsWith("MODE")) {
      int m = cmd.substring(4).toInt();

      if (m >= 0 && m <= 8) {
        currentMode = m;
        paused = false;

        Serial.print("Switched to mode: ");
        
      }
    }
  }
}

// ================== LOOP ==================
void loop() {
  int distance = getSmoothedDistance();

  handleGestures(distance);
  handleSerial();
  runMode();

  delay(100);
}