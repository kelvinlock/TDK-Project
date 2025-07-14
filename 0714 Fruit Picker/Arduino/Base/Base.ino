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

        if (data.startsWith("move:")) {
            int dir = 0, speed = 0;
            bool on_off = false;

            int dirIdx = data.indexOf("dir,") + 4;
            int speedIdx = data.indexOf("speed,") + 6;
            int onOffIdx = data.indexOf("on_off,") + 7;

            dir = data.substring(dirIdx, data.indexOf(':', dirIdx)).toInt();
            speed = data.substring(speedIdx, data.indexOf(':', speedIdx)).toInt();
            String onOffStr = data.substring(onOffIdx, data.indexOf(':', onOffIdx));
            on_off = (onOffStr == "True");

            if (on_off) {
                for (int i = 0; i < 4; i++) {
                    controller.setMotor(i, dir, speed);
                }
            } else {
                for (int i = 0; i < 4; i++) {
                    controller.setMotor(i, 0, 0); // 停止所有馬達
                }
            }
        }
        else if (data.startsWith("platAbase") || data.startsWith("platBbase") || data.startsWith("reset")) {
            // 保留舊伺服控制
            int commaPos = data.indexOf(',');
            String valStr = data.substring(commaPos + 1);
            bool up = (valStr == "1");
            controller.servo(data.substring(0, commaPos), up);
        }
    }
}
