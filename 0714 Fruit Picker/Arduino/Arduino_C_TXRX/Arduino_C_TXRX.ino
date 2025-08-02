/*
    平台控制 Arduino Board C
    連接方式：透過 TX/RX 連接（第二塊 Arduino 板子）
    主要負責：叉杆結構的升降
    位置：位於平臺A上方
*/
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
    if (holding) {
        // 如果已經激磁一段時間（如2秒）
        if (millis() - magnetOnTime >= 2000) {
            digitalWrite(EN_PIN, HIGH); // 釋放激磁（解鎖）
            holding = false;            // 結束等待，下次動作前才會再激磁
        }
    }
}
