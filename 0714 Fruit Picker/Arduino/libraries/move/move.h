#ifndef MOVE_H
#define MOVE_H

#include <Arduino.h>

// 結構：單顆直流馬達的腳位設定
struct DcMotor {
    int forwardPin;     // 前進方向腳位
    int backwardPin;    // 後退方向腳位
    int speedPin;       // 速度控制腳位（PWM）
};

// XboxDcMotorControl：專責控制四顆DC馬達的類別
class XboxDcMotorControl {
    public:
        XboxDcMotorControl();  // 建構子，初始化腳位參數
        void begin();          // 初始化所有馬達腳位
        void setMotor(int index, int direction, int speed); // 控制單一馬達（index:0~3）
    private:
        DcMotor motors[4];     // 四顆馬達腳位配置
};

#endif
