#ifndef CLIMB_H
#define CLIMB_H

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

// 直流馬達腳位設定結構
struct DcConfig {
    int forwardPin;      // 前進方向腳位
    int backwardPin;     // 後退方向腳位
    int speedPin;        // 速度控制腳位（PWM）
};

class ServoManager {
    public:
        ServoManager();                          // 建構子，初始化伺服與馬達腳位
        void begin();                            // 初始化所有硬體腳位
        void moveServo(int channel, int initial, int end, int increment, bool ini_to_end); // 控制指定平台的伺服馬達動作
        void setClimbMotor(int dir, int speed, bool on_off);
        void resetAll();                         // 將所有伺服馬達歸零（回到原點）
    private:
        Adafruit_PWMServoDriver pwm;             // 控制多路伺服馬達的驅動器
        static const int servoCount = 3;         // 伺服馬達總數
        ServoConfig servos[servoCount];          // 伺服馬達參數表
        DcConfig DcMotor[2];                     // 兩顆直流馬達腳位配置
}

#endif