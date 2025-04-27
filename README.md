# TDK-Project
*Created by [Kelvin](https://github.com/kelvinlock/TDK-Project) - November 2023*
### **Summary of Taiwan TDK Cup National University Robotics Competition**  

The **Taiwan TDK Cup National University Creative Design and Robotics Competition** is one of Taiwan’s longest-running and largest robotics contests, co-organized by the Ministry of Education and the TDK Foundation since 1997.  

- **Objective**: To cultivate students' innovation, design, and hands-on integration skills in robotics.  
- **Competition Categories**:  
  - **Remote Control Group**: Manual operation tasks (e.g., obstacle crossing, item transport).  
  - **Autonomous Group**: AI-driven robots with vision recognition and path planning.  
  - **Flying Group** (occasional): UAV challenges like tunnel navigation and precision drops.  
- **Themes & Challenges**: Tasks are inspired by local culture or global issues (e.g., environmental protection, regional-themed missions like "Yunlin Hero Challenge" in 2025).  
- **Prizes**: Winners receive up to **NT$200,000**, with opportunities for international exchanges (e.g., visits to Japan’s NHK Robocon).  
- **Educational Impact**: Emphasizes practical skills in automation, AI, and mechanical design, aligning with industry needs.  
- **Recent Updates**: Hybrid/online formats were adopted during the pandemic, and some categories (e.g., Flying Group) may vary by year.  
For details, visit the [official website](https://web02.yuntech.edu.tw/~tdk_4hhoerjt/) or follow "Taiwan TDK Robocon" on Facebook.
---
# Xbox Controller Serial Communication with Arduino

This project demonstrates how to use an Xbox controller to send serial commands to an Arduino Mega 2560, controlling an LED connected to pin 13.

## Features
- Detect Xbox controller button presses using Pygame
- Send serial commands via PySerial when Button 0 (A button) is pressed/released
- Arduino Mega 2560 receives commands and controls an LED on pin 13
- Real-time display of controller status and button states

## Requirements
### Hardware
- Xbox controller (wired or wireless with USB adapter)
- Arduino Mega 2560 (or compatible board)
- LED with resistor (for pin 13)
- USB cable for Arduino connection

### Software
- Python 3.x
- Pygame library (`pip install pygame`)
- PySerial library (`pip install pyserial`)
- Arduino IDE

## Setup Instructions

1. **Arduino Setup**:
   - Connect an LED to pin 13 (with appropriate resistor)
   - Upload the following code to your Arduino:
     ```arduino
     void setup() {
       pinMode(13, OUTPUT);
       Serial.begin(9600);
     }
     
     void loop() {
       if (Serial.available() > 0) {
         char command = Serial.read();
         if (command == '1') {
           digitalWrite(13, HIGH);
         } else if (command == '0') {
           digitalWrite(13, LOW);
         }
       }
     }
     ```

2. **Python Setup**:
   - Install required libraries:
     ```
     pip install pygame pyserial
     ```
   - Note your Arduino's COM port (check in Arduino IDE or Device Manager)

3. **Running the Program**:
   - Modify the COM port in the Python script (line `arduino = serial.Serial('COM3', 9600, timeout=1)`)
   - Run the Python script:
     ```
     python controller_serial.py
     ```

## How It Works
- When Button 0 (typically the A button) is pressed:
  - Python sends '1' via serial
  - Arduino turns on the LED
- When the button is released:
  - Python sends '0' via serial
  - Arduino turns off the LED

The program also displays:
- Connected joystick information
- Button states
- Real-time LED status

## Troubleshooting
- **Controller not detected**: Ensure it's properly connected before running the script
- **Serial connection issues**:
  - Verify correct COM port
  - Check no other program is using the serial port
  - Ensure matching baud rate (9600) on both Arduino and Python
- **LED not responding**:
  - Verify correct pin connection
  - Check serial communication with Arduino Serial Monitor

## License
This project is open-source under the MIT License.

---

# Xbox Controller Motor Control via Arduino

This project enables precise motor control using an Xbox controller's analog sticks. The system:
1. Reads analog stick positions via Pygame
2. Maps axis values to PWM signals
3. Transmits PWM data via serial to Arduino
4. Controls a motor driver to drive DC motors

## Features
- 🎮 Xbox controller analog stick input with precise axis reading
- 🔄 Real-time axis-to-PWM mapping with deadzone handling
- 📡 Serial communication between Python and Arduino
- ⚙️ Motor direction and speed control via PWM
- 🖥️ Real-time GUI displaying controller data

## Hardware Requirements
- Xbox One/360 controller (wired or wireless with adapter)
- Arduino Mega 2560 (or compatible board)
- Motor driver (L298N or similar)
- DC motor
- Power supply for motor
- USB cables
- Jumper wires

## Software Requirements
- Python 3.8+
- Pygame (`pip install pygame`)
- PySerial (`pip install pyserial`)
- Arduino IDE

## Installation & Setup

### 1. Hardware Connections
```plaintext
Arduino Pin | Motor Driver
-----------|-------------
9          | ENA (PWM)
3          | IN1
4          | IN2
GND        | GND

### 2. Arduino Setup
Upload this code to your Arduino:
```arduino
const int IN1 = 3;
const int IN2 = 4;
const int enablePin = 9;

void setup() {
  Serial.begin(9600);
  pinMode(enablePin, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    if (data.startsWith("PWM:")) {
      int speed = data.substring(4).toInt();
      speed = constrain(speed, 0, 255);

      // Motor control logic with deadzone
      if (speed >= 132) {         // Forward
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(enablePin, speed);
      } else if (speed <= 122) {  // Reverse
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(enablePin, 255 - speed);
      } else {                    // Stop
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        analogWrite(enablePin, 0);
      }
    }
  }
}
```

### 3. Python Setup
1. Install dependencies:
```bash
pip install pygame pyserial
```
2. Modify the COM port in the Python script:
```python
arduino = serial.Serial("COM6", 9600, timeout=1)  # Change COM6 to your port
```

## Usage
1. Connect your Xbox controller
2. Run the Python script:
```bash
python controller_motor.py
```
3. GUI will show:
   - Controller connection status
   - Raw axis values
   - Mapped PWM values
4. Move the left analog stick up/down to control motor speed and direction

## Technical Details

### Axis-to-PWM Mapping
```python
def map_axis_to_pwm(axis_value):
    """Maps joystick axis (-1.0 to 1.0) to PWM (0-255)"""
    pwm = int(axis_value * 127 + 127)
    if abs(pwm - 127) < 2:  # Deadzone
        return 127
    return pwm
```

### Motor Control Logic
| PWM Range | Motor Action       |
|-----------|--------------------|
| 132-255   | Forward            |
| 122-0     | Reverse            |
| 123-131   | Stop (deadzone)    |

### PWM Mapping Table
| Axis Value | PWM Value | Motor Action |
|------------|-----------|--------------|
| 1.0        | 254       | Full forward |
| 0.0        | 127       | Stop         |
| -1.0       | 0         | Full reverse |

## Troubleshooting
- **Controller not detected**: Ensure it's connected before running the script
- **Serial errors**: Verify correct COM port and baud rate (9600)
- **Motor not responding**: Check power supply and motor driver connections
- **Erratic behavior**: Ensure proper grounding between all components

## License
MIT License - Free for personal and educational use

---

*Project by [Kelvin] | Last updated: April 2025*
```
