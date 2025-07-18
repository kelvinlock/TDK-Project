#ifndef FRUIT_PICKER_H
#define FRUIT_PICKER_H

#include <Arduino.h>

extern const int EN_PIN;
extern const int stepPinX;  // 1號馬達
extern const int dirPinX;
extern const int stepPinY;  // 2號馬達
extern const int dirPinY;

// 步數與延遲
const uint8_t MICROSTEP = 1;  
const long stepsPerRevolution = 200 * MICROSTEP;
const unsigned int stepDelay = 800;       // 脈衝延遲 (微秒)

struct Motor
{
    int forwardPin;
    int backwardPin;
    int speedPin;
};

class FruitPicker {
public:
    FruitPicker();
    void begin();
    void setDC(int index, int direction, int speed);
    void setStepper(int height, bool increase, int speed, bool force);

private:
    Motor motors[2];
};

#endif