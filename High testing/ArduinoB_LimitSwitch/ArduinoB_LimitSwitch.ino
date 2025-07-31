// --- 極限開關腳位定義 ---
const int limitSwitchPin = 42;  // 你用的平台A的極限開關腳位

void setup() {
  pinMode(limitSwitchPin, INPUT_PULLUP); // 極限開關一端GND，一端接腳位
  Serial.begin(9600);
}

void loop() {
  static bool lastState = HIGH;
  bool currState = digitalRead(limitSwitchPin);

  // 偵測「下降緣」(HIGH -> LOW)，極限開關被觸發
  if (lastState == HIGH && currState == LOW) {
    Serial.println("LimitSwitchTriggered");
    delay(30); // 消抖
  }
  lastState = currState;
}
