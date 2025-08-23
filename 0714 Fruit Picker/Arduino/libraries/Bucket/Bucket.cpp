#include <Bucket.h>

// 建構子：初始化伺服馬達通道及角度
bucketServoManager::bucketServoManager() : pwm(Adafruit_PWMServoDriver()) {
    // ========== 預設伺服馬達參數設定 ==========
    // servos[0] → 通道 11，初始角度 5 度，終點角度 85 度
    servos[0] = {11, 5, 85};
    // servos[1] → 通道 12，初始角度 7 度，終點角度 95 度
    servos[1] = {12, 7, 95};
    // servos[2] → 通道 13，初始角度 5 度，終點角度 63 度
    servos[2] = {13, 5, 63};
}

void bucketServoManager::begin() {
    pwm.begin();         // 啟動 PCA9685 PWM 控制器
    pwm.setPWMFreq(60);  // 設定伺服馬達 PWM 頻率為 60Hz（標準伺服常用頻率）
}

void bucketServoManager::ExpandFrontArmsForGrab(int channel,int initial,int end,int increment,bool ini_to_end) {
    // 防呆：確保參數合理，避免異常
    increment = (increment <= 0) ? 1 : increment; // 步進值至少為 1
    initial   = constrain(initial, 0, 180);       // 初始角度限制在 0~180 度
    end       = constrain(end,     0, 180);       // 終點角度限制在 0~180 度

    // 遍歷所有伺服馬達設定，找到目標 channel
    for (int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel ) {
            // ========== 更新 ServoConfig ==========
            // 先記錄該馬達的起點與終點角度，確保狀態一致
            servos[i].startAngle = initial;
            servos[i].endAngle   = end;

            // ========== 根據 ini_to_end 決定動作方向 ==========
            // ini_to_end = true  → 從 startAngle → endAngle
            // ini_to_end = false → 從 endAngle → startAngle
            int from = ini_to_end ? servos[i].startAngle : servos[i].endAngle;
            int to   = ini_to_end ? servos[i].endAngle   : servos[i].startAngle;

            // 如果 from == to，直接落點即可，無需移動
            if (from == to) {
                pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
                return; // 完成後直接退出函式
            }

            // 計算步進方向（由小到大正向；由大到小反向）
            int step = (from < to) ? increment : -increment;

            // ========== 平滑移動過程 ==========
            for (int angle = from; (step > 0 ? angle <= to : angle >= to); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX); // 將角度轉換成 PWM 脈衝
                pwm.setPWM(channel, 0, pulse);                        // 寫入 PWM 模組
                delay(20); // 延遲，避免移動過快導致不穩
            }

            // ========== 精準落點 ==========
            // 最後強制設定到目標角度，避免累積誤差
            pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
            return; // 找到後完成，結束函式
        }
    }
}

void bucketServoManager::FrontArmsGrab(int channel, int initial, int end, int increment, bool ini_to_end) {
    // 防呆：確保參數合理，避免異常
    increment = (increment <= 0) ? 1 : increment; // 步進值至少為 1
    initial   = constrain(initial, 0, 180);       // 初始角度限制在 0~180 度
    end       = constrain(end,     0, 180);       // 終點角度限制在 0~180 度

    // 遍歷所有伺服馬達設定，找到目標 channel
    for (int i = 0; i < servoCount; i++) {
        if (servos[i].channel == channel ) {
            // 1) 先更新這顆伺服的狀態（之後都以這兩個欄位為準）
            servos[i].startAngle = initial;
            servos[i].endAngle   = end;

            // 2) 依 ini_to_end 決定移動方向
            //    true  = start -> end（展開/夾合 由你定義）
            //    false = end   -> start（反向）
            int from = ini_to_end ? servos[i].startAngle : servos[i].endAngle;
            int to   = ini_to_end ? servos[i].endAngle   : servos[i].startAngle;

            // 3) 若 from == to，直接落點即可
            if (from == to) {
                pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
                return;
            }

            // 4) 計算步進方向（正向/反向）
            int step = (from < to) ? increment : -increment;

            // 5) 平滑移動：每次前進 step 度
            for (int angle = from; (step > 0 ? angle <= to : angle >= to); angle += step) {
                int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
                pwm.setPWM(channel, 0, pulse);
                delay(20);  // 視機構負載可調
            }

            // 6) 精準落點，避免累積誤差
            pwm.setPWM(channel, 0, map(to, 0, 180, SERVO_MIN, SERVO_MAX));
            return; // 完成該通道後結束
        }
    }
}