#include <Climb.h>

// 建構子：初始化伺服馬達通道及角度、直流馬達腳位
climbServoManager::climbServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // ========== 伺服馬達參數設定 ==========
    servos[0] = {3, 5, 85};     // channel 3，起始角5度，終點85度（預設值）
    servos[1] = {14, 7, 95};    // channel 14，起始角7度，終點95度（預設值）
    servos[2] = {15, 4, 90};    // channel 15，起始角4度，終點90度（預設值）
    // ========== 直流馬達腳位設定（可依實際需求更改）==========
    DcMotor[0] = {34, 36, 6};   // 前進腳位=34, 後退腳位=36, 速度PWM腳位=6
    DcMotor[1] = {35, 37, 7};   // 前進腳位=35, 後退腳位=37, 速度PWM腳位=7
}

// 初始化所有硬體腳位（伺服馬達PWM、直流馬達腳位）
void climbServoManager::begin() {
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
void climbServoManager::moveServo(int channel, int initial, int end, int increment, bool ini_to_end) {
    // 防呆
    increment = (increment == 0) ? 1 : abs(increment);
    if (!ini_to_end) {
        // 若 ini_to_end 為 false，則直接全部歸零（resetAll）
        resetAll(increment);
        return;
    }
    // ini_to_end 為 true，搜尋對應channel並控制該伺服馬達
    for(int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel) {
            int start  = constrain(initial, 0, 180);
            int target = constrain(end,     0, 180);
            servos[i].startAngle = start;
            servos[i].endAngle   = target;

            // 依「儲存後」的起訖角度計算方向
            int step = (start < target) ? increment : -increment;

            // 平滑移動：從 start 走到 target
            for (int angle = start; (step > 0 ? angle <= target : angle >= target); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                pwm.setPWM(channel, 0, pulse);
                delay(20); // 依硬體調整
            }
            // 收尾：確保精確停在 target（避免步進無法整除時差一點點）
            pwm.setPWM(channel, 0, map(target, 0, 180, SERVO_MIN, SERVO_MAX));
            return;
        }
    }
}

void climbServoManager::setClimbMotor(int dir, int speed, bool on_off) {
    for (int i = 0; i < 2; i++) {
        if (!on_off) {
            // 若 on_off 為 false，直接關閉馬達
            digitalWrite(DcMotor[i].forwardPin, LOW);
            digitalWrite(DcMotor[i].backwardPin, LOW);
            analogWrite(DcMotor[i].speedPin, 0);
        } else if (dir == 1) {
            digitalWrite(DcMotor[i].forwardPin, HIGH);
            digitalWrite(DcMotor[i].backwardPin, LOW);
            analogWrite(DcMotor[i].speedPin, speed);
        } else if (dir == -1) {
            digitalWrite(DcMotor[i].forwardPin, LOW);
            digitalWrite(DcMotor[i].backwardPin, HIGH);
            analogWrite(DcMotor[i].speedPin, speed);
        } else { // 停止
            digitalWrite(DcMotor[i].forwardPin, LOW);
            digitalWrite(DcMotor[i].backwardPin, LOW);
            analogWrite(DcMotor[i].speedPin, 0);
        }
    }
}

// 將所有伺服馬達「平滑歸零」——每個馬達從終點endAngle慢慢走回原點startAngle，步進大小由increment決定
void climbServoManager::resetAll(int increment) {
    // 防呆：確保步進至少為 1
    increment = abs(increment);
    if (increment == 0) increment = 1;

    const uint16_t kStepDelayMs = 20; // 動作平順延遲（可視硬體調整）

    for (int i = 0; i < servoCount; i++) {
        const int channel = servos[i].channel;
        // 將角度夾在 0~180 度
        int start  = constrain(servos[i].endAngle,   0, 180);   // 預設伺服馬達結束時都停在endAngle
        int target = constrain(servos[i].startAngle, 0, 180);   // 歸零目標為startAngle（原點）

        if (start == target) {
            pwm.setPWM(channel, 0, map(target, 0, 180, SERVO_MIN, SERVO_MAX));
            continue;
        }

        int step = (start < target) ? increment : -increment;

        // 平滑回原點
        for (int angle = start; (step > 0 ? angle <= target : angle >= target); angle += step) {
            int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
            pwm.setPWM(channel, 0, pulse);
            delay(kStepDelayMs);
        }
        // 確保最後角度正好停在原點
        pwm.setPWM(channel, 0, map(target, 0, 180, SERVO_MIN, SERVO_MAX));
    }
}
