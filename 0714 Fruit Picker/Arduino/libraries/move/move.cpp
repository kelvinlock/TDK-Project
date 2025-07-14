#include "move.h"

XboxDcMotorControl::XboxDcMotorControl()
    : pwm(Adafruit_PWMServoDriver())
{
    motors[0] = {26, 28, 2};  // 左前輪 (A)
    motors[1] = {27, 29, 3};  // 右前輪 (B)
    motors[2] = {30, 32, 4};  // 左後輪 (C)
    motors[3] = {31, 33, 5};  // 右後輪 (D)
}

ServoConfig servos[] = {
    {0, 5, 85},   // 第1顆：channel 0，原點5度，終點85度
    {14, 7, 95},  // 第2顆：channel 14，原點7度，終點95度
    {15, 4, 90},  // 第3顆：channel 15，原點4度，終點90度
};

int servoCount = sizeof(servos)/sizeof(servos[0]);

void XboxDcMotorControl::begin() {
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);
    }
    pwm.begin();
    pwm.setPWMFreq(60);
}

void XboxDcMotorControl::setMotor(int index, int direction, int speed) {
    digitalWrite(motors[index].forwardPin, direction == 1 ? HIGH : LOW);
    digitalWrite(motors[index].backwardPin, direction == -1 ? HIGH : LOW);
    analogWrite(motors[index].speedPin, speed);
}

initial, final, bool final or initial
void XboxDcMotorControl::servo(const String& platform, bool up) {
    // 查找需要控制的servo编号
    if (platform == "platAbase") {
        // 控制 channel 14 & 15
        for (int i = 0; i < servoCount; i++) {
            if (servos[i].channel == 14 || servos[i].channel == 15) {
                int angle = up ? servos[i].endAngle : servos[i].startAngle;
                int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
                pwm.setPWM(servos[i].channel, 0, pulse);
                Serial.print("Channel ");
                Serial.print(servos[i].channel);
                Serial.print(" -> Angle: ");
                Serial.println(angle);
            }
        }
    } else if (platform == "platBbase") {
        // 控制 channel 0
        for (int i = 0; i < servoCount; i++) {
            if (servos[i].channel == 0) {
                int angle = up ? servos[i].endAngle : servos[i].startAngle;
                int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
                pwm.setPWM(0, 0, pulse);
                Serial.print("Channel 0 -> Angle: ");
                Serial.println(angle);
            }
        }
    } else {
        // Reset: 全部回原點
        for (int i = 0; i < servoCount; i++) {
            int pulse = map(servos[i].startAngle, 0, 180, SERVOMIN, SERVOMAX);
            pwm.setPWM(servos[i].channel, 0, pulse);
            Serial.print("Reset Channel ");
            Serial.print(servos[i].channel);
            Serial.print(" to Angle: ");
            Serial.println(servos[i].startAngle);
        }
    }
}
