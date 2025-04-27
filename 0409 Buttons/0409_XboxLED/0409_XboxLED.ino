void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600); // 波特率设置为 9600
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    digitalWrite(13, command == '1' ? HIGH : LOW);
  }
}