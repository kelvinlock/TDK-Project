import pygame
import serial
import time
import threading

pygame.init()

# 畫文本到屏幕的類
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

def monitor_limit_switch(arduinoA, arduinoB):
    while True:
        line = arduinoB.readline().decode(errors='ignore').strip()
        if "PlatformA_Bottom" in line:
            print("收到A底部，發送停止")
            arduinoA.write(b'stopA,rebound=500\n')
        if "PlatformB_Bottom" in line:
            print("收到B底部，發送停止")
            arduinoA.write(b'stopB,rebound=500\n')

def run_dc_motor(arduinoB, distance_cm, speed=70, motor_name="platB_base"):
    """
    控制定時驅動 DC 馬達跑指定距離（單位：公分）
    arduinoB   ：目標 Arduino（負責控制 DC 馬達）
    distance_cm：希望移動的距離（公分），可由隊友任意更改
    speed      ：馬達轉速（0~255），預設為70，可自行調整
    motor_name ：目標馬達的名稱（指令字串），預設為 platB_base
    """
    # ★根據實際實驗結果：speed=70 時，每 1 公分約需 0.05 秒（請依你們測量的數值調整）
    seconds_per_cm = 0.05
    duration = distance_cm * seconds_per_cm

    # 發送啟動指令，讓 DC 馬達往前
    arduinoB.write(f'{motor_name},dir=1,speed={speed},on_off=1:\n'.encode())
    time.sleep(duration)  # 馬達運行指定時間
    # 發送停止指令
    arduinoB.write(f'{motor_name},dir=0,speed=0,on_off=0:\n'.encode())

def init_climb(arduinoA, arduinoB):
    # 初始步驟：重置平臺位置，平台皆上升、展開支撐架
    arduinoA.write(b'platform_reset,speed=520,increase=0,force=1\n')
    time.sleep(1.0)
    arduinoA.write(b'platA,height=120,increase=1,speed=520,force=1:\n')  # 前平台上升
    arduinoA.write(b'platB,height=120,increase=1,speed=520,force=1:\n')  # 後平台上升
    time.sleep(1.0)
    arduinoB.write(b'platA_base,channel=3,initial=10,end=20,increment=1,ini_to_end=True:\n')   # Servo 展開支撐架
    arduinoB.write(b'platB_base,channel=14,initial=10,end=20,increment=1,ini_to_end=True:\n')
    arduinoB.write(b'platB_base,channel=15,initial=10,end=20,increment=1,ini_to_end=True:\n')
    time.sleep(1.0)

def climb_step(arduinoA, arduinoB):
    """
    機器人單階爬樓梯動作
    arduinoA        ：主控板A，負責平臺升降
    arduinoB        ：主控板B，負責驅動DC輪馬達
    dc_distance_cm  ：DC馬達推進距離（公分），可讓隊友隨時調整
    """
    # 同時讓前後兩個平台下降到底部（方便底盤準備推進）
    arduinoA.write(b'platform_reset,speed=520,increase=0,force=1\n')
    time.sleep(1.2)

    # 前平台先上升（避免前平台卡在階梯邊緣）
    arduinoA.write(b'platA,height=120,increase=1,speed=520,force=1:\n')
    time.sleep(1.2)

    # DC馬達推動底盤前進（依設定距離前進，例如30公分）
    run_dc_motor(arduinoB, distance_cm=30, speed=70, motor_name="platB_base")

    # 後平台上升（完成這一階的爬升後，恢復後平台高度）
    arduinoA.write(b'platB,height=120,increase=1,speed=520,force=1:\n')
    time.sleep(1.2)

def end_climb(arduinoB):
    # 收起支撐架或其他結束動作
    arduinoB.write(b'platA_base,channel=3,initial=10,end=20,increment=1,ini_to_end=False:\n') # Servo 收起
    arduinoB.write(b'platB_base,channel=14,initial=10,end=20,increment=1,ini_to_end=False:\n')
    arduinoB.write(b'platB_base,channel=15,initial=10,end=20,increment=1,ini_to_end=False:\n')
    time.sleep(0.8)

def main():
    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Joystick Axes Display")

    try:
        # 連接兩個Arduino，A為底盤，B為上層/外設
        arduinoA = serial.Serial(port="COM3", baudrate=9600, timeout=1)
        arduinoB = serial.Serial(port="COM4", baudrate=9600, timeout=0.1)
        threading.Thread(target=monitor_limit_switch, args=(arduinoA, arduinoB), daemon=True).start()
        time.sleep(2)  # 等待串口初始化
    except Exception as e:
        print(f"串口連接失敗: {e}")
        return

    arduinoA.write(b'platform_reset,speed=520,increase=0,force=1\n')
    clock = pygame.time.Clock()
    text_print = TextPrint()
    joysticks = {}

    done = False
    hat = (0, 0)
    ori_speed = 70
    climb_level = 0
    fruit_force_on = False      # 控制步進馬達激磁狀態
    orange_clamp_on = False     # 夾柳丁狀態
    bucket_grabbed = False      # 夾桶子狀態
    coffee_arm_open = False     # 咖啡手臂展開
    coffee_clamp_on = False     # 咖啡杯夾合
    coffee_table_low = False    # 咖啡桌高度低/高

    # 主循環
    while not done:

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True

            # 手柄插入，動態註冊
            elif event.type == pygame.JOYDEVICEADDED:
                joy = pygame.joystick.Joystick(event.device_index)
                joysticks[joy.get_instance_id()] = joy
                print(f"Joystick {joy.get_instance_id()} connected")

            # 手柄拔出，動態移除
            elif event.type == pygame.JOYDEVICEREMOVED:
                del joysticks[event.instance_id]
                print(f"Joystick {event.instance_id} disconnected")

            # 處理帽子（方向鍵/十字鍵）事件
            elif event.type == pygame.JOYHATMOTION:
                hat = event.value  # hat值是元組 (x, y)

            # 處理按鍵事件（主體功能）
            elif event.type == pygame.JOYBUTTONDOWN:
                # 平台升降，對應底盤動作
                if event.button == 0:  # A鍵
                    text_print.tprint(screen, "Platform B ↑")
                    arduinoA.write(b'platB,height=50,increase=1,speed=520,force=1:\n')
                elif event.button == 1:  # B鍵
                    text_print.tprint(screen, "Platform B ↓")
                    arduinoA.write(b'platB,height=50,increase=0,speed=520,force=1:\n')
                elif event.button == 2:  # X鍵
                    text_print.tprint(screen, "Platform A ↑")
                    arduinoA.write(b'platA,height=50,increase=1,speed=520,force=1:\n')
                elif event.button == 3:  # Y鍵
                    text_print.tprint(screen, "Platform A ↓")
                    arduinoA.write(b'platA,height=50,increase=0,speed=520,force=1:\n')
                elif event.button == 4:  # LB 減速
                    ori_speed -= 10
                elif event.button == 5:  # RB 加速
                    ori_speed += 10

                # 模式判斷：帽子方向 + 按鍵組合
                if hat == (0, 1):   # ↑ 娃娃機模式
                    if event.button == 0:   # A鍵，夾取
                        arduinoB.write(b'doll,Clamp:\n')
                    elif event.button == 1: # B鍵，釋放
                        arduinoB.write(b'doll,Release:\n')
                    elif event.button == 2: # X鍵，機械臂展開
                        arduinoB.write(b'doll,Expand:\n')
                    elif event.button == 3: # Y鍵，機械臂收回
                        arduinoB.write(b'doll,Retracts:\n')

                elif hat == (0, -1):    # ↓ 果樹採摘/平台升降模式
                    if event.button == 2:   # X鍵，低高度模式
                        fruit_force_on = not fruit_force_on
                        force_value = 1 if fruit_force_on else 0
                        arduinoB.write(f'fruit,height=50,increase={force_value},speed=520,force={force_value}:\n'.encode())
                    elif event.button == 3: # Y鍵，中高度模式
                        fruit_force_on = not fruit_force_on
                        force_value = 1 if fruit_force_on else 0
                        arduinoB.write(f'fruit,height=90,increase={force_value},speed=520,force={force_value}:\n'.encode())
                    elif event.button == 1: # B鍵，高高度模式
                        fruit_force_on = not fruit_force_on
                        force_value = 1 if fruit_force_on else 0
                        arduinoB.write(f'fruit,height=120,increase={force_value},speed=520,force={force_value}:\n'.encode())
                    elif event.button == 0:  # A鍵，柳丁夾取/釋放狀態切換
                        orange_clamp_on = not orange_clamp_on
                        if orange_clamp_on:
                            arduinoB.write(b'fruit,dir=1,speed=100,on_off=True:\n')    # 執行夾取
                        else:
                            arduinoB.write(b'fruit,dir=-1,speed=100,on_off=True:\n')   # 執行釋放

                elif hat == (-1, 0):    # ← 桶子/攀爬模式
                    if event.button == 0:   # A鍵，桶子夾取/放下
                        bucket_grabbed = not bucket_grabbed
                        data = f"bucket,{bucket_grabbed}:\n"
                        arduinoB.write(data.encode())
                    elif event.button == 1: # B鍵，高台娃娃放置
                        arduinoB.write(b'ladder,place:\n')
                    elif event.button == 2: # X鍵，桶子傾倒
                        arduinoB.write(b'bucket,titl:\n')
                    elif event.button == 3: # Y鍵，爬高台樓梯 Servo, Stepper, DC
                        # 展開->平臺下降->DC(前後)往前推->前面平臺上升->DC(后面)往前推->後面平臺上升->收起來
                        if climb_level == 0:
                            # 第一次按下，初始化＋爬一階
                            init_climb(arduinoA, arduinoB)
                            climb_step(arduinoA, arduinoB)
                            climb_level += 1
                        elif climb_level == 1:
                            # 第二次按下，只爬一階＋結束
                            climb_step(arduinoA, arduinoB)
                            end_climb(arduinoB)
                            climb_level = 0  # 完成兩階爬樓梯，重設

                elif hat == (1, 0):     # → 咖啡模式
                    if event.button == 0:  # A鍵，咖啡手臂展開/收回
                        coffee_arm_open = not coffee_arm_open
                        data = f"coffee_arm,{coffee_arm_open}:\n"
                        arduinoB.write(data.encode())
                    elif event.button == 1:  # B鍵，咖啡杯夾合/放
                        coffee_clamp_on = not coffee_clamp_on
                        data = f"coffee_clamp,{coffee_clamp_on}:\n"
                        arduinoB.write(data.encode())
                    elif event.button == 2:  # X鍵，咖啡盤吸附/放開
                        arduinoB.write(b'coffee,channel=8,initial=10,end=20,increment=1,reset=True:\n') # reset 代表轉到指定位置會回到原點
                        time.sleep(1)
                    elif event.button == 3:  # Y鍵，咖啡桌高度低/高
                        coffee_table_low = not coffee_table_low
                        data = f"coffee_table_height,{coffee_table_low}:\n"
                        arduinoB.write(data.encode())

        # 畫面每次都清空重畫，顯示當前搖桿資訊與發送封包
        screen.fill((255, 255, 255))
        text_print.reset()

        joystick_count = pygame.joystick.get_count()
        text_print.tprint(screen, f"Number of joysticks: {joystick_count}")
        text_print.indent()

        # 依次處理所有已連接搖桿
        for joystick in joysticks.values():
            jid = joystick.get_instance_id()
            text_print.tprint(screen, f"Joystick {jid}")
            text_print.indent()

            name = joystick.get_name()
            text_print.tprint(screen, f"Joystick name: {name}")

            guid = joystick.get_guid()
            text_print.tprint(screen, f"GUID: {guid}")

            power_level = joystick.get_power_level()
            text_print.tprint(screen, f"Joystick's power level: {power_level}")

            axes = joystick.get_numaxes()
            text_print.tprint(screen, f"Number of axes: {axes}")
            text_print.indent()

            try:
                # 設定死區，防止誤動作
                deadzone = 0.1
                # 左搖桿 X、Y
                x_axis = joystick.get_axis(0)
                y_axis = -joystick.get_axis(1)
                # 右搖桿 X
                right_x = joystick.get_axis(2)

                # 四輪輪速計算
                wheel_speeds = [
                    y_axis + x_axis + right_x,  # 左前
                    y_axis - x_axis - right_x,  # 右前
                    y_axis - x_axis + right_x,  # 左後
                    y_axis + x_axis - right_x   # 右後
                ]

                data_packet = ""
                for speed in wheel_speeds:
                    if abs(speed) < deadzone:
                        direction = 0
                    elif speed > 0:
                        direction = 1
                    else:
                        direction = -1
                    data_packet += f"move,dir={direction},speed={int(abs(speed) * ori_speed)},on_off=1:"

                Send_str = f"{data_packet}\n"
                text_print.tprint(screen, Send_str)

                # 傳送封包到 ArduinoB
                if arduinoB and arduinoB.is_open:
                    arduinoB.write(Send_str.encode())

                # LT/RT 觸發器模擬單輪旋轉
                lt_value = joystick.get_axis(4)
                rt_value = joystick.get_axis(5)
                lt_speed = int(max(0, lt_value) * 255)
                rt_speed = int(max(0, rt_value) * 255)

                turn_packet = ""
                if lt_speed > 0:
                    turn_packet = (
                        f"turn,dir=1,speed={lt_speed},on_off=True:"
                        f"turn,dir=0,speed=0,on_off=True:"
                        f"turn,dir=0,speed=0,on_off=True:"
                        f"turn,dir=0,speed=0,on_off=True:"
                    )
                elif rt_speed > 0:
                    turn_packet = (
                        f"turn,dir=0,speed=0,on_off=True:"
                        f"turn,dir=1,speed={lt_speed},on_off=True:"
                        f"turn,dir=0,speed=0,on_off=True:"
                        f"turn,dir=0,speed=0,on_off=True:"
                    )

                if turn_packet:
                    if arduinoB and arduinoB.is_open:
                        arduinoB.write((turn_packet + "\n").encode())

            except pygame.error:
                text_print.tprint(screen, "錯誤: 無法讀取搖桿軸值")

        pygame.display.flip()
        clock.tick(30)

    # 程式結束時關閉串口
    if (arduinoA and arduinoA.is_open) or (arduinoB and arduinoB.is_open):
        arduinoA.close()
        arduinoB.close()
    pygame.quit()

if __name__ == "__main__":
    main()
