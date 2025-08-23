/*
    平台控制 Arduino Board B
    連接方式：直接從 UP7000 連接出來
    功能範圍：負責控制整台機器的各種動作（平台、夾具、咖啡機等）
    位置：整台機器的上層控制系統
*/

// ====== 引入自訂及外部函式庫 ======
#include <move.h>         // 控制直流馬達（DC Motor）的類別
#include <Climb.h>        // 控制爬升平台伺服馬達的類別
#include <FruitPicker.h>  // 控制水果夾持模組（含直流馬達與伺服）
#include <CoffeeMove.h>   // 控制咖啡機夾杯與托盤的伺服馬達
#include <Bucket.h>       // 控制桶子伺服的類別

// ====== 建立各功能模組物件 ======
XboxDcMotorControl controller;  // 控制四顆 DC 馬達（平台行走用）
climbServoManager ClimbServo;   // 控制爬升平台伺服
FruitPicker Fruit;              // 水果夾持模組
coffeeServoManager CoffeeServo; // 咖啡機伺服模組
bucketServoManager Bucket;      // Bucket 伺服模組

// ====== 極限開關腳位（平台位置檢測） ======
const int limitSwitchA = 42;  // 平台 A 底部極限開關
const int limitSwitchB = 43;  // 平台 B 底部極限開關

// ====== 紀錄極限開關的上一個狀態（用於判斷變化） ======
bool lastLimitA = HIGH; // 平台 A 上一次的開關狀態（HIGH=未觸發）
bool lastLimitB = HIGH; // 平台 B 上一次的開關狀態

// ====== 初始化階段 ======
void setup() {
    Serial.begin(9600);    // 與上位機（例如 Python）串口通信
    Serial1.begin(9600);   // 與下層板子通信（板對板傳輸）

    // 設定極限開關為內部上拉輸入（未觸發時為 HIGH）
    pinMode(limitSwitchA, INPUT_PULLUP);
    pinMode(limitSwitchB, INPUT_PULLUP);

    // 初始化各控制模組
    controller.begin();  // 直流馬達控制初始化
    ClimbServo.begin();  // 爬升伺服初始化
    Fruit.begin();
    CoffeeServo.begin(); // 咖啡伺服初始化
    Bucket.begin();
}

// ====== 主循環 ======
void loop() {
    // ========= 極限開關狀態檢測 =========
    bool nowLimitA = digitalRead(limitSwitchA); // 讀取平台 A 狀態
    bool nowLimitB = digitalRead(limitSwitchB); // 讀取平台 B 狀態

    // 檢測「下降沿觸發」：HIGH → LOW，表示碰到底部位置
    if (lastLimitA == HIGH && nowLimitA == LOW) {
        Serial.println("PlatformA_Bottom"); // 回報平台 A 到達底部
    }
    if (lastLimitB == HIGH && nowLimitB == LOW) {
        Serial.println("PlatformB_Bottom"); // 回報平台 B 到達底部
    }

    // 更新狀態記錄
    lastLimitA = nowLimitA;
    lastLimitB = nowLimitB;

    // ========= 處理上位機傳來的指令 =========
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // 讀取一行完整指令
        data.trim(); // 去除前後空白

        // ====== 將指令分段（以 ':' 分隔，可同時下多條指令） ======
        int last = 0;
        int idx = 0;
        String segments[10]; // 最多 10 段指令
        while (last < data.length()) {
            int colon = data.indexOf(':', last);
            if (colon == -1) {
                segments[idx++] = data.substring(last);
                break;
            } else {
                segments[idx++] = data.substring(last, colon);
                last = colon + 1;
            }
        }

        int motorIndex = 0; // 用來記錄目前處理的是哪顆 DC 馬達（依順序）

        // ====== 逐段解析指令 ======
        for (int i = 0; i < idx; i++) {
            String segment = segments[i];
            segment.trim();

            // ====== 處理 DC 馬達移動指令 ======
            // 格式範例: move,dir=1,speed=100,on_off=1:
            if (segment.startsWith("move") || segment.startsWith("turn")) {
                int dir = 0, speed = 0;
                bool on_off = false;

                // 抓取 dir 參數
                int dirIdx = segment.indexOf("dir=") + 4;
                int speedIdx = segment.indexOf("speed=") + 6;
                int onOffIdx = segment.indexOf("on_off=") + 7;

                dir = segment.substring(dirIdx, segment.indexOf(',', dirIdx)).toInt();
                speed = segment.substring(speedIdx, segment.indexOf(',', speedIdx)).toInt();

                // 取 on_off（啟動狀態）
                String onOffStr = segment.substring(onOffIdx);
                if (onOffStr.indexOf(',') != -1)
                    onOffStr = onOffStr.substring(0, onOffStr.indexOf(','));
                on_off = (onOffStr == "1");

                // 控制對應馬達
                if (on_off) {
                    controller.setMotor(motorIndex, dir, speed);
                } else {
                    controller.setMotor(motorIndex, 0, 0);
                }
                motorIndex++;
            }

            // ====== 處理水果夾持指令 ======
            // 格式範例: fruit,dir=-1,speed=100,on_off=True:
            else if (segment.startsWith("fruit")) {
                if (segment.indexOf("dir=") != -1) {
                    // 解析三個參數：dir、speed、on_off
                    int dirIdx = segment.indexOf("dir=") + 4;
                    int dirEnd = paramEnd(segment, dirIdx);
                    int dir = segment.substring(dirIdx, dirEnd).toInt();

                    int speedIdx = segment.indexOf("speed=") + 6;
                    int speedEnd = paramEnd(segment, speedIdx);
                    int speed = segment.substring(speedIdx, speedEnd).toInt();

                    int onOffIdx = segment.indexOf("on_off=") + 7;
                    int onOffEnd = paramEnd(segment, onOffIdx);
                    String onOffStr = segment.substring(onOffIdx, onOffEnd);
                    onOffStr.trim(); onOffStr.toLowerCase();
                    bool on_off = (onOffStr == "1" || onOffStr == "true");

                    // 防呆修正
                    int dirSafe   = (dir > 0) ? 1 : (dir < 0 ? -1 : 0);
                    int speedSafe = constrain(speed, 0, 255);

                    // 同時控制兩顆直流馬達
                    for (int i = 0; i < 2; ++i) {
                        if (on_off) {
                            Fruit.setDC(i, dirSafe, speedSafe);
                        } else {
                            Fruit.setDC(i, 0, 0);
                        }
                    }
                } else {
                    // 如果沒有 dir 參數，直接轉發給另一塊板子
                    Serial1.println(segment);
                }
            }

            // ====== 處理平台伺服指令 ======
            // 格式範例: platA_base,channel=3,initial=10,end=20,increment=1,ini_to_end=True:
            else if (segment.startsWith("platA_base") || segment.startsWith("platB_base")) {
                if (segment.indexOf("channel=") != -1) {
                    // 解析各參數
                    int channelIdx = segment.indexOf("channel=") + 8;
                    int channelCommaIdx = segment.indexOf(',', channelIdx);
                    int channel = segment.substring(channelIdx, channelCommaIdx).toInt();

                    int initialIdx = segment.indexOf("initial=") + 8;
                    int initialCommaIdx = segment.indexOf(',', initialIdx);
                    int initial = segment.substring(initialIdx, initialCommaIdx).toInt();

                    int endIdx = segment.indexOf("end=") + 4;
                    int endCommaIdx = segment.indexOf(',', endIdx);
                    int end = segment.substring(endIdx, endCommaIdx).toInt();

                    int incrementIdx = segment.indexOf("increment=") + 10;
                    int incrementCommaIdx = segment.indexOf(',', incrementIdx);
                    int increment = segment.substring(incrementIdx, incrementCommaIdx).toInt();

                    int ini_to_endIdx = segment.indexOf("ini_to_end=") + 11;
                    int ini_to_endColonIdx = segment.indexOf(':', ini_to_endIdx);
                    String iniToEndStr = segment.substring(ini_to_endIdx, ini_to_endColonIdx);
                    bool ini_to_end = (iniToEndStr == "True" || iniToEndStr == "1");

                    // 執行平台伺服動作
                    ClimbServo.moveServo(channel, initial, end, increment, ini_to_end);
                }
                // 格式範例: platB_base,dir=1,speed=70,on_off=1:
                else if (segment.indexOf("dir=") != -1) {
                    int dirIdx = segment.indexOf("dir=") + 4;
                    int dirCommaIdx = segment.indexOf(',', dirIdx);
                    int dir = segment.substring(dirIdx, dirCommaIdx).toInt();

                    int speedIdx = segment.indexOf("speed=") + 6;
                    int speedCommaIdx = segment.indexOf(',', speedIdx);
                    int speed = segment.substring(speedIdx, speedCommaIdx).toInt();

                    int onOffIdx = segment.indexOf("on_off=") + 7;
                    int onOffColonIdx = segment.indexOf(':', onOffIdx);
                    String onOffStr = segment.substring(onOffIdx, onOffColonIdx);

                    bool on_off = (onOffStr == "1" || onOffStr == "True");

                    ClimbServo.setClimbMotor(dir, speed, on_off);
                }
            }

            // ====== 處理咖啡機伺服指令 ======
            // 格式範例: coffee,channel=8,initial=10,end=20,increment=1,ini_to_end=True:
            else if (segment.startsWith("coffee")) {
                if (segment.indexOf("channel=") != -1){
                    // 先定義變數
                    int  channel   = -1;
                    int  initial   = -1;
                    int  end       = -1;
                    int  increment = 1;
                    bool ini_to_end = true;
                    bool hasIni     = false;
                    bool reset      = false;
                    bool hasReset   = false;

                    // 依序解析 channel、initial、end、increment、ini_to_end、reset
                    int pos = segment.indexOf("channel=");
                    if (pos != -1) {
                        pos += 8;
                        int e = paramEnd(segment, pos);
                        channel = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("initial=");
                    if (pos != -1) {
                        pos += 8;
                        int e = paramEnd(segment, pos);
                        initial = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("end=");
                    if (pos != -1) {
                        pos += 4;
                        int e = paramEnd(segment, pos);
                        end = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("increment=");
                    if (pos != -1) {
                        pos += 10;
                        int e = paramEnd(segment, pos);
                        increment = segment.substring(pos, e).toInt();
                        if (increment <= 0) increment = 1;
                    }
                    pos = segment.indexOf("ini_to_end=");
                    if (pos != -1) {
                        hasIni = true;
                        pos += 11;
                        int e = paramEnd(segment, pos);
                        String v = segment.substring(pos, e);
                        v.trim();
                        v.toLowerCase();
                        ini_to_end = (v == "1" || v == "true");
                    }
                    pos = segment.indexOf("reset=");
                    if (pos != -1) {
                        hasReset = true;
                        pos += 6;
                        int e = paramEnd(segment, pos);
                        String rs = segment.substring(pos, e);
                        rs.trim();
                        rs.toLowerCase();
                        reset = (rs == "1" || rs == "true");
                    }

                    // 驗證必要參數
                    if (channel >= 0 && initial >= 0 && end >= 0) {
                        if (hasIni) {
                            CoffeeServo.grabCup(channel, initial, end, increment, ini_to_end);
                        } else if (hasReset) {
                            CoffeeServo.removePlate(channel, initial, end, increment, reset);
                        }
                    }
                }
            }
            // ====== 處理 Bucket（前臂/夾具）伺服指令 ======
            // 單向格式範例：bucket,channel=7,initial=90,end=0,increment=2,ini_to_end=True:
            else if (segment.startsWith("bucket")) {
                if (segment.indexOf("channel=") != -1){
                    // 必要參數
                    int  channel    = -1;
                    int  initial    = -1;
                    int  end        = -1;
                    int  increment  = 1;      // 預設 1（避免 0）
                    bool ini_to_end = true;   // True: initial->end；False: end->initial
                    bool hasIni     = false;
                    bool reset      = false;

                    // 依序解析 channel、initial、end、increment、ini_to_end、(reset)
                    int pos = segment.indexOf("channel=");
                    if (pos != -1) {
                        pos += 8;
                        int e = paramEnd(segment, pos);
                        channel = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("initial=");
                    if (pos != -1) {
                        pos += 8;
                        int e = paramEnd(segment, pos);
                        initial = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("end=");
                    if (pos != -1) {
                        pos += 4;
                        int e = paramEnd(segment, pos);
                        end = segment.substring(pos, e).toInt();
                    }
                    pos = segment.indexOf("increment=");
                    if (pos != -1) {
                        pos += 10;
                        int e = paramEnd(segment, pos);
                        increment = segment.substring(pos, e).toInt();
                        if (increment <= 0) increment = 1;
                    }
                    pos = segment.indexOf("ini_to_end=");
                    if (pos != -1) {
                        hasIni = true;
                        pos += 11;
                        int e = paramEnd(segment, pos);
                        String v = segment.substring(pos, e);
                        v.trim(); v.toLowerCase();
                        ini_to_end = (v == "1" || v == "true");
                    }
                    // 解析階段若沒帶 ini_to_end，請預設：ini_to_end = true
                    // 這裡只做參數檢查 + 依 channel 路由
                    if (channel >= 0 && initial >= 0 && end >= 0) {
                        // 將 bucket 的單向動作統一由 ini_to_end 決定方向
                        if (channel == 11) {
                            // 11 號：前臂夾取（單臂）
                            Bucket.FrontArmsGrab(channel, initial, end, increment, ini_to_end);
                        } else if (channel == 12 || channel == 13) {
                            // 12/13 號：展開前臂（雙/前臂版本）
                            Bucket.ExpandFrontArmsForGrab(channel, initial, end, increment, ini_to_end);
                        }
                    }
                }
            }
        }
    }
}

// ====== 輔助函式 ======
// 找到某參數的結束位置（逗號、冒號或字串結尾）
int paramEnd(const String& s, int startIdx) {
    int end = s.indexOf(',', startIdx);
    return (end == -1) ? s.length() : end;
}
