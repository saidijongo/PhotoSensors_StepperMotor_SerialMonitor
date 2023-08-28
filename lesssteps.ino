const int CW_PIN = 5;       // Stepper motor CW+ pin
const int CCW_PIN = 6;      // Stepper motor CCW+ pin
const int RIGHT_SENSOR_PIN = 11;  // Right photo sensor pin
const int LEFT_SENSOR_PIN = 10;   // Left photo sensor pin

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
  if (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
    rotateMotor(CW_PIN, 90);
  } else {
    rotateMotor(CW_PIN, 8);
    Serial.println("CW Interrupted");
  }
}

void rotateCounterClockwise() {
  if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    rotateMotor(CCW_PIN, 90);
  } else {
    rotateMotor(CCW_PIN, 8);
    Serial.println("CCW Interrupted");
  }
}

void rotateToHomeBase() {
  if (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
      rotateMotor(CW_PIN, 1);
    }
    rotateMotor(CW_PIN, 188);  // Rotate back 188 degrees
    homeBase = true;
    Serial.println("Home Base");
  } else if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
      rotateMotor(CCW_PIN, 1);
    }
    rotateMotor(CCW_PIN, 8);   // Rotate away from sensor
    homeBase = false;
    Serial.println("Home Base (Adjusted)");
  }
}

void rotateMotor(int pin, int degrees) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(degrees * 10);  // Adjust delay as needed
  digitalWrite(pin, LOW);
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
