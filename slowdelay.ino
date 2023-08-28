const int CW_PIN = 5;         // Stepper motor CW+ pin
const int CCW_PIN = 6;        // Stepper motor CCW+ pin
const int RIGHT_SENSOR_PIN = 11; // Right photo sensor pin
const int LEFT_SENSOR_PIN = 10;  // Left photo sensor pin

bool cwFlag = false;          // Flag for 'C' command
bool ccwFlag = false;         // Flag for 'D' command
bool rotating = false;        // Flag to indicate motor rotation
bool resetFlag = false;       // Flag for 'R' command
bool stopFlag = false;        // Flag for 'S' command

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
}

void executeCommand(char cmd) {
  if (cmd == 'C') {
    cwFlag = true;
    rotating = true;
  } else if (cmd == 'D') {
    ccwFlag = true;
    rotating = true;
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
  bool homeBaseReached = false;
  while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    rotate(1, false);
    homeBaseReached = true;
  }
  if (homeBaseReached) {
    delay(1000);  // Pause for 1 second
    rotate(188, true);
    Serial.println("Home Base");
  } else {
    while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
      rotate(1, true);
    }
    delay(1000);  // Pause for 1 second
    rotate(8, false);
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
