#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200 // 一圈200步（每步1.8度）

void setup() {
  // put your setup code here, to run once:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  // 測試移動 10 cm
  float distance_cm = 5.0;
  int steps = (distance_cm / calculate_cm_per_step());

  // 上升
  digitalWrite(dirPin, HIGH);  // 順時針

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);  // 控制速度
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }

  delay(2000); // 暫停兩秒

  // 下降
  digitalWrite(dirPin, LOW);  // 逆時針

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);  // 控制速度
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }

  delay(2000); // 暫停兩秒
}

float calculate_cm_per_step(){
  // 導程
  float Leadperrevolution_cm = 0.8; // 絲桿導程：一圈推進0.8cm
  return (Leadperrevolution_cm/stepsPerRevolution); // 每步推進距離（cm）
}