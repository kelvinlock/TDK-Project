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
```
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
- **Controller isn't detected**: Ensure it's connected before running the script
- **Serial errors**: Verify correct COM port and baud rate (9600)
- **Motor not responding**: Check power supply and motor driver connections
- **Erratic behavior**: Ensure proper grounding between all components

## License
MIT License — Free for personal and educational use
