#include <FruitPicker.h>
FruitPicker controller;

void setup() {
    Serial.begin(9600);
    controller.begin();
}

void loop() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        data.trim();

        // 只處理 "fruit" 開頭（或你自定義的上平台stepper指令格式）
        if (data.startsWith("fruit") && data.indexOf("height=") != -1) {
            int heightIdx = data.indexOf("height=") + 7;
            int increaseIdx = data.indexOf("increase=") + 9;
            int speedIdx = data.indexOf("speed=") + 6;
            int forceIdx = data.indexOf("force=") + 6;

            int height = data.substring(heightIdx, data.indexOf(',', heightIdx)).toInt();
            bool increase = (data.substring(increaseIdx, data.indexOf(',', increaseIdx)).toInt() == 1);
            int speed = data.substring(speedIdx, data.indexOf(',', speedIdx)).toInt();

            String forceStr = data.substring(forceIdx);
            if (forceStr.indexOf(',') != -1)
                forceStr = forceStr.substring(0, forceStr.indexOf(','));
            bool force = (forceStr == "1");

            controller.setStepper(height, increase, speed, force);
        }
    }
}
