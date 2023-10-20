#include <Arduino.h>

const int CW_PIN = 6;
const int CCW_PIN = 5; //switch
const int RELAY_PIN = 8; //connected to the digital pins
const int RIGHT_SENSOR_PIN = 11;
const int LEFT_SENSOR_PIN = 10;

const float STEP_ANGLE = 0.72;
int STEP_DELAY = 1000; // Initial step delay value

enum class MotorState { STOPPED, MOVING_CW, MOVING_CCW };

MotorState _motorState = MotorState::STOPPED;
bool _isHoming = false;

void setup() {
  pinMode(CW_PIN, OUTPUT);
  pinMode(CCW_PIN, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LEFT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(115200);
}

void rotateMotor(bool isClockwise, int steps) {
  digitalWrite(isClockwise ? CCW_PIN : CW_PIN, HIGH);
  delayMicroseconds(STEP_DELAY);
  digitalWrite(isClockwise ? CCW_PIN : CW_PIN, LOW);
  delayMicroseconds(STEP_DELAY);
}

void stopMotor() {
  digitalWrite(CW_PIN, LOW);
  digitalWrite(CCW_PIN, LOW);
}

void moveMotor(int targetAngle, int speedPercent) {
  int speed = map(speedPercent, 0, 100, 5000, 1000);
  STEP_DELAY = speed;

  int currentAngle = 0;
  int targetSteps = abs(targetAngle) / STEP_ANGLE;
  bool isClockwise = targetAngle >= 0;

  _motorState = isClockwise ? MotorState::MOVING_CW : MotorState::MOVING_CCW;

  Serial.print("test1\r\n");

  while (currentAngle < targetSteps) {
    bool sleft = digitalRead(LEFT_SENSOR_PIN) == LOW;
    bool sright = digitalRead(RIGHT_SENSOR_PIN) == LOW;

    if (isClockwise) {
      Serial.print("test2\r\n");

      if (sleft == true) {
        Serial.print("test3\r\n");
        stopMotor();
        delay(1000); // Stop for 1 second
        rotateMotor(false, 8); // Rotate 8 degrees CCW
        delay(1000); // Stop for 1 second
        return;
      }

      if (sright == true) {
        Serial.print("test4\r\n");
        stopMotor();
        delay(1000); // Stop for 1 second
        rotateMotor(true, 8); // Rotate 8 degrees CW
        delay(1000); // Stop for 1 second
        return;
      }

      rotateMotor(isClockwise, 1); // Rotate 1 step
      currentAngle++;
    }
  }

  Serial.print("test 5\r\n");

  _motorState = MotorState::STOPPED;
}

void initializeMotor() {
  int stepOverCheck = 500;

  bool sleft = true;
  bool sright = true;

  // loop ccw
  do {
    rotateMotor(true, 1); // Rotate CCW until right sensor is interrupted
    stepOverCheck--;
    if(stepOverCheck < 0) {
      Serial.print("ST,0,RETINIT,ERR-OV-CCW,ED\r\n");
      return;
    }
    sleft = digitalRead(LEFT_SENSOR_PIN) == LOW;
    sright = digitalRead(RIGHT_SENSOR_PIN) == LOW;
    if(sleft) {
      Serial.print("ccw left sensor detect\r\n");
    }
    if(sright) {
      Serial.print("ccw right sensor detect\r\n");
    }

  } while(sleft==false && sright==false);
  
  stopMotor();
  rotateMotor(false, 8); // Rotate 8 degrees CW
  delay(1000);

  // loop cw
  stepOverCheck = 500;
  do {

    rotateMotor(false, 1); // Rotate CW until left sensor is interrupted
    stepOverCheck--;
    if(stepOverCheck < 0) {
      Serial.print("ST,0,RETINIT,ERR-OV-CW,ED\r\n");
      return;
    }    
    sleft = digitalRead(LEFT_SENSOR_PIN) == LOW;
    sright = digitalRead(RIGHT_SENSOR_PIN) == LOW;
    if(sleft) {
      Serial.print("cw left sensor detect\r\n");
    }
    if(sright) {
      Serial.print("cw right sensor detect\r\n");
    }
  } while(sleft==false && sright==false);

  stopMotor();
  rotateMotor(true, 8); // Rotate 8 degrees CCW
  delay(1000);

  Serial.print("ST,0,RETINIT,OK,ED\r\n");
  return;
}

void status() {
  String status;
  int angle = 0;

  switch (_motorState) {
    case MotorState::STOPPED:
      status = "Stopped";
      break;
    case MotorState::MOVING_CW:
      status = "Moving CW";
      break;
    case MotorState::MOVING_CCW:
      status = "Moving CCW";
      break;
  }

  Serial.print("ST,");
  Serial.print(status);
  Serial.print(",");
  Serial.print(angle);
  Serial.print(",");
  Serial.print(STEP_DELAY);
  Serial.print(",ED \r\n");
}

void ionizerLamp(bool state) {
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
}

void processCommand(String commandStr) {
  commandStr.trim();

  String cmd = getValue(commandStr,',',2);

  if (cmd.equals("INIT")) {
    initializeMotor();
    return;
  }
  
  if (cmd.equals("STATUS")) {
    status();
    return;
  }
  
  if (cmd.equals("MOVE")) {

    int angle = getValue(commandStr, ',', 4).toInt();
    int speedPercent = getValue(commandStr, ',', 5).toInt();
    moveMotor(angle, speedPercent);
    return;
  }
  
  if (cmd.equals("LAMP")) {
    int lampState = getValue(commandStr, ',', 3).toInt();
    ionizerLamp(lampState == 1);
    Serial.print("ST,0,RETLAMP,0," + String (lampState) + ",ED\r\n");
    return;
  }

  // Check for serial connection status
  if (commandStr.equals("ST,0,OK,0,ED")) {
    // Serial is connected, send OK response
    Serial.print("ST,0,OK,0,ED\r\n");
  } else {
    // Serial connection error, send error response
    Serial.print("ST,0,RETOK,ERR33,ED\r\n");
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readString();
    processCommand(data);
  }
}
