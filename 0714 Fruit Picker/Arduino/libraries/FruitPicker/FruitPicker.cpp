#include <FruitPicker.h>

// 建構子，初始化兩個直流馬達的腳位設定
FruitPicker::FruitPicker() {
    motors[0] = {38, 40, 8};  // 左前輪 (A)
    motors[1] = {39, 41, 9};  // 右前輪 (B)
}

// 步進馬達與使能腳位（需補上正確腳位數值）
const int EN_PIN    = 8;    // 步進馬達使能腳位
const int stepPinX  = 2;    // X軸步進馬達脈衝腳位
const int dirPinX   = 5;    // X軸步進馬達方向腳位
const int stepPinY  = 3;    // Y軸步進馬達脈衝腳位
const int dirPinY   = 6;    // Y軸步進馬達方向腳位

// 初始化馬達腳位
void FruitPicker::begin() {
    for(int i=0; i<2; i++ ) {
        pinMode(motors[i].forwardPin, OUTPUT);   // 設定前進腳位為輸出
        pinMode(motors[i].backwardPin, OUTPUT);  // 設定後退腳位為輸出
        pinMode(motors[i].speedPin, OUTPUT);     // 設定速度(PWM)腳位為輸出
    }
    // 步進馬達控制腳位初始化
    pinMode(EN_PIN, OUTPUT);
    pinMode(stepPinX, OUTPUT);
    pinMode(dirPinX, OUTPUT);
    pinMode(stepPinY, OUTPUT);
    pinMode(dirPinY, OUTPUT);
}

// 控制直流馬達方向與速度
void FruitPicker::setDC(int index, int direction, int speed) {
    if (direction == 1) { // 正轉
        digitalWrite(motors[index].forwardPin, HIGH);   // 前進腳位高電位
        digitalWrite(motors[index].backwardPin, LOW);   // 後退腳位低電位
        analogWrite(motors[index].speedPin, speed);     // 設定PWM速度
    } else if (direction == -1) { // 反轉
        digitalWrite(motors[index].forwardPin, LOW);    // 前進腳位低電位
        digitalWrite(motors[index].backwardPin, HIGH);  // 後退腳位高電位
        analogWrite(motors[index].speedPin, speed);     // 設定PWM速度
    } else { // 停止
        digitalWrite(motors[index].forwardPin, LOW);    // 前進腳位低電位
        digitalWrite(motors[index].backwardPin, LOW);   // 後退腳位低電位
        analogWrite(motors[index].speedPin, 0);         // 關閉PWM速度
    }
}

void FruitPicker::setStepper(int height, bool increase, int speed, bool force) {
    // 激磁控制
    if (force) {
        digitalWrite(EN_PIN, LOW);   // 低電位 → 使能（抱死）
    } else {
        digitalWrite(EN_PIN, HIGH);  // 高電位 → 釋放
        return;  // 如果 force=0，直接離開，不執行動作
    }

    // 設定方向
    digitalWrite(dirPinX, increase ? HIGH : LOW);
    digitalWrite(dirPinY, increase ? HIGH : LOW);

    // 計算步數（簡單用高度對應）
    long targetSteps = map(height, 0, 200, 0, stepsPerRevolution);  // 假設200對應最高

    // 計算延遲（簡單用速度對應）
    unsigned int delayMicro = constrain(1000000L / speed, 500, 2000);  // 最小500µs，最大2000µs

    // 執行步進
    for (long step = 0; step < targetSteps; step++) {
        digitalWrite(stepPinX, HIGH);
        digitalWrite(stepPinY, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPinX, LOW);
        digitalWrite(stepPinY, LOW);
        delayMicroseconds(stepDelay);
    }
}