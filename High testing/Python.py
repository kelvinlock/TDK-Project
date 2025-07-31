import serial
import time

# 請依照你實際的 COM Port 修改！
PORT_A = 'COM5'  # Board A 步進馬達
PORT_B = 'COM6'  # Board B 極限開關

ser_a = serial.Serial(PORT_A, 9600, timeout=0.1)
ser_b = serial.Serial(PORT_B, 9600, timeout=0.1)

ser_a.write(b'S')  # 讓馬達開始動作
print("步進馬達開始運轉...")

step_count = 0

while True:
    # 監聽Board B 極限開關訊號
    line = ser_b.readline().decode(errors='ignore').strip()
    if line == "LimitSwitchTriggered":
        print("偵測到極限開關觸發！發送停止訊號給步進馬達...")
        ser_a.write(b'X')  # 通知A停下來
        break

    # 監聽Board A 步數回報
    line_a = ser_a.readline().decode(errors='ignore').strip()
    if line_a.startswith("StepCount="):
        step_count = int(line_a.split('=')[1])
        print(f"目前步數：{step_count}")

    time.sleep(0.01)  # 降低CPU負擔

print(f"測量完成，總步數為：{step_count}")
ser_a.close()
ser_b.close()
