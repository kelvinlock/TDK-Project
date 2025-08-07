/*
    平台控制 Arduino Board B
    連接方式：直接從 UP7000 連接出來
    主要負責：控制各種動作
    位置：整個機器
*/
#include <move.h>
#include <Climb.h>

XboxDcMotorControl controller;
climbServoManager ClimbServo;
coffeeServoManager CoffeeServo;
const int limitSwitchA = 42;  // 平台A 極限開關腳位
const int limitSwitchB = 43;  // 平台B 極限開關腳位

bool lastLimitA = HIGH; // 記錄上一次平台A極限開關狀態
bool lastLimitB = HIGH; // 記錄上一次平台B極限開關狀態

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    pinMode(limitSwitchA, INPUT_PULLUP); // 設定極限開關腳位為上拉輸入
    pinMode(limitSwitchB, INPUT_PULLUP);
    controller.begin(); // 初始化馬達控制器
    ClimbServo.begin();
}

void loop() {
    // ========= 極限開關偵測與回報（僅變化時回傳）=========
    bool nowLimitA = digitalRead(limitSwitchA);
    bool nowLimitB = digitalRead(limitSwitchB);
    // 若檢測到 HIGH 轉 LOW（下降沿），代表到達頂部，只回報一次
    if (lastLimitA == HIGH && nowLimitA == LOW) {
        Serial.println("PlatformA_Bottom");  // 通知上位機，平台A到頂
    }
    if (lastLimitB == HIGH && nowLimitB == LOW) {
        Serial.println("PlatformB_Bottom");  // 通知上位機，平台B到頂
    }
    lastLimitA = nowLimitA; // 更新狀態
    lastLimitB = nowLimitB;

    // ========= 指令接收與分割處理 =========
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // 讀取一行串口數據
        data.trim(); // 去除字串首尾空白

        // 使用 ':' 拆分成多個指令段（例如一次控制多顆馬達）
        int last = 0;
        int idx = 0;
        String segments[10]; // 最多10個指令段
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

        int motorIndex = 0; // 馬達索引（左前、右前、左後、右後）

        // 依序解析每個指令段(segment)
        for (int i = 0; i < idx; i++) {
            String segment = segments[i];
            segment.trim();

            // 支援 "move" 或 "turn" 開頭的直流馬達控制指令
            if (segment.startsWith("move") || segment.startsWith("turn")) {
                int dir = 0, speed = 0;
                bool on_off = false;

                // 解析 dir、speed、on_off 參數
                int dirIdx = segment.indexOf("dir=") + 4;
                int speedIdx = segment.indexOf("speed=") + 6;
                int onOffIdx = segment.indexOf("on_off=") + 7;

                // 取得馬達方向（1前進、-1後退、0停止）
                dir = segment.substring(dirIdx, segment.indexOf(',', dirIdx)).toInt();
                // 取得馬達速度（0~255）
                speed = segment.substring(speedIdx, segment.indexOf(',', speedIdx)).toInt();
                // on_off 取到字串結尾（判斷是否要啟動馬達）
                String onOffStr = segment.substring(onOffIdx);
                if (onOffStr.indexOf(',') != -1)
                    onOffStr = onOffStr.substring(0, onOffStr.indexOf(','));
                on_off = (onOffStr == "1"); // 是否啟動（1為啟動）

                // 執行馬達控制
                if (on_off) {
                    controller.setMotor(motorIndex, dir, speed); // 控制第 motorIndex 顆馬達
                } else {
                    controller.setMotor(motorIndex, 0, 0);      // 停止該馬達
                }
                motorIndex++; // 處理下一顆馬達
            }
            // "fruit" 模式：用於水果夾持的馬達指令
            // Format: fruit,dir=-1,speed=100,on_off=True:
            else if (segment.startsWith("fruit")) {
                if (segment.indexOf("dir=") != -1) {
                    // 解析 DC 馬達指令
                    int dirIdx = segment.indexOf("dir=") + 4;
                    int speedIdx = segment.indexOf("speed=") + 6;
                    int onOffIdx = segment.indexOf("on_off=") + 7;

                    int dir = segment.substring(dirIdx, segment.indexOf(',', dirIdx)).toInt();
                    int speed = segment.substring(speedIdx, segment.indexOf(',', speedIdx)).toInt();

                    String onOffStr = segment.substring(onOffIdx);
                    if (onOffStr.indexOf(',') != -1)
                        onOffStr = onOffStr.substring(0, onOffStr.indexOf(','));
                    bool on_off = (onOffStr == "True" || onOffStr == "1");

                    controller.setMotor(dir, speed, on_off);  // 需於library中自訂
                } else {
                    // 指令直接轉發給另一塊板子（例如下層執行特殊動作）
                    Serial1.println(segment); // 注意下層端 Serial.begin(9600) 是否有打開
                }
            }
            // 處理平台伺服馬達指令
            // Format: platA_base,channel=3,initial=10,end=20,increment=1,ini_to_end=True:
            else if (segment.startsWith("platA_base") || segment.startsWith("platB_base")) {
                if (segment.indexOf("channel=") != -1) {
                    // 解析 channel
                    int channelIdx = segment.indexOf("channel=") + 8;
                    int channelCommaIdx = segment.indexOf(',', channelIdx);
                    int channel = segment.substring(channelIdx, channelCommaIdx).toInt();

                    // 解析 initial
                    int initialIdx = segment.indexOf("initial=") + 8;
                    int initialCommaIdx = segment.indexOf(',', initialIdx);
                    int initial = segment.substring(initialIdx, initialCommaIdx).toInt();

                    // 解析 end
                    int endIdx = segment.indexOf("end=") + 4;
                    int endCommaIdx = segment.indexOf(',', endIdx);
                    int end = segment.substring(endIdx, endCommaIdx).toInt();

                    // 解析 increment
                    int incrementIdx = segment.indexOf("increment=") + 10;
                    int incrementCommaIdx = segment.indexOf(',', incrementIdx);
                    int increment = segment.substring(incrementIdx, incrementCommaIdx).toInt();

                    // 解析 ini_to_end
                    int ini_to_endIdx = segment.indexOf("ini_to_end=") + 11;
                    int ini_to_endColonIdx = segment.indexOf(':', ini_to_endIdx);
                    String iniToEndStr = segment.substring(ini_to_endIdx, ini_to_endColonIdx);
                    bool ini_to_end = (iniToEndStr == "True" || iniToEndStr == "1");

                    ClimbServo.moveServo(channel, initial, end, increment, ini_to_end);
                }
                // Format: platB_base,dir=1,speed=70,on_off=1:
                else if (segment.indexOf("dir=") != -1) {
                    // 取得 dir 參數的位置，dir= 後面為數值
                    int dirIdx = segment.indexOf("dir=") + 4;  // 找到 dir= 字串後，定位到數值起始處
                    int dirCommaIdx = segment.indexOf(',', dirIdx); // 找下一個逗號（,）的位置，表示 dir 結束的位置
                    int dir = segment.substring(dirIdx, dirCommaIdx).toInt(); // 取出 dir 的數值，轉換為整數（1=正轉, -1=反轉, 0=停止）

                    // 取得 speed 參數的位置，speed= 後面為數值
                    int speedIdx = segment.indexOf("speed=") + 6;   // 找到 speed= 字串後，定位到數值起始處
                    int speedCommaIdx = segment.indexOf(',', speedIdx); // 找 speed= 之後第一個逗號（,）的位置
                    int speed = segment.substring(speedIdx, speedCommaIdx).toInt(); // 取出 speed 數值（0~255），轉換為整數

                    // 取得 on_off 參數的位置，on_off= 後面為數值
                    int onOffIdx = segment.indexOf("on_off=") + 7;  // 找到 on_off= 字串後，定位到數值起始處
                    int onOffColonIdx = segment.indexOf(':', onOffIdx); // 找到指令結尾的冒號（:）
                    String onOffStr = segment.substring(onOffIdx, onOffColonIdx); // 取出 on_off 的字串內容（"1"、"0"、"True"、"False"）

                    // 判斷是否啟動馬達（1 或 True 為啟動，0 或 False 為關閉）
                    bool on_off = (onOffStr == "1" || onOffStr == "True");

                    ClimbServo.setClimbMotor(dir, speed, on_off);
                }
            }
            // Format: coffee,channel=8,initial=10,end=20,reset=True\n
            else if (segment.startsWith("coffee")) {
                if (segment.indexOf("channel=") != -1){
                    // 解析 channel
                    int channelIdx = segment.indexOf("channel=") + 8;
                    int channelCommaIdx = segment.indexOf(',', channelIdx);
                    int channel = segment.substring(channelIdx, channelCommaIdx).toInt();

                    // 解析 initial
                    int initialIdx = segment.indexOf("initial=") + 8;
                    int initialCommaIdx = segment.indexOf(',', initialIdx);
                    int initial = segment.substring(initialIdx, initialCommaIdx).toInt();

                    // 解析 end
                    int endIdx = segment.indexOf("end=") + 4;
                    int endCommaIdx = segment.indexOf(',', endIdx);
                    int end = segment.substring(endIdx, endCommaIdx).toInt();


                }
            }
        }
    }
}