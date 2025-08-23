#ifndef BUCKET_H
#define BUCKET_H

#include <Arduino.h>                     // 引入 Arduino 核心函式庫（提供 pinMode、digitalWrite 等）
#include <Adafruit_PWMServoDriver.h>     // 引入 Adafruit 的 PWM 驅動模組（用來控制 PCA9685 控制板）

// =============================
// 伺服馬達 PWM 脈衝範圍設定
// =============================
// PCA9685 透過 PWM 輸出控制伺服馬達角度，
// 不同伺服馬達的脈衝範圍可能不同，需根據實際測試調整
#define SERVO_MIN 150    // 伺服馬達脈衝最小值（對應大約 0 度位置）
#define SERVO_MAX 600    // 伺服馬達脈衝最大值（對應大約 180 度位置）

// =============================
// 伺服馬達參數結構（用來存每顆馬達的設定）
// =============================
struct ServoConfig {
    int channel;         // 伺服馬達所使用的 PCA9685 通道編號（0~15）
    int startAngle;      // 該馬達的起始角度（原點位置，單位：度）
    int endAngle;        // 該馬達的終點角度（動作終點，單位：度）
};

// =============================
// 咖啡機伺服馬達管理類別
// =============================
// 用來集中管理多顆伺服馬達的動作邏輯（抓杯子、移動托盤等功能）
class bucketServoManager {
    public:
        bucketServoManager();   // 建構子：設定預設伺服馬達參數（起點、終點等）
        void begin();           // 初始化 PCA9685 控制器（設置 PWM 頻率等）
        void ExpandFrontArmsForGrab(int channel,int initial,int end,int increment,bool ini_to_end);
        void FrontArmsGrab(int channel, int initial, int end, int increment, bool ini_to_end);
    private:
        Adafruit_PWMServoDriver pwm;      // PCA9685 控制物件（可同時控制最多 16 顆伺服）
        static const int servoCount = 3;  // 伺服馬達數量（依實際機器數量修改）
        ServoConfig servos[servoCount];   // 儲存每顆伺服的設定資料（通道、起點、終點）
};
#endif