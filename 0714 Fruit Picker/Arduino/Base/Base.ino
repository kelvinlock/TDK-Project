#include <move.h>

XboxDcMotorControl controller;

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    controller.begin(); // 初始化馬達控制器
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // 讀取一行串口數據
        data.trim();

        // 先將資料用 ':' 拆分成 segments
        int last = 0;
        int idx = 0;
        String segments[10]; // 最多10個指令段
        while (last < data.length()) {
            int colon = data.indexOf(':', last);
            if (colon == -1) {
                segments[idx++] = data.substring(last);
                break;
            } else {
                segments[idx++] = data.substring(last, colon);
                last = colon + 1;
            }
        }

        int motorIndex = 0; // 用來對應第幾顆馬達（左前、右前、左後、右後）

        // 逐一解析每個指令段(segment)
        for (int i = 0; i < idx; i++) {
            String segment = segments[i];
            segment.trim();

            // 僅支援處理移動"move"或"turn"開頭的指令
            if (segment.startsWith("move") || segment.startsWith("turn")) {
                int dir = 0, speed = 0;
                bool on_off = false;

                // 解析dir、speed、on_off的數值
                int dirIdx = segment.indexOf("dir=") + 4;
                int speedIdx = segment.indexOf("speed=") + 6;
                int onOffIdx = segment.indexOf("on_off=") + 7;

                // 馬達方向（1前進、-1後退、0停止
                dir = segment.substring(dirIdx, segment.indexOf(',', dirIdx)).toInt();
                speed = segment.substring(speedIdx, segment.indexOf(',', speedIdx)).toInt(); // 馬達速度（0~255）

                // on_off 取到字串尾（是否啟動馬達）
                String onOffStr = segment.substring(onOffIdx);
                if (onOffStr.indexOf(',') != -1)
                    onOffStr = onOffStr.substring(0, onOffStr.indexOf(','));
                on_off = (onOffStr == "1"); // 是否執行（True=1, False=0）

                // 執行馬達控制
                if (on_off) {
                    controller.setMotor(motorIndex, dir, speed); // 控制指定馬達（motorIndex）方向與速度
                } else {
                    controller.setMotor(motorIndex, 0, 0);       // 若 off 則該馬達停止
                }
                motorIndex++; // 處理下一顆馬達
            }
            // "fruit" 模式，格式："fruit,height=50,increase=1,speed=520,force=1:\n"
            else if (segment.startsWith("fruit")) {
                if (segment.indexOf("dir=") != -1) {
                    // DC motor 指令處理
                    int dirIdx = segment.indexOf("dir=") + 4;
                    int speedIdx = segment.indexOf("speed=") + 6;
                    int onOffIdx = segment.indexOf("on_off=") + 7;

                    int dir = segment.substring(dirIdx, segment.indexOf(',', dirIdx)).toInt();
                    int speed = segment.substring(speedIdx, segment.indexOf(',', speedIdx)).toInt();

                    String onOffStr = segment.substring(onOffIdx);
                    if (onOffStr.indexOf(',') != -1)
                        onOffStr = onOffStr.substring(0, onOffStr.indexOf(','));
                    bool on_off = (onOffStr == "True" || onOffStr == "1");

                    controller.setMotor(dir, speed, on_off);  // 你需要在 library 實作這個
                } else {
                    // 轉發給 Board B
                    Serial1.println(segment); // 請確認 B 端是 Serial.begin(9600)
                }
            }
            // 處理平台伺服馬達指令
            else if (segment.startsWith("platAbase") || segment.startsWith("platBbase") || segment.startsWith("reset")) {
                int commaPos = segment.indexOf(',');
                String valStr = segment.substring(commaPos + 1);
                bool up = (valStr == "1");
                controller.servo(segment.substring(0, commaPos), up);
            }
        }
    }
}
