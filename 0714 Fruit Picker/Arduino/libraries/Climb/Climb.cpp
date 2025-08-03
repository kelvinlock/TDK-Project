#include <Climb.h>

// 建構子：初始化伺服馬達通道及角度、直流馬達腳位
ServoManager::ServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // ========== 伺服馬達參數設定 ==========
    servos[0] = {3, 5, 85};     // channel 3，起始角5度，終點85度（預設值）
    servos[1] = {14, 7, 95};    // channel 14，起始角7度，終點95度（預設值）
    servos[2] = {15, 4, 90};    // channel 15，起始角4度，終點90度（預設值）
    // ========== 直流馬達腳位設定（可依實際需求更改）==========
    DcMotor[0] = {34, 36, 6};   // 前進腳位=34, 後退腳位=36, 速度PWM腳位=6
    DcMotor[1] = {35, 37, 7};   // 前進腳位=35, 後退腳位=37, 速度PWM腳位=7
}

// 初始化所有硬體腳位（伺服馬達PWM、直流馬達腳位）
void ServoManager::begin() {
    // 初始化直流馬達腳位
    for(int i = 0; i < 2; i++) {
        pinMode(DcMotor[i].forwardPin, OUTPUT);   // 設定前進腳位為輸出
        pinMode(DcMotor[i].backwardPin, OUTPUT);  // 設定後退腳位為輸出
        pinMode(DcMotor[i].speedPin, OUTPUT);     // 設定PWM速度腳位為輸出
    }
    // 初始化伺服馬達PWM模組
    pwm.begin();
    pwm.setPWMFreq(60); // 設定伺服馬達PWM頻率為60Hz（標準伺服適用）
}

// 控制指定channel的伺服馬達，讓其從 initial 角度「平滑」移動到 end 角度，步進大小為 increment
void ServoManager::moveServo(int channel, int initial, int end, int increment, bool ini_to_end) {
    if (!ini_to_end) {
        // 若 ini_to_end 為 false，則直接全部歸零（resetAll）
        resetAll();
        return;
    }
    // ini_to_end 為 true，搜尋對應channel並控制該伺服馬達
    for(int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel) {
            int step = (initial < end) ? increment : -increment;
            // 使用for迴圈「平滑」移動，逐步讓角度從 initial 走到 end
            for (int angle = initial; (step > 0 ? angle <= end : angle >= end); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX); // 將角度轉為PWM脈衝
                pwm.setPWM(channel, 0, pulse);                        // 輸出PWM信號到對應通道
                delay(20); // 暫停20毫秒，確保動作平順（可視需求調整）
            }
            // 找到後立即結束，不再檢查其他channel
            return;
        }
    }
}

// 將所有伺服馬達「平滑歸零」——每個馬達從終點endAngle慢慢走回原點startAngle，步進大小由increment決定
void ServoManager::resetAll(int increment) {
    for (int i = 0; i < servoCount; i++) {
        int start = servos[i].endAngle;   // 預設伺服馬達結束時都停在endAngle
        int target = servos[i].startAngle;// 歸零目標為startAngle（原點）
        int step = (start < target) ? increment : -increment;
        // 用for迴圈實現平滑回原點
        for (int angle = start; (step > 0 ? angle <= target : angle >= target); angle += step) {
            int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
            pwm.setPWM(servos[i].channel, 0, pulse);
            delay(20); // 動作平順（可視需求加大或縮小）
        }
        // 確保最後角度正好停在原點
        pwm.setPWM(servos[i].channel, 0, map(target, 0, 180, SERVO_MIN, SERVO_MAX));
    }
}
