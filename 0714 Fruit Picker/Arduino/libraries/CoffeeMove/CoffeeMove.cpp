#include <CoffeeMove.h>

// 建構子：初始化伺服馬達通道及角度
coffeeServoManager::coffeeServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // ========== 預設伺服馬達參數設定 ==========
    // servos[0] → 通道 7，初始角度 5 度，終點角度 85 度
    servos[0] = {7, 5, 85};
    // servos[1] → 通道 8，初始角度 7 度，終點角度 95 度
    servos[1] = {8, 7, 95};
}

void coffeeServoManager::begin() {
    pwm.begin();         // 啟動 PCA9685 PWM 控制器
    pwm.setPWMFreq(60);  // 設定伺服馬達 PWM 頻率為 60Hz（標準伺服常用頻率）
}

/**
 * grabCup
 * 單向動作：
 * ini_to_end = true  → 從 initial → end
 * ini_to_end = false → 從 end → initial
 */
void coffeeServoManager::grabCup(int channel, int initial, int end, int increment, bool ini_to_end) {
    // ====== 防呆處理 ======
    increment = (increment <= 0) ? 1 : increment; // 遞增步長至少為 1
    initial   = constrain(initial, 0, 180);       // 限制角度範圍在 0~180
    end       = constrain(end,     0, 180);

    // 遍歷所有已定義的伺服馬達
    for (int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel) { // 找到對應通道
            // 儲存最新的起點與終點
            servos[i].startAngle = initial;
            servos[i].endAngle   = end;

            // 根據 ini_to_end 決定起點 (from) 與終點 (to)
            int from = ini_to_end ? servos[i].startAngle : servos[i].endAngle;
            int to   = ini_to_end ? servos[i].endAngle   : servos[i].startAngle;

            // 如果起點與終點相同，直接落點後結束
            if (from == to) {
                pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
                return;
            }

            // 根據方向決定正遞增或負遞減
            int step = (from < to) ? increment : -increment;

            // 從 from 平滑移動到 to
            for (int angle = from; (step > 0 ? angle <= to : angle >= to); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX); // 角度轉 PWM 脈衝
                pwm.setPWM(channel, 0, pulse); // 發送 PWM 到指定伺服通道
                delay(20); // 延遲 20 毫秒，讓動作平順
            }

            // 確保最後精準落在目標角度
            pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
            return; // 完成後結束
        }
    }
}

/**
 * removePlate
 * 移動托盤：
 * reset = false → 從 initial → end
 * reset = true  → 從 initial → end → initial（回彈）
 */
void coffeeServoManager::removePlate(int channel, int initial, int end, int increment, bool reset) {
    // ====== 防呆處理 ======
    increment = (increment <= 0) ? 1 : increment; // 遞增步長至少為 1
    initial   = constrain(initial, 0, 180);       // 限制角度範圍在 0~180
    end       = constrain(end,     0, 180);

    // 遍歷所有已定義的伺服馬達
    for(int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel) { // 找到對應通道
            // 儲存最新的起點與終點
            servos[i].startAngle = initial;
            servos[i].endAngle   = end;

            // ====== 第一段：從 initial 移動到 end ======
            if (initial != end) { // 如果角度不同才需要移動
                int step = (initial < end) ? increment : -increment;
                for (int angle = initial; (step > 0 ? angle <= end : angle >= end); angle += step) {
                    int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                    pwm.setPWM(channel, 0, pulse);
                    delay(20); // 平滑動作
                }
            }
            // 確保精準落在 end
            pwm.setPWM(channel, 0, map(end, 0, 180, SERVO_MIN, SERVO_MAX));

            // ====== 第二段（可選）：回到 initial ======
            if (reset) {
                if (end != initial) { // 如果需要回彈，且角度不同
                    int step = (end < initial) ? increment : -increment;
                    for (int angle = end; (step > 0 ? angle <= initial : angle >= initial); angle += step) {
                        int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                        pwm.setPWM(channel, 0, pulse);
                        delay(20);
                    }
                }
                // 精準落在 initial
                pwm.setPWM(channel, 0, map(initial, 0, 180, SERVO_MIN, SERVO_MAX));
            }
            return; // 完成後結束
        }
    }
}
