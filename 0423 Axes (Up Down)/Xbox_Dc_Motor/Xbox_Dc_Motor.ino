const int IN1 = 3;
const int IN2= 4;
const int enablePin = 9;

void setup() {
  Serial.begin(9600);

  // Set motor control pins as outputs
  pinMode(enablePin, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    // 解析數據格式 "PWM:255"
    if (data.startsWith("PWM:")) {
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