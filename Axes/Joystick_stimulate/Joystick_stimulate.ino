const int LED_PIN = 13;  // 内置LED用于显示按钮状态

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  while (!Serial);  // 等待串口连接
    Serial.println("Ready to receive data...");
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    // 解析数据
    int x = 0, y = 0, button = 0;
    sscanf(data.c_str(), "%d,%d,%d", &x, &y, &button);
    
    // 显示数据
    Serial.print("Received - ");
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.print(y);
    Serial.print(" | Button: ");
    Serial.println(button ? "PRESSED" : "RELEASED");
    
    // 控制LED显示按钮状态
    digitalWrite(LED_PIN, button ? HIGH : LOW);
  }
}