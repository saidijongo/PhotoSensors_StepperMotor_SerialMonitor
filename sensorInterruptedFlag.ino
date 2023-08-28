#define CW 5   
#define CCW 6  
#define RIGHT_SENSOR_PIN 11   // Right photomicrosensor pin
#define LEFT_SENSOR_PIN 10   // Left photomicrosensor pin

const int stepDelay = 10000;
const float angleToSteps = 0.72;  // Conversion factor from angle to steps

bool motorStopped = true;         // Flag to track motor status
bool resetInProgress = false;     // Flag for reset process
bool moveInProgress = false;      // Flag for move process
bool leftSensorInterrupted = false;  // Flag for left sensor interruption
bool rightSensorInterrupted = false; // Flag for right sensor interruption

void rotate(int motorPin, int steps, bool clockwise) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(motorPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(motorPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void setup() {
  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LEFT_SENSOR_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (!resetInProgress && !moveInProgress) {
      if (command == "C") {
        if (!leftSensorInterrupted) {
          motorStopped = false;
          moveInProgress = true;
          rotate(CW, int(90 / angleToSteps), true);
          moveInProgress = false;
          motorStopped = true;
        }
      } else if (command == "D") {
        if (!rightSensorInterrupted) {
          motorStopped = false;
          moveInProgress = true;
          rotate(CCW, int(90 / angleToSteps), true);
          moveInProgress = false;
          motorStopped = true;
        }
      } else if (command == "R") {
        resetInProgress = true;
        motorStopped = false;
        rotateToHomeBase();
        resetInProgress = false;
        motorStopped = true;
      } else if (command == "S") {
        stopMotor();
      } else if (command == "I") {
        printStatus();
      } else if (command == "P") {
        powerOn();
      } else if (command == "M") {
        robotMoving();
      }
    }
  }
}

void rotateToHomeBase() {
  while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    rotate(CCW, 1, false);
  }
  delay(1000);

  while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
    rotate(CW, 1, true);
  }

  if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    stopMotor();
    delay(1000);
    rotate(CCW, int(8 / angleToSteps), true);
    stopMotor();
    Serial.println("Home Base");
  }
}

void stopMotor() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, LOW);
  motorStopped = true;
  Serial.println("Motor Stopped");
}

void printStatus() {
  if (motorStopped) {
    if (resetInProgress) {
      Serial.println("Stopped: Home Base");
    } else {
      Serial.println("Stopped: Not Home Base");
    }
  } else {
    Serial.println("Rotating");
  }
}

void powerOn() {
  if (resetInProgress) {
    while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
      rotate(CCW, 1, false);
    }
    delay(1000);

    while (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
      rotate(CW, int(8 / angleToSteps), true);
      delay(1000);
      stopMotor();
    }
  }
}

void robotMoving() {
  if (!motorStopped) {
    while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
      rotate(CW, 1, true);
    }
    delay(1000);

    while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
      rotate(CCW, int(8 / angleToSteps), true);
      delay(2000);
      stopMotor();
      delay(1000);
    }
  }
}
