const int CW_PIN = 5;         // Stepper motor CW+ pin
const int CCW_PIN = 6;        // Stepper motor CCW+ pin
const int RIGHT_SENSOR_PIN = 11; // Right photo sensor pin
const int LEFT_SENSOR_PIN = 10;  // Left photo sensor pin

bool cwFlag = false;          // Flag for 'C' command
bool ccwFlag = false;         // Flag for 'D' command
bool rotating = false;        // Flag to indicate motor rotation
bool resetFlag = false;       // Flag for 'R' command
bool stopFlag = false;        // Flag for 'S' command

bool globalStopFlag = false;  // Global flag for left sensor interruption

const int stepDelay = 10000;   // Adjust delay for slower motor speed

void setup() {
  pinMode(CW_PIN, OUTPUT);
  pinMode(CCW_PIN, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT);
  pinMode(LEFT_SENSOR_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  char command = ' ';  // Initialize command

  if (Serial.available()) {
    command = Serial.read();  // Read the serial command
    executeCommand(command);
  }

  if (rotating) {
    if (cwFlag) {
      rotate(90, true);
      rotating = false;
      cwFlag = false;
    } else if (ccwFlag) {
      rotate(90, false);
      rotating = false;
      ccwFlag = false;
    }
  }

  if (resetFlag) {
    rotateToHomeBase();
    resetFlag = false;
  }

  if (stopFlag) {
    stopMotor();
    stopFlag = false;
  }
  
  if (globalStopFlag && (digitalRead(LEFT_SENSOR_PIN) == LOW)) {
    stopMotor();
    delay(3000);
    rotate(8, false);
    globalStopFlag = false;  // Reset the global flag
  }
}

void executeCommand(char cmd) {
  if (cmd == 'C') {
    if (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
      rotate(8, true);  // Slowly rotate 8 degrees in CW direction
      Serial.println("Rotated CW (Adjusted)");
    } else {
      cwFlag = true;
      rotating = true;
    }
  } else if (cmd == 'D') {
    if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
      rotate(8, false);  // Slowly rotate 8 degrees in CCW direction
      Serial.println("Rotated CCW (Adjusted)");
    } else {
      ccwFlag = true;
      rotating = true;
    }
  } else if (cmd == 'R') {
    resetFlag = true;
  } else if (cmd == 'S') {
    stopFlag = true;
  } else if (cmd == 'I') {
    getInfo();
  }
}

void rotate(int angleDegrees, bool isClockwise) {
  int steps = angleDegrees / 0.72;  // Calculate steps based on angle
  int motorPin = isClockwise ? CW_PIN : CCW_PIN;

  for (int i = 0; i < steps; i++) {
    digitalWrite(motorPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(motorPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void rotateToHomeBase() {
  if (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    // Rotate CCW while RIGHT_SENSOR_PIN is HIGH
    rotate(1, false);
    return;
  }

  // Stop motor for 3 seconds if sensor is interrupted
  if (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
    stopMotor();
    delay(3000);
  }

  // Rotate 188 degrees CW
  rotate(178, true);

  // Stop motor for 3 seconds if LEFT_SENSOR_PIN is interrupted
  if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    stopMotor();
    delay(3000);
    rotate(8, false);
    Serial.println("Home Base");
  } else {
    Serial.println("Home Base (Adjusted)");
  }
}


void stopMotor() {
  digitalWrite(CW_PIN, LOW);
  digitalWrite(CCW_PIN, LOW);
  rotating = false;
  Serial.println("Motor Stopped");
}

void getInfo() {
  if (rotating) {
    Serial.println("Rotating");
  } else {
    if (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
      Serial.println("Stopped: Home Base");
    } else {
      Serial.println("Stopped: Not Home Base");
    }
  }
}
