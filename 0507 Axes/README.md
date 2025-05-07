# Xbox Controller Mecanum Wheel Motor Control

![Mecanum Wheel Robot](https://via.placeholder.com/600x300?text=Mecanum+Wheel+Robot+Diagram)

A complete system for controlling a mecanum wheel robot using an Xbox controller, Python, and Arduino.

## Features
- 🎮 Xbox controller input for intuitive omnidirectional control
- 📊 Real-time axis-to-PWM conversion with deadzone handling
- 🤖 Mecanum wheel kinematics calculations for 4-wheel control
- 📡 Serial communication between Python and Arduino
- 🚀 Individual motor control with direction and speed
- 📺 Real-time GUI displaying controller inputs and motor outputs

## Hardware Requirements
| Component | Quantity | Notes |
|-----------|----------|-------|
| Xbox Controller | 1 | Wired or wireless with adapter |
| Arduino Mega 2560 | 1 | Or compatible board |
| Mecanum Wheels | 4 | With matching motors |
| Motor Driver (L298N) | 2 | Or equivalent dual H-bridge |
| 12V Power Supply | 1 | For motor power |
| Jumper Wires | 20+ | Male-to-male |

## Wiring Diagram
```plaintext
Motor   Arduino Pins
-----   ------------
Left Front:
  DIR1 → 2
  DIR2 → 3
  PWM  → 5

Right Front:
  DIR1 → 4
  DIR2 → 5
  PWM  → 6

Left Rear:
  DIR1 → 7
  DIR2 → 8
  PWM  → 9

Right Rear:
  DIR1 → 9
  DIR2 → 10
  PWM  → 10
```
---
## Software Setup
### Python Requirements
```bash
pip install pygame pyserial
```
### Python Script (controller_mecanum.py)
```python
import pygame
import serial
import time

pygame.init()


class TextPrint:
    def __init__(self):
        self.reset()
        self.font = pygame.font.Font(None, 25)

    def tprint(self, screen, text):
        text_bitmap = self.font.render(text, True, (0, 0, 0))
        screen.blit(text_bitmap, (self.x, self.y))
        self.y += self.line_height

    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 15

    def indent(self):
        self.x += 10

    def unindent(self):
        self.x -= 10


def map_axis_to_speed(axis_value, deadzone=0.1):
    """將搖桿值(-1.0~1.0)映射到-255~255，加入死區處理"""
    if abs(axis_value) < deadzone:
        return 0
    return int(axis_value * 255)


def calculate_mecanum_speeds(x, y):
    """計算麥克納姆輪四輪速度"""
    # 麥克納姆輪運動學公式
    wheel_speeds = [
        y + x,  # 左前輪 (A)
        y - x,  # 右前輪 (B)
        y - x,  # 左後輪 (C)
        y + x  # 右後輪 (D)
    ]

    # 歸一化處理
    max_speed = max(abs(s) for s in wheel_speeds)
    if max_speed > 255:
        wheel_speeds = [int(s * 255 / max_speed) for s in wheel_speeds]

    return wheel_speeds


def main():
    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Joystick Axes Display")

    try:
        arduino = serial.Serial("COM6", 9600, timeout=1)
        time.sleep(2)  # 等待串口初始化
    except Exception as e:
        print(f"串口連接失敗: {e}")
        return

    clock = pygame.time.Clock()
    text_print = TextPrint()
    joysticks = {}
    done = False

    # 主要迴圈
    while not done:
        # 事件處理
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True
            elif event.type == pygame.JOYDEVICEADDED:
                joy = pygame.joystick.Joystick(event.device_index)
                joysticks[joy.get_instance_id()] = joy
                print(f"Joystick {joy.get_instance_id()} connected")
            elif event.type == pygame.JOYDEVICEREMOVED:
                del joysticks[event.instance_id]
                print(f"Joystick {event.instance_id} disconnected")

        # Drawing step
        screen.fill((255, 255, 255))
        text_print.reset()

        joystick_count = pygame.joystick.get_count()
        text_print.tprint(screen, f"Number of joysticks: {joystick_count}")
        text_print.indent()

        # For each joystick:
        for joystick in joysticks.values():
            jid = joystick.get_instance_id()
            text_print.tprint(screen, f"Joystick {jid}")
            text_print.indent()

            # Get the name from the OS for the controller/joystick.
            name = joystick.get_name()
            text_print.tprint(screen, f"Joystick name: {name}")

            guid = joystick.get_guid()
            text_print.tprint(screen, f"GUID: {guid}")

            power_level = joystick.get_power_level()
            text_print.tprint(screen, f"Joystick's power level: {power_level}")

            # Only show axes information
            axes = joystick.get_numaxes()
            text_print.tprint(screen, f"Number of axes: {axes}")
            text_print.indent()

            # 取得左搖桿Y軸值（Xbox手柄通常為axis 1）
            try:
                # 獲取搖桿值 (X軸和Y軸)
                x_axis = joystick.get_axis(0)  # 左搖桿X軸
                y_axis = -joystick.get_axis(1)  # 左搖桿Y軸 (反向)

                # 計算速度
                x_speed = map_axis_to_speed(x_axis)
                y_speed = map_axis_to_speed(y_axis)
                wheel_speeds = calculate_mecanum_speeds(x_speed, y_speed)

                # 顯示數據
                text_print.tprint(screen, f"X: {x_axis:.2f} -> {x_speed}")
                text_print.tprint(screen, f"PWM value: {x_speed}")
                text_print.tprint(screen, f"Y: {y_axis:.2f} -> {y_speed}")
                text_print.tprint(screen, f"PWM value: {y_speed}")

                for i, speed in enumerate(wheel_speeds):
                    text_print.tprint(screen, f"Wheel {i}: {speed}")

                # 發送數據到Arduino (格式: A_DIR,A_SPD:B_DIR,B_SPD:...)
                if arduino and arduino.is_open:
                    data_packet = ""
                    for speed in wheel_speeds:
                        direction = 1 if speed >= 0 else 0
                        data_packet += f"{direction},{abs(speed)}:"
                    try:
                        arduino.write(data_packet[:-1].encode() + b'\n')
                    except serial.SerialException as e:
                        print(f"Send error: {e}")
            except pygame.error:
                text_print.tprint(screen, "錯誤: 無法讀取搖桿軸值")

        pygame.display.flip()
        clock.tick(30)  # 控制更新速率

    if arduino and arduino.is_open:
        arduino.close()
    pygame.quit()


if __name__ == "__main__":
    main()
```
### Arduino Code
```arduino
struct Motor {
    int forwardPin;
    int backwardPin;
    int speedPin;
};

Motor motors[4] = {
    {2, 3, 6},   // 左前輪 (A)
    {4, 5, 6},   // 右前輪 (B)
    {7, 9, 6},   // 左後輪 (C)
    {8, 10, 6}   // 右後輪 (D)
};

void setup() {
    Serial.begin(9600);
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].forwardPin, OUTPUT);
        pinMode(motors[i].backwardPin, OUTPUT);
        pinMode(motors[i].speedPin, OUTPUT);
    }
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processMotorData(data);
    }
}

void processMotorData(String data) {
    String wheelData[4];
    int index = 0;

    // 解析數據 (格式: "1,200:0,150:1,100:0,255")
    while (data.length() > 0 && index < 4) {
        int colonPos = data.indexOf(':');
        if (colonPos == -1) {
            wheelData[index++] = data;
            data = "";
        } else {
            wheelData[index++] = data.substring(0, colonPos);
            data = data.substring(colonPos+1);
        }
    }

    // 控制每個馬達
    for (int i = 0; i < 4; i++) {
        int commaPos = wheelData[i].indexOf(',');
        if (commaPos != -1) {
            int dir = wheelData[i].substring(0, commaPos).toInt();
            int speed = wheelData[i].substring(commaPos+1).toInt();

            digitalWrite(motors[i].forwardPin, dir);
            digitalWrite(motors[i].backwardPin, !dir);
            analogWrite(motors[i].speedPin, speed);
        }
    }
}
```
## Installation Guide
1. Hardware Assembly
   * Connect all motors to the motor drivers
   * Wire motor drivers to Arduino as shown above
   * Connect power supply to motor drivers
2. Software Setup
   * Upload Arduino code to your board
   * Install Python dependencies
   * Modify COM port in a Python script if needed
3. Running the System
```bash
python controller_mecanum.py
```
## Control Scheme
| Controller Input | Robot Movement |
|------------------|----------------|
| Left Stick Up    | Forward        |
| Left Stick Down  | Backward       |
| Left Stick Left  | Strafe Left    |
| Left Stick Right | Strafe Right   |
| Diagonal Inputs  | Diagonal Move  |
| Stick Release    | Stop           |

## Mecanum-Wheel Kinematics
The system uses these formulas to calculate individual wheel speeds:
```
Wheel Speeds:
  Left Front  = Y + X
  Right Front = Y - X
  Left Rear   = Y - X
  Right Rear  = Y + X
```
Where:
- X = normalized x-axis input (-255 to 255)
- Y = normalized y-axis input (-255 to 255)

## Troubleshooting
| Issue | Solution |
|-------|----------|
| Motors not responding | Check power connections and serial port |
| Uneven movement | Verify wheel orientation and motor wiring |
| Jerky motion | Adjust deadzone value in Python script |
| Serial errors | Ensure correct baud rate (9600) |

## License
MIT License—Free for modification and commercial use
