#include <move.h>

XboxDcMotorControl controller;

void setup() {
    Serial.begin(9600);
    controller.begin();
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        controller.processMotorData(data);
    }
}
