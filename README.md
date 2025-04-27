# TDK-Project
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

*Last edited by Creator on April 27, 2025*
