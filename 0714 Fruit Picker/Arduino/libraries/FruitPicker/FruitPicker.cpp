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

unsigned long magnetOnTime = 0;
bool holding = false;

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

    digitalWrite(EN_PIN, LOW);  // 激磁（馬達上鎖，平台保持定位）
    // 1. 設定步進馬達的運動方向
    // increase 為 true 時 HIGH，反之為 LOW（根據實際馬達需求調整）
    digitalWrite(dirPinX, increase ? HIGH : LOW);
    digitalWrite(dirPinY, increase ? HIGH : LOW);

    // 2. 計算需要移動的步數
    // 將 height（例如 0~200）映射到一圈的步數，請根據實際平台高度與馬達對應調整
    long targetSteps = map(height, 0, 200, 0, stepsPerRevolution);

    // 3. 根據速度計算脈衝延遲時間
    // speed 愈大 delay 愈短，馬達轉得愈快，最低500微秒，最高2000微秒
    unsigned int delayMicro = constrain(1000000L / speed, 500, 2000);

    // 4. 依照計算好的步數執行步進脈衝
    for (long step = 0; step < targetSteps; step++) {
        digitalWrite(stepPinX, HIGH);  // X軸步進脈衝
        digitalWrite(stepPinY, HIGH);  // Y軸步進脈衝
        delayMicroseconds(delayMicro); // 保持 HIGH 狀態一段時間
        digitalWrite(stepPinX, LOW);
        digitalWrite(stepPinY, LOW);
        delayMicroseconds(delayMicro); // 保持 LOW 狀態一段時間
    }
    magnetOnTime = millis();    // 記錄激磁開始時間
    holding = true;             // 設為等待釋放
}
