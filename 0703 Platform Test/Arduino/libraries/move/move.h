#ifndef Move_H
#define Move_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 150
#define SERVOMAX 600

struct Motor {
    int forwardPin;
    int backwardPin;
    int speedPin;
};

class XboxDcMotorControl {
public:
    XboxDcMotorControl();
    void begin();
    void processMotorData(const String& data);
    void servo(const String& platform, bool up);

private:
    Adafruit_PWMServoDriver pwm;
    Motor motors[4];
};

#endif
