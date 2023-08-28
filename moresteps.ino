const int CW_PIN = 5;       // Stepper motor CW+ pin
const int CCW_PIN = 6;      // Stepper motor CCW+ pin
const int RIGHT_SENSOR_PIN = 11;  // Right photo sensor pin
const int LEFT_SENSOR_PIN = 10;   // Left photo sensor pin

int stepDelay = 1000;       // Delay between steps in microseconds
int angleDegrees = 0;       // Variable to store the angle in degrees
bool cw_ccwFlag = false;    // Flag for CW/CCW movement
bool moving = false;        // Flag to indicate if the motor is currently moving
bool homeBase = false;      // Flag to indicate if motor is at home base

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
}

void executeCommand(char cmd) {
  if (cmd == 'C') {
    rotateClockwise();
  } else if (cmd == 'D') {
    rotateCounterClockwise();
  } else if (cmd == 'R') {
    rotateToHomeBase();
  } else if (cmd == 'S') {
    stopMotor();
  } else if (cmd == 'I') {
    getInfo();
  }
}

void rotateClockwise() {
  angleDegrees = 90;
  if (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
    rotateMotor(CW_PIN);
  } else {
    rotateMotor(CW_PIN, true);
    Serial.println("CW Interrupted");
  }
}

void rotateCounterClockwise() {
  angleDegrees = 90;
  if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    rotateMotor(CCW_PIN);
  } else {
    rotateMotor(CCW_PIN, true);
    Serial.println("CCW Interrupted");
  }
}

void rotateToHomeBase() {
  while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    rotateMotor(CW_PIN);
  }
  rotateMotor(CW_PIN, true);
  while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
    rotateMotor(CCW_PIN);
  }
  homeBase = true;
  Serial.println("Home Base");
}

void rotateMotor(int pin, bool interrupted = false) {
  int steps = angleDegrees / 0.72;
  if (interrupted) {
    steps = 8 / 0.72; // 8 degrees rotation
  }
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(pin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void stopMotor() {
  digitalWrite(CW_PIN, LOW);
  digitalWrite(CCW_PIN, LOW);
  moving = false;
  Serial.println("Motor Stopped");
}

void getInfo() {
  if (moving) {
    Serial.println("Rotating");
  } else {
    if (homeBase) {
      Serial.println("Stopped: Home Base");
    } else {
      Serial.println("Stopped: Not Home Base");
    }
  }
}
