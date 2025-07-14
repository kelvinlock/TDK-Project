#include <move.h>
#include <FruitPicker.h>

XboxDcMotorControl controller;

void setup() {
    Serial.begin(9600);
    controller.begin();
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        data.trim();

        // 判断是伺服指令还是移动指令
        if (data.startsWith("platAbase")) {
            // 格式: platAbase,1 或 platAbase,0
            int commaPos = data.indexOf(',');
            String valStr = data.substring(commaPos+1);
            bool up = (valStr == "1");
            controller.servo("platAbase", up);
        } else if (data.startsWith("platBbase")) {
            int commaPos = data.indexOf(',');
            String valStr = data.substring(commaPos+1);
            bool up = (valStr == "1");
            controller.servo("platBbase", up);
        } else if (data.startsWith("reset")) {
            controller.servo("reset", false);
        } else {
            // 默认为底盘移动
            controller.processMotorData(data);
        }
    }
}