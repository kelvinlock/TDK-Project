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
- **Controller isn't detected**: Ensure it's properly connected before running the script
- **Serial connection issues**:
  - Verify correct COM port
  - Check no other program is using the serial port
  - Ensure matching baud rate (9600) on both Arduino and Python
- **LED not responding**:
  - Verify the correct pin connection
  - Check serial communication with Arduino Serial Monitor

## License
This project is open-source under the MIT License.
