#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ---- Servo Driver 設定 ----
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN 150  // 最小脈寬 (out of 4096)
#define SERVOMAX 600  // 最大脈寬

struct Motor {
    int forwardPin;
    int backwardPin;
    int speedPin;
};

Motor motors[4] = {
    {26, 28, 2},   // 左前輪 (A)
    {27, 29, 3},   // 右前輪 (B)
    {30, 32, 4},   // 左後輪 (C)
    {31, 33, 5}   // 右後輪 (D)
};

void setup() {
    Serial.begin(9600);
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);
    }
    pwm.begin();
    pwm.setPWMFreq(60);  // 建議頻率 50~60Hz
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processMotorData(data);
    }
}

void processMotorData(String data) {
    String Data[5];
    int index = 0;

    // 解析數據 (格式: "1,150:0,120:1,100:0,0:platAbase,1\n")
    while (data.length() > 0 && index < 5) {
        int colonPos = data.indexOf(':');
        if (colonPos == -1) {
            Data[index++] = data;
            data = "";
        } else {
            Data[index++] = data.substring(0, colonPos);
            data = data.substring(colonPos+1);
        }
    }

    // 控制每個馬達
    for (int i = 0; i < 4; i++) {
        int commaPos = Data[i].indexOf(',');
        if (commaPos != -1) {
            int dir = Data[i].substring(0, commaPos).toInt();
            int speed = Data[i].substring(commaPos+1).toInt();

            if (speed > 0){
                digitalWrite(motors[i].forwardPin, dir);
                digitalWrite(motors[i].backwardPin, !dir);
                analogWrite(motors[i].speedPin, speed);
            }
            else {
                digitalWrite(motors[i].forwardPin, 0);
                digitalWrite(motors[i].backwardPin, 0);
                analogWrite(motors[i].speedPin, speed);
            }
        }
    }
    int commaPos = Data[4].indexOf(',');
    if (commaPos != -1) {
        String platform = Data[4].substring(0, commaPos);
        String valStr = Data[4].substring(commaPos+1);
        bool val = (valStr == "1");
        Servo(platform, val == "1");
    }
}

void Servo(String platform,bool up) {
    if (platform == "platAbase" || platform == "platBbase") {
        for (uint8_t ch = 0; ch < 16; ch++) {
            int angle = random(0, 271);
            int pulse = map(angle, 0, 270, SERVOMIN, SERVOMAX);
            pwm.setPWM(ch, 0, pulse);
            Serial.print("Channel "); Serial.print(ch);
            Serial.print(" -> Angle: "); Serial.println(angle);
            delay(500);
        }
    } else {
        for (uint8_t ch = 0; ch < 16; ch++) {
            int pulse = map(0, 0, 270, SERVOMIN, SERVOMAX);  // 0 度位置
            pwm.setPWM(ch, 0, pulse);
            Serial.print("Reset Channel "); Serial.println(ch);
            delay(500);  // 可調整速度
        }
    }
}