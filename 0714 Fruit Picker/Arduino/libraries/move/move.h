#ifndef MOVE_H
#define MOVE_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 150
#define SERVOMAX 600

struct Motor {
    int forwardPin;
    int backwardPin;
    int speedPin;
};

struct ServoConfig {
    int channel;
    int startAngle; // 原點（起始角度）
    int endAngle;   // 重點（終點角度）
};

class XboxDcMotorControl {
public:
    XboxDcMotorControl();
    void begin();
    void setMotor(int index, int direction, int speed);
    void servo(const String& platform, bool up);

private:
    Adafruit_PWMServoDriver pwm;
    Motor motors[4];
};

#endif
