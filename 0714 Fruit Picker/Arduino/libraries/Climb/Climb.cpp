#include <Climb.h>

// 建構子：初始化伺服馬達通道及角度、直流馬達腳位
ServoManager::ServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // 伺服馬達參數設定
    servos[0] = {3, 5, 85};     // channel 3，起始角5度，終點85度 default
    servos[1] = {14, 7, 95};    // channel 14，起始角7度，終點95度 default
    servos[2] = {15, 4, 90};    // channel 15，起始角4度，終點90度 default
    // 直流馬達腳位（範例，實際請依實體配線設定）
    DcMotor[0] = {34, 36, 6};     // forwardPin=34, backwardPin=36, speedPin=6
    DcMotor[1] = {35, 37, 7};     // forwardPin=35, backwardPin=37, speedPin=7
}

// 初始化所有硬體腳位（伺服馬達PWM、直流馬達腳位）
void ServoManager::begin() {
    // 初始化直流馬達腳位
    for(int i = 0; i < 2; i++) {
        pinMode(DcMotor[i].forwardPin, OUTPUT);   // 前進腳位
        pinMode(DcMotor[i].backwardPin, OUTPUT);  // 後退腳位
        pinMode(DcMotor[i].speedPin, OUTPUT);     // PWM 速度腳位
    }
    // 初始化伺服馬達PWM
    pwm.begin();
    pwm.setPWMFreq(60); // 設定伺服馬達PWM頻率為60Hz
}

// 控制指定channel伺服馬達從 initial 移動到 end，每次遞增 increment
void ServoManager::moveServo(int channel, int initial, int end, int increment, bool ini_to_end) {
    if (!ini_to_end) {
        // 若 ini_to_end 為 false，則直接全部歸零
        resetAll();
        return;
    }
    // ini_to_end 為 true，依照 channel 操作對應的伺服馬達
    for(int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel) {
            int step = (initial < end) ? increment : -increment;
            for (int angle = initial; (step > 0 ? angle <= end : angle >= end); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                pwm.setPWM(channel, 0, pulse);
                delay(20); // 讓動作看起來比較平滑，可以依需求調整
            }
            return; // 找到對應channel就完成了
        }
    }
}

// 將所有伺服馬達歸零（回到原點）
void ServoManager::resetAll() {
    for (int i = 0; i < servoCount; i++) {
        int pulse = map(servos[i].startAngle, 0, 180, SERVO_MIN, SERVO_MAX);
        pwm.setPWM(servos[i].channel, 0, pulse);
        Serial.print("Reset Channel ");
        Serial.print(servos[i].channel);
        Serial.print(" to Angle: ");
        Serial.println(servos[i].startAngle);
    }
}