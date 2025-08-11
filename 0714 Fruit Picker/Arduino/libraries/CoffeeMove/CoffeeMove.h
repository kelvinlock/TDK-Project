#ifndef COFFEE_MOVE_H
#define COFFEE_MOVE_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// 伺服馬達脈衝範圍（根據實際馬達需求設定）
#define SERVO_MIN 150    // 伺服馬達脈衝最小值
#define SERVO_MAX 600    // 伺服馬達脈衝最大值

// 伺服馬達設定結構
struct ServoConfig {
    int channel;         // 伺服馬達的通道編號（PCA9685 的 Channel）
    int startAngle;      // 馬達原點角度（起始角，單位：度）
    int endAngle;        // 馬達終點角度（終止角，單位：度）
};

// 專用伺服馬達控制管理類別
class coffeeServoManager {
    public:
        coffeeServoManager();                   // 建構子，初始化伺服馬達參數
        void begin();                           // 初始化所有硬體腳位（PWM模組）
        void removePlate(int channel, int initial, int end, int increment, bool reset);
    private:
        Adafruit_PWMServoDriver pwm;            // 控制多路伺服馬達的驅動器
        static const int servoCount = 2;        // 伺服馬達總數（依機器結構調整）
        ServoConfig servos[servoCount];         // 伺服馬達參數列表
};
#endif