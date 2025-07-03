// Merged Arduino Sketch: Stepper Motors & PCA9685 Servos
// 合併 Arduino 草圖：步進馬達與 PCA9685 伺服器

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ---- Stepper Motor 設定 ----
// 定義步進與方向腳位
const int stepPinX = 2;
const int dirPinX  = 5;
const int stepPinY = 3;
const int dirPinY  = 6;
const int stepPinZ = 4;
const int dirPinZ  = 7;
const int stepPinE = 12;
const int dirPinE  = 13;

// 步數與延遲
const uint8_t MICROSTEP = 1;  
const long stepsPerRevolution = 200 * MICROSTEP;
const unsigned int stepDelay = 800;       // 脈衝延遲 (微秒)

// ---- Servo Driver 設定 ----
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN 150  // 最小脈寬 (out of 4096)
#define SERVOMAX 600  // 最大脈寬

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

  // 初始化 PCA9685
  pwm.begin();               
  pwm.setPWMFreq(60);       // 設定伺服頻率 60Hz
  randomSeed(analogRead(A0));
}

// 步進馬達 5 圈正/反轉
void stepAllMotors(long revCount) {
  long steps = stepsPerRevolution * revCount;
  // 正轉
  digitalWrite(dirPinX, HIGH);
  digitalWrite(dirPinY, HIGH);
  digitalWrite(dirPinZ, HIGH);
  digitalWrite(dirPinE, HIGH);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinX, HIGH);
    digitalWrite(stepPinY, HIGH);
    digitalWrite(stepPinZ, HIGH);
    digitalWrite(stepPinE, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinX, LOW);
    digitalWrite(stepPinY, LOW);
    digitalWrite(stepPinZ, LOW);
    digitalWrite(stepPinE, LOW);
    delayMicroseconds(stepDelay);
  }

  delay(1000);

  // 反轉
  digitalWrite(dirPinX, LOW);
  digitalWrite(dirPinY, LOW);
  digitalWrite(dirPinZ, LOW);
  digitalWrite(dirPinE, LOW);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPinX, HIGH);
    digitalWrite(stepPinY, HIGH);
    digitalWrite(stepPinZ, HIGH);
    digitalWrite(stepPinE, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPinX, LOW);
    digitalWrite(stepPinY, LOW);
    digitalWrite(stepPinZ, LOW);
    digitalWrite(stepPinE, LOW);
    delayMicroseconds(stepDelay);
  }

  delay(1000);
}

// 隨機控制所有 16 通道伺服
void randomServoCycle() {
  for (uint8_t ch = 0; ch < 16; ch++) {
    int angle = random(0, 271);
    int pulse = map(angle, 0, 270, SERVOMIN, SERVOMAX);
    pwm.setPWM(ch, 0, pulse);
    Serial.print("Channel "); Serial.print(ch);
    Serial.print(" -> Angle: "); Serial.println(angle);
    delay(1500);
  }
}

void loop() {
  // 1) 步進馬達運行：正/反轉 5 圈
  // 1) Steppers: 5 revolutions forward & backward
  stepAllMotors(5);

  // 2) 伺服隨機擺動
  // 2) Servos: random sweep on all channels
  randomServoCycle();
}
