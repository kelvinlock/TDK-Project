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
    int speed = 200;

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
    int commaPos = Data[4].indexOf(',');
    if (commaPos != -1) {
        String platform = Data[4].substring(0, commaPos);
        String valStr = Data[4].substring(commaPos + 1);
        bool val = (valStr == "1");
        servo(platform, val);
    }
}

void XboxDcMotorControl::servo(const String& platform, bool up) {
    if (platform == "platAbase" || platform == "platBbase") {
        for (uint8_t ch = 0; ch < 16; ch++) {
            int angle = random(0, 271);
            int pulse = map(angle, 0, 270, SERVOMIN, SERVOMAX);
            pwm.setPWM(ch, 0, pulse);
            Serial.print("Channel "); Serial.print(ch);
            Serial.print(" -> Angle: "); Serial.println(angle);
            delay(500);
        }
    } else {
        for (uint8_t ch = 0; ch < 16; ch++) {
            int pulse = map(0, 0, 270, SERVOMIN, SERVOMAX);
            pwm.setPWM(ch, 0, pulse);
            Serial.print("Reset Channel "); Serial.println(ch);
            delay(500);
        }
    }
}
