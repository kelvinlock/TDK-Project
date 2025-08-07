#ifndef COFFEE_MOVE_H
#define COFFEE_MOVE_H

#include <Arduino.h>

#include <Adafruit_PWMServoDriver.h>

// 伺服馬達脈衝範圍
#define SERVO_MIN 150    // 伺服馬達脈衝最小值
#define SERVO_MAX 600    // 伺服馬達脈衝最大值

// 伺服馬達設定結構
struct ServoConfig {
    int channel;         // 伺服馬達的通道（channel）
    int startAngle;      // 原點（起始角度）
    int endAngle;        // 終點（終點角度）
};

class coffeeServoManager {
    public:
        coffeeServoManager();                     // 建構子，初始化伺服與馬達腳位
        void begin();
    private:
        Adafruit_PWMServoDriver pwm;             // 控制多路伺服馬達的驅動器
        static const int servoCount =;
        ServoConfig servos[servoCount]
}
##endif