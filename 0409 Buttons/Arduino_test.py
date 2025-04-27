import serial
import time

arduino = serial.Serial('COM6', 9600, timeout=1)  # Adjust port

while True:
    arduino.write(b'1')  # Turn LED ON
    time.sleep(1)
    arduino.write(b'0')  # Turn LED OFF
    time.sleep(1)