#include <CoffeeMove.h>

// 建構子：初始化伺服馬達通道及角度、直流馬達腳位
coffeeServoManager::coffeeServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // ========== 伺服馬達參數設定 ==========
    servos[0] = {3, 5, 85};     // channel 3，起始角5度，終點85度（預設值）
    servos[1] = {8, 7, 95};     // channel 8，起始角7度，終點95度（預設值）
}

void coffeeServoManager::begin() {
    pwm.begin();
    pwm.setPWMFreq(60); // 設定伺服馬達PWM頻率為60Hz（標準伺服適用）
}

void coffeeServoManager::removePlate(int channel, int initial, int end, int increment, bool reset) {
    // 循環檢查所有伺服馬達設定（servoCount 為總數量）
    for(int i = 0; i < servoCount; i++) {
        // 如果找到目標通道的伺服馬達
        if (servos[i].channel == channel) {
            // 即時更新該馬達的起點與終點角度
            servos[i].startAngle = initial;
            servos[i].endAngle = end;

            // 根據 initial 與 end 的大小決定遞增或遞減方向
            int step = (initial < end) ? increment : -increment;

            // 第一階段：從 initial 平滑移動到 end
            for (int angle = initial; (step > 0 ? angle <= end : angle >= end); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX); // 將角度轉換成 PWM 脈衝寬度
                pwm.setPWM(channel, 0, pulse);                        // 輸出 PWM 到指定通道
                delay(20); // 延遲 20 毫秒，讓動作平順
            }

            // 若需要回到原點（reset = true）
            if (reset) {
                int start = servos[i].endAngle;    // 從當前的結束角度開始
                int target = servos[i].startAngle; // 回到初始角度
                step = (start < target) ? increment : -increment;

                // 第二階段：從 end 平滑移動回 initial
                for (int angle = start; (step > 0 ? angle <= target : angle >= target); angle += step) {
                    int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                    pwm.setPWM(channel, 0, pulse);
                    delay(20); // 延遲以保持平順
                }

                // 確保最後精確停在原點
                pwm.setPWM(channel, 0, map(target, 0, 180, SERVO_MIN, SERVO_MAX));
            }

            // 找到對應通道後就結束函式，不再檢查其他馬達
            return;
        }
    }
}
