#include "move.h"

XboxDcMotorControl::XboxDcMotorControl()
    : pwm(Adafruit_PWMServoDriver())
{
    motors[0] = {26, 28, 2};  // 左前輪 (A)
    motors[1] = {27, 29, 3};  // 右前輪 (B)
    motors[2] = {30, 32, 4};  // 左後輪 (C)
    motors[3] = {31, 33, 5};  // 右後輪 (D)
}

void XboxDcMotorControl::begin() {
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);
    }
    pwm.begin();
    pwm.setPWMFreq(60);
}

void XboxDcMotorControl::processMotorData(const String& data) {
    String Data[5];
    int index = 0;
    String input = data;
    int speed = 70;

    while (input.length() > 0 && index < 5) {
        int colonPos = input.indexOf(':');
        if (colonPos == -1) {
            Data[index++] = input;
            input = "";
        } else {
            Data[index++] = input.substring(0, colonPos);
            input = input.substring(colonPos + 1);
        }
    }

    // 全部停止：只要第一段是"stop"
    if (Data[0] == "stop") {
        for (int i = 0; i < 4; i++) {
            digitalWrite(motors[i].forwardPin, LOW);
            digitalWrite(motors[i].backwardPin, LOW);
            analogWrite(motors[i].speedPin, 0);
        }
    } else {
        // 這裡才根據資料給每個馬達方向與速度
        for (int i = 0; i < 4; i++) {
            digitalWrite(motors[i].forwardPin, Data[i].toInt());
            digitalWrite(motors[i].backwardPin, !Data[i].toInt());
            analogWrite(motors[i].speedPin, speed);
        }
    }
    // 後面servo指令部分維持不變
    int commaPos = Data[4].indexOf(',');
    if (commaPos != -1) {
        String platform = Data[4].substring(0, commaPos);
        String valStr = Data[4].substring(commaPos + 1);
        bool val = (valStr == "1");
        servo(platform, val);
    }
}

void XboxDcMotorControl::servo(const String& platform, bool up) {
    const int minAngle = 0;
    const int maxAngle = 180;
    int targetAngle = up ? 90 : 0;
    int pulse = map(targetAngle, minAngle, maxAngle, SERVOMIN, SERVOMAX);

    if (platform == "platAbase") {
        // 只控制 pin 14 與 pin 15 / Only control servo on channel 14 and 15
        pwm.setPWM(14, 0, pulse);
        Serial.print("Channel 14 -> Angle: "); Serial.println(targetAngle);
        pwm.setPWM(15, 0, pulse);
        Serial.print("Channel 15 -> Angle: "); Serial.println(targetAngle);
    } else if (platform == "platBbase") {
        // 只控制 pin 0 / Only control servo on channel 0
        pwm.setPWM(0, 0, pulse);
        Serial.print("Channel 0 -> Angle: "); Serial.println(targetAngle);
    } else {
        // 歸零全部 / Reset all
        int resetPulse = map(0, minAngle, maxAngle, SERVOMIN, SERVOMAX);
        pwm.setPWM(0, 0, resetPulse);
        Serial.print("Reset Channel 0\n");
        pwm.setPWM(14, 0, resetPulse);
        Serial.print("Reset Channel 14\n");
        pwm.setPWM(15, 0, resetPulse);
        Serial.print("Reset Channel 15\n");
    }
}

