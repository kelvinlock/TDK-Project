// 定義馬達結構體
struct Motor {
  int forwardPin;   // 馬達正轉引腳
  int backwardPin;  // 馬達反轉引腳
  int speedPin;     // 馬達使能引腳（控制速度）
};

Motor motors[4] = {
  {2, 3, 6},    // 馬達1：forwardPin=2, backwardPin=3, enablePin=6
  {4, 5, 6},    // 馬達2：forwardPin=4, backwardPin=5, enablePin=6
  {7, 9, 6},    // 馬達3：forwardPin=7, backwardPin=9, enablePin=6
  {8, 10, 6}    // 馬達4：forwardPin=8, backwardPin=10, enablePin=6
}
void setup() {
  // 初始化串行通訊S
  Serial.begin(9600);

  // 設置馬達引腳為輸出
  for(int a = 0 ; a < 4 ; a++) {
    pinMode(motors[a].forwardPin, OUTPUT);
    pinMode(motors[a].backwardPin, OUTPUT);
    pinMode(motors[a].enablePin, OUTPUT);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    // 解析數據格式 "PWM:255"
    if (data.startsWith("X:")) {
      int speed = data.substring(4).toInt();
      speed = constrain(speed, 0, 255);  // 確保值在合法範圍

      // 強化停止判斷（127±5為死區）
      if (speed >= 132) {         // 前進
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(enablePin, speed);
      } else if (speed <= 122) {  // 後退
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(enablePin, 255 - speed);  // 反轉時保持速度線性
      } else {                    // 停止
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        analogWrite(enablePin, 0);
      }
    }
  }
}

void move(int[])