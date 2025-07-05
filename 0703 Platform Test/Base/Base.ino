// ---- Stepper Motor 設定 ----
// 定義步進與方向腳位
const int stepPinX = 2;     # 平臺A
const int dirPinX  = 5;
const int stepPinY = 3;     # 平臺A
const int dirPinY  = 6;
const int stepPinZ = 4;     # 平臺B
const int dirPinZ  = 7;
const int stepPinE = 12;    # 平臺B
const int dirPinE  = 13;

// 步數與延遲
const uint8_t MICROSTEP = 1;  
const long stepsPerRevolution = 200 * MICROSTEP;
const unsigned int stepDelay = 800;       // 脈衝延遲 (微秒)

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // 初始化步進馬達腳位
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  pinMode(stepPinZ, OUTPUT);
  pinMode(dirPinZ, OUTPUT);
  pinMode(stepPinE, OUTPUT);
  pinMode(dirPinE, OUTPUT);
}

// 步進馬達 5 圈正/反轉
void stepAllMotors_platA_UP(long revCount) {
  long steps = stepsPerRevolution * revCount;
  // 正轉
  digitalWrite(dirPinX, HIGH);
  digitalWrite(dirPinY, HIGH);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinX, HIGH);
    digitalWrite(stepPinY, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinX, LOW);
    digitalWrite(stepPinY, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(1000);
}

void stepAllMotors_platB_UP(long revCount) {
  long steps = stepsPerRevolution * revCount;
  // 正轉
  digitalWrite(dirPinZ, HIGH);
  digitalWrite(dirPinE, HIGH);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinZ, HIGH);
    digitalWrite(stepPinE, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinZ, LOW);
    digitalWrite(stepPinE, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(1000);
}

void stepAllMotors_PlatA_DOWN(long revCount) {
  long steps = stepsPerRevolution * revCount;
    // 反轉
  digitalWrite(dirPinX, LOW);
  digitalWrite(dirPinY, LOW);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinX, HIGH);
    digitalWrite(stepPinY, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinX, LOW);
    digitalWrite(stepPinY, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(1000);
}

void stepAllMotors_PlatB_DOWN(long revCount) {
  long steps = stepsPerRevolution * revCount;
    // 反轉
  digitalWrite(dirPinZ, LOW);
  digitalWrite(dirPinE, LOW);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinZ, HIGH);
    digitalWrite(stepPinE, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinZ, LOW);
    digitalWrite(stepPinE, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(1000);
}

void processMotorData(data) {
  // 解析數據 (格式: platA,1)
  int commaPos = data.indexOf(',');

  String Platform = data.substring(0, commaPos);
  String value = data.substring(commaPos+1);

  if (command == "platA") {
    if (value == "1") {
      stepAllMotors_platA_UP(5);
    } else if (value == "0") {
      stepAllMotors_PlatA_DOWN(5);
    }
  } else if (command == "platB") {
    if (value == "1") {
      stepAllMotors_platB_UP(5);
    } else if (value == "0") {
      stepAllMotors_PlatB_DOWN(5);
    }
  }
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();  // 移除換行符號或空白
    processMotorData(data);
  }
}
