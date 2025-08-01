#include "move.h"

// 建構子：設定每顆馬達的腳位
XboxDcMotorControl::XboxDcMotorControl() {
    motors[0] = {26, 28, 2};  // 左前輪 (A)
    motors[1] = {27, 29, 3};  // 右前輪 (B)
    motors[2] = {30, 32, 4};  // 左後輪 (C)
    motors[3] = {31, 33, 5};  // 右後輪 (D)
}

// 初始化馬達腳位
void XboxDcMotorControl::begin() {
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);   // 方向腳位設為輸出
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);     // 速度PWM腳位設為輸出
    }
}

// 控制單一馬達運轉
// index：馬達編號（0~3）
// direction：1=前進, -1=後退, 0=停止
// speed：速度（0~255，對應PWM值）
void XboxDcMotorControl::setMotor(int index, int direction, int speed) {
    if (direction == 1) { // 前進
        digitalWrite(motors[index].forwardPin, HIGH);
        digitalWrite(motors[index].backwardPin, LOW);
        analogWrite(motors[index].speedPin, speed);
    } else if (direction == -1) { // 後退
        digitalWrite(motors[index].forwardPin, LOW);
        digitalWrite(motors[index].backwardPin, HIGH);
        analogWrite(motors[index].speedPin, speed);
    } else { // 停止
        digitalWrite(motors[index].forwardPin, LOW);
        digitalWrite(motors[index].backwardPin, LOW);
        analogWrite(motors[index].speedPin, 0);
    }
}
