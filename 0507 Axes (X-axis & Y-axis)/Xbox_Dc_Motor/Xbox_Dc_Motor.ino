struct Motor {
    int forwardPin;
    int backwardPin;
    int speedPin;
};

Motor motors[4] = {
    {2, 3, 6},   // 左前輪 (A)
    {4, 5, 6},   // 右前輪 (B)
    {7, 9, 6},   // 左後輪 (C)
    {8, 10, 6}   // 右後輪 (D)
};

void setup() {
    Serial.begin(9600);
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);
    }
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processMotorData(data);
    }
}

void processMotorData(String data) {
    String wheelData[4];
    int index = 0;

    // 解析數據 (格式: "1,200:0,150:1,100:0,255")
    while (data.length() > 0 && index < 4) {
        int colonPos = data.indexOf(':');
        if (colonPos == -1) {
            wheelData[index++] = data;
            data = "";
        } else {
            wheelData[index++] = data.substring(0, colonPos);
            data = data.substring(colonPos+1);
        }
    }

    // 控制每個馬達
    for (int i = 0; i < 4; i++) {
        int commaPos = wheelData[i].indexOf(',');
        if (commaPos != -1) {
            int dir = wheelData[i].substring(0, commaPos).toInt();
            int speed = wheelData[i].substring(commaPos+1).toInt();

            digitalWrite(motors[i].forwardPin, dir);
            digitalWrite(motors[i].backwardPin, !dir);
            analogWrite(motors[i].speedPin, speed);
        }
    }
}
