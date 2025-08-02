/*
    平台控制 Arduino Board A
    連接方式：直接從 UP7000 連接出來
    主要負責：控制兩個底層平臺的升降
    位置：底層
    指令格式範例一：「platA,height=50,increase=1,speed=520,force=1:」
    指令格式範例二：「platform_reset,speed=520,increase=0,force=1:」
    height = 幾公分 (cm)，會自動轉換為馬達步數
    increase = 1 (向上) / 0 (向下)
    speed = 步進馬達每秒步數
    force = 1 (啟動EN_PIN) / 0 (釋放EN_PIN)
*/

// ---- 步進馬達腳位設定 ----
const int EN_PIN   = 8;
const int stepPinX = 2;     // 平臺A
const int dirPinX  = 5;
const int stepPinY = 3;     // 平臺A
const int dirPinY  = 6;
const int stepPinZ = 4;     // 平臺B
const int dirPinZ  = 7;
const int stepPinE = 12;    // 平臺B
const int dirPinE  = 13;

// 你需根據平台結構自行設定：每 1 公分要跑幾步（依皮帶/螺桿/齒輪比等調整）
const float STEPS_PER_CM = 250.0; 

const uint8_t MICROSTEP = 1;
const long stepsPerRevolution = 200 * MICROSTEP;
unsigned int stepDelay = 800;       // 預設脈衝延遲 (微秒)

// 狀態變數：記錄平台是否正在運作，以及目前已走的步數
volatile bool runningA = false;
volatile bool runningB = false;
long stepCountA = 0;
long stepCountB = 0;

// --- Arduino 初始化 ---
void setup() {
    Serial.begin(9600);

    // 初始化所有馬達腳位為輸出模式
    pinMode(EN_PIN, OUTPUT);
    pinMode(stepPinX, OUTPUT); pinMode(dirPinX, OUTPUT);
    pinMode(stepPinY, OUTPUT); pinMode(dirPinY, OUTPUT);
    pinMode(stepPinZ, OUTPUT); pinMode(dirPinZ, OUTPUT);
    pinMode(stepPinE, OUTPUT); pinMode(dirPinE, OUTPUT);
}

// --- 主指令解析函數 ---
void processMotorData(String data) {
    data.trim();

    // --- 處理 platA/platB 指令，支援指定移動距離、高度、方向、速度等參數 ---
    if (data.startsWith("platA") || data.startsWith("platB")) {
        char platform = data.charAt(4); // 'A' or 'B'
        float height_cm = 0;
        bool increase = true;
        unsigned int speed = 800;
        bool force = true;

        // 解析參數
        int h_idx = data.indexOf("height=");
        int i_idx = data.indexOf("increase=");
        int s_idx = data.indexOf("speed=");
        int f_idx = data.indexOf("force=");

        if (h_idx >= 0) {
          int endIdx = data.indexOf(',', h_idx);
          String val = (endIdx > 0) ? data.substring(h_idx + 7, endIdx) : data.substring(h_idx + 7);
          height_cm = val.toFloat();
        }
        if (i_idx >= 0) {
          int endIdx = data.indexOf(',', i_idx);
          String val = (endIdx > 0) ? data.substring(i_idx + 9, endIdx) : data.substring(i_idx + 9);
          increase = (val.toInt() == 1);
        }
        if (s_idx >= 0) {
          int endIdx = data.indexOf(',', s_idx);
          String val = (endIdx > 0) ? data.substring(s_idx + 6, endIdx) : data.substring(s_idx + 6);
          speed = val.toInt();
        }
        if (f_idx >= 0) {
          int endIdx = data.indexOf(',', f_idx);
          String val = (endIdx > 0) ? data.substring(f_idx + 6, endIdx) : data.substring(f_idx + 6);
          force = (val.toInt() == 1);
        }

        // 依據 height 轉換要跑的步數
        long steps = (long)(height_cm * STEPS_PER_CM);
        // 根據速度參數計算脈衝間隔（愈快則delay愈小）
        unsigned int delayMicro = constrain(1000000L / speed, 500, 2000);

        if (platform == 'A') {

            // 設定平臺A移動方向與啟動
            digitalWrite(dirPinX, increase ? HIGH : LOW);
            digitalWrite(dirPinY, increase ? HIGH : LOW);
            digitalWrite(EN_PIN, force ? LOW : HIGH);
            runningA = true;
            stepDelay = delayMicro;
            stepCountA = 0;
            for (long i = 0; i < steps; i++) {
                if (!runningA) break; // 若收到stopA可強制中斷
                digitalWrite(stepPinX, HIGH);
                digitalWrite(stepPinY, HIGH);
                delayMicroseconds(stepDelay);
                digitalWrite(stepPinX, LOW);
                digitalWrite(stepPinY, LOW);
                delayMicroseconds(stepDelay);
                stepCountA++;
            }
            digitalWrite(EN_PIN, HIGH); // 停止使能，省電
            runningA = false;
        }
        else if (platform == 'B') {
            // 設定平臺B移動方向與啟動
            digitalWrite(dirPinZ, increase ? HIGH : LOW);
            digitalWrite(dirPinE, increase ? HIGH : LOW);
            digitalWrite(EN_PIN, force ? LOW : HIGH);
            runningB = true;
            stepDelay = delayMicro;
            stepCountB = 0;
            for (long i = 0; i < steps; i++) {
                if (!runningB) break; // 若收到stopB可強制中斷
                digitalWrite(stepPinZ, HIGH);
                digitalWrite(stepPinE, HIGH);
                delayMicroseconds(stepDelay);
                digitalWrite(stepPinZ, LOW);
                digitalWrite(stepPinE, LOW);
                delayMicroseconds(stepDelay);
                stepCountB++;
            }
        digitalWrite(EN_PIN, HIGH);
        runningB = false;
        }
        return;
    }

    // --- 處理 platform_reset 歸零指令（A/B一起大量步進到極限開關）---
    if (data.startsWith("platform_reset")) {
        unsigned int speed = 800;
        bool increase = true;
        bool force = true;
    
        int s_idx = data.indexOf("speed=");
        int i_idx = data.indexOf("increase=");
        int f_idx = data.indexOf("force=");

        if (s_idx > 0) {
            int endIdx = data.indexOf(',', s_idx);
            String numStr = (endIdx > 0) ? data.substring(s_idx + 6, endIdx) : data.substring(s_idx + 6);
            speed = numStr.toInt();
        }
        if (i_idx > 0) {
            int endIdx = data.indexOf(',', i_idx);
            String incStr = (endIdx > 0) ? data.substring(i_idx + 9, endIdx) : data.substring(i_idx + 9);
            increase = (incStr.toInt() == 1);
        }
        if (f_idx > 0) {
            int endIdx = data.indexOf(',', f_idx);
            String forceStr = (endIdx > 0) ? data.substring(f_idx + 6, endIdx) : data.substring(f_idx + 6);
            force = (forceStr.toInt() == 1);
        }

        unsigned int delayMicro = constrain(1000000L / speed, 500, 2000);
        long steps = 20000; // 預設一個很大的步數（根據平台最大行程可調整）

        // 設定所有馬達方向、啟動
        digitalWrite(dirPinX, increase ? HIGH : LOW);
        digitalWrite(dirPinY, increase ? HIGH : LOW);
        digitalWrite(dirPinZ, increase ? HIGH : LOW);
        digitalWrite(dirPinE, increase ? HIGH : LOW);

        digitalWrite(EN_PIN, force ? LOW : HIGH);

        // 歸零運動開始
        stepDelay = delayMicro;
        runningA = true;
        runningB = true;
        stepCountA = 0;
        stepCountB = 0;
        for (long i = 0; i < steps; i++) {
            if (!runningA && !runningB) break; // 外部stopA/stopB時可即時中斷
            if (runningA) {
                digitalWrite(stepPinX, HIGH);
                digitalWrite(stepPinY, HIGH);
            }
            if (runningB) {
                digitalWrite(stepPinZ, HIGH);
                digitalWrite(stepPinE, HIGH);
            }
            delayMicroseconds(stepDelay);
            if (runningA) {
                digitalWrite(stepPinX, LOW);
                digitalWrite(stepPinY, LOW);
                stepCountA++;
            }
            if (runningB) {
                digitalWrite(stepPinZ, LOW);
                digitalWrite(stepPinE, LOW);
                stepCountB++;
            }
            delayMicroseconds(stepDelay);
        }
        digitalWrite(EN_PIN, HIGH);
        return;
    }

    // --- 處理單獨停止指令（隨時可用stopA/stopB外部中斷任何運動）---
    if (data.startsWith("stopA")) {
        runningA = false;
        int reboundSteps = 1000;  // 預設值

        // 支援 stopA,rebound=xxx 參數
        int r_idx = data.indexOf("rebound=");
        if (r_idx >= 0) {
            int endIdx = data.indexOf(',', r_idx + 8);
            String val = (endIdx > 0) ? data.substring(r_idx + 8, endIdx) : data.substring(r_idx + 8);
            reboundSteps = val.toInt();
            if (reboundSteps <= 0) reboundSteps = 1000;  // 萬一亂輸入，回預設
        }
            // 設定方向為“上升”
            digitalWrite(dirPinX, HIGH);
            digitalWrite(dirPinY, HIGH);

        digitalWrite(EN_PIN, LOW);  // 重新使能馬達
        for (int i = 0; i < reboundSteps; i++) {
            digitalWrite(stepPinX, HIGH);
            digitalWrite(stepPinY, HIGH);
            delayMicroseconds(stepDelay);
            digitalWrite(stepPinX, LOW);
            digitalWrite(stepPinY, LOW);
            delayMicroseconds(stepDelay);
        }
        digitalWrite(EN_PIN, HIGH); // 釋放使能，節省電力
        return;
    }
    if (data.startsWith("stopB")) {
        runningB = false;
        digitalWrite(dirPinZ, HIGH);
        digitalWrite(dirPinE, HIGH);

        int reboundSteps = 1000;
        digitalWrite(EN_PIN, LOW);
        for (int i = 0; i < reboundSteps; i++) {
            digitalWrite(stepPinZ, HIGH);
            digitalWrite(stepPinE, HIGH);
            delayMicroseconds(stepDelay);
            digitalWrite(stepPinZ, LOW);
            digitalWrite(stepPinE, LOW);
            delayMicroseconds(stepDelay);
        }
        digitalWrite(EN_PIN, HIGH);
        return;
    }
}

// --- loop：僅監聽Serial訊息並執行運動 ---
void loop() {
    // 你如果要定時回報/監控，也可放在這裡
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        data.trim();  // 移除換行符號或空白
        processMotorData(data);
    }
}
