// --- 腳位定義 ---
const int ENABLE_PIN = 8;
const int stepPin = 2;    // 平台A
const int dirPin  = 5;

long stepCount = 0;
bool running = false;

void setup() {
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);

  digitalWrite(ENABLE_PIN, LOW); // 使能
  digitalWrite(dirPin, HIGH);    // 設定方向（上移）
}

void loop() {
  // 檢查是否收到指令
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'S') { // Start
      running = true;
      stepCount = 0;
    } else if (cmd == 'X') { // Stop
      running = false;
      Serial.print("StepCount=");
      Serial.println(stepCount); // 回報步數
    }
  }

  // 執行步進馬達動作
  if (running) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800); // 根據馬達調整
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
    stepCount++;
  }
}
