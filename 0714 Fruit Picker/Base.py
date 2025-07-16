import pygame
import serial
import time

pygame.init()

# 画文本到屏幕的类
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

def main():
    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Joystick Axes Display")

    try:
        # 这里连接两个Arduino串口。A负责底盘DC马达，B负责爬楼梯单元
        arduinoA = serial.Serial(port="COM3", baudrate=9600, timeout=1)
        arduinoB = serial.Serial(port="COM4", baudrate=9600, timeout=1)
        time.sleep(2)  # 等待串口初始化完成
    except Exception as e:
        print(f"串口連接失敗: {e}")  # 如果串口无法打开，直接报错并退出
        return

    clock = pygame.time.Clock()
    text_print = TextPrint()
    joysticks = {}  # 保存已连接的手柄

    done = False
    hat = (0, 0)
    bucket_grabbed = False      # False=放開, True=抓住
    coffee_arm_open = False     # 咖啡手臂展開/收回狀態
    coffee_clamp_on = False     # 咖啡杯夾夾/放
    coffee_suction_on = False   # 咖啡盤吸附/放開
    coffee_table_low = False    # 咖啡桌高度低/高

    # 主循环
    while not done:

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True

            # 手柄插入时，注册并加入joysticks字典
            elif event.type == pygame.JOYDEVICEADDED:
                joy = pygame.joystick.Joystick(event.device_index)
                joysticks[joy.get_instance_id()] = joy
                print(f"Joystick {joy.get_instance_id()} connected")

            # 手柄拔出时，从joysticks字典移除
            elif event.type == pygame.JOYDEVICEREMOVED:
                del joysticks[event.instance_id]
                print(f"Joystick {event.instance_id} disconnected")

            # 帽子开关事件（通常是方向盘）
            elif event.type == pygame.JOYHATMOTION:
                hat = event.value  # hat的值是个tuple，比如(0,1)

            # 按钮按下事件（用于平台升降）
            elif event.type == pygame.JOYBUTTONDOWN:
                if event.button == 0:  # A键，平台B上升
                    text_print.tprint(screen, "Platform B ↑")
                    arduinoA.write(b'platB:height,50:lifting,1:speed,520:force,1:\n')
                elif event.button == 1:  # B键，平台B下降
                    text_print.tprint(screen, "Platform B ↓")
                    arduinoA.write(b'platB:height,50:lifting,0:speed,520:force,1:\n')
                elif event.button == 2:  # X键，平台A上升
                    text_print.tprint(screen, "Platform A ↑")
                    arduinoA.write(b'platA:height,50:lifting,1:speed,520:force,1:\n')
                elif event.button == 3:  # Y键，平台A下降
                    text_print.tprint(screen, "Platform A ↓")
                    arduinoA.write(b'platA:height,50:lifting,0:speed,520:force,1:\n')
                elif event.button == 4:  # LB (Left Bumper) 減速
                    arduinoB.write(b'accelerate:speed,\n')
                elif event.button == 5:  # RB (Right Bumper) 加速
                    arduinoB.write(b'decelerate:speed,\n')

                # 根據帽子 + 按鍵組合判斷
                if hat == (0, 1):   # ↑ doll mode
                    if event.button == 0:   # A键，夾取
                        arduinoB.write(b'doll:Clamp\n')
                    elif event.button == 1: # A键，釋放
                        arduinoB.write(b'doll:Release\n')
                    elif event.button == 2: # X键，機械臂：展開
                        arduinoB.write(b'doll:Expand\n')
                    elif event.button == 3: # Y键，機械臂：收回
                        arduinoB.write(b'doll:Retracts\n')
                elif hat == (0, -1):    # ↓ fruit mode
                    if event.button == 2:   # X键，高度"低"模式 "60cm"
                        arduinoB.write(b'fruit,height=50,increase=1,speed=520,force=1:\n')   # 步進馬達 stepper motor
                    elif event.button == 3: # Y键，高度"中"模式 "100cm"
                        arduinoB.write(b'fruit:height,90:increase=1,speed=520,force=1:\n')   # 步進馬達 stepper motor
                    elif event.button == 1: # B键，高度"高"模式 "130cm"
                        arduinoB.write(b'fruit:height,120:increase,1:speed,520:force,1:\n')  # 步進馬達 stepper motor
                    elif event.button == 0: # A键，夾取柳丁
                        arduinoB.write(b'fruit:dir,1:speed,100:on_off,True:\n')  # 直流馬達 DC motor
                elif hat == (-1, 0):    # ← bucket & climb mode
                    if event.button == 0:   # A键，夾取桶子抓/放
                        bucket_grabbed = not bucket_grabbed  # 切換狀態
                        data = f"bucket:{bucket_grabbed}\n"
                        arduinoB.write(data.encode())
                    elif event.button == 1: # B键，高台娃娃放置
                        arduinoB.write(b'ladder:place\n')
                    elif event.button == 2: # X键，桶子夾夾倒/豎
                        arduinoB.write(b'bucket:titl\n')
                    elif event.button == 3: # Y键，爬高台樓梯一鍵爬完
                        arduinoB.write(b'ladder:climb:\n')
                elif hat == (1, 0):     # → Coffee mode
                    if event.button == 0:  # A键，咖啡手臂展開/收回
                        coffee_arm_open = not coffee_arm_open       # 切換 True/False
                        data = f"coffee_arm:{coffee_arm_open}\n"
                        arduinoB.write(data.encode())
                    elif event.button == 1:  # B键，咖啡杯夾夾/放
                        coffee_clamp_on = not coffee_clamp_on       # 切換 True/False
                        data = f"coffee_clamp:{coffee_clamp_on}\n"
                        arduinoB.write(data.encode())
                    elif event.button == 2:  # X键，咖啡盤吸附/放開
                        coffee_suction_on = not coffee_suction_on   # 切換 True/False
                        data = f"coffee_suction:{coffee_suction_on}\n"
                        arduinoB.write(data.encode())
                    elif event.button == 3:  # Y键，咖啡桌高度低/高
                        coffee_table_low = not coffee_table_low     # 切換 True/False
                        data = f"coffee_table_height:{coffee_table_low}\n"
                        arduinoB.write(data.encode())

        # 屏幕绘制部分，每次都清空重画
        screen.fill((255, 255, 255))
        text_print.reset()

        joystick_count = pygame.joystick.get_count()
        text_print.tprint(screen, f"Number of joysticks: {joystick_count}")
        text_print.indent()

        # 遍历所有手柄，获取轴数据
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

            # 取得左搖桿X、Y轴数据
            try:
                # 死區設定，防止微小搖動產生誤動作
                deadzone = 0.1
                # 取得左搖桿 X/Y 值
                x_axis = joystick.get_axis(0)  # 左搖桿 X軸（左右）
                y_axis = -joystick.get_axis(1)  # 左搖桿 Y軸（上下，取負數是符合直觀“上推為正”）

                # 計算四個輪子對應的移動量（方向+速度，理論上可以斜移、橫移）
                wheel_speeds = [
                    y_axis + x_axis,  # 左前輪
                    y_axis - x_axis,  # 右前輪
                    y_axis - x_axis,  # 左後輪
                    y_axis + x_axis  # 右後輪
                ]

                data_packet = ""
                for speed in wheel_speeds:
                    if abs(speed) < deadzone:
                        direction = 0  # 在死區內，停車
                    elif speed > 0:
                        direction = 1  # 前進
                    else:
                        direction = -1  # 後退
                    # 組合通訊封包格式（每個輪子一筆指令）
                    data_packet += f"move,dir={direction},speed=70,on_off=1:"

                Send_str = f"{data_packet}\n"  # 加上換行，代表結束

                # 畫面上顯示目前送出的封包（方便Debug/追蹤訊號）
                text_print.tprint(screen, Send_str)

                # 傳送資料到 ArduinoB 控制底盤（四輪皆獨立送方向/速度）
                if arduinoB and arduinoB.is_open:
                    arduinoB.write(Send_str.encode())

                # 取得右搖桿 X/Y
                right_x = joystick.get_axis(2)
                right_y = -joystick.get_axis(3)

                # 取得 LT（左下板機），RT（右下板機）數值
                lt_value = joystick.get_axis(4)  # LT，代表「左擺動」
                rt_value = joystick.get_axis(5)  # RT，代表「右擺動」

                # 設定死區與最大轉速（將類比值轉成 0~255 的PWM轉速）
                lt_speed = int(max(0, lt_value) * 255)
                rt_speed = int(max(0, rt_value) * 255)

                # 組合turn的封包（以四輪輪流檢查，目前只用第一輪有效，其餘作佯裝資料）
                turn_packet = ""
                if lt_speed > 0:
                    # 只對第一顆輪子做左轉
                    turn_packet = (
                        f"turn:dir,1:speed,{lt_speed}:on_off,True:"  # 1=左轉
                        f"turn:dir,0:speed,0:on_off,True:"
                        f"turn:dir,0:speed,0:on_off,True:"
                        f"turn:dir,0:speed,0:on_off,True:"
                    )
                elif rt_speed > 0:
                    # 只對第二顆輪子做右轉
                    turn_packet = (
                        f"turn:dir,0:speed,0:on_off,True:"
                        f"turn:dir,1:speed,{rt_speed}:on_off,True:"  # 1=右轉
                        f"turn:dir,0:speed,0:on_off,True:"
                        f"turn:dir,0:speed,0:on_off,True:"
                    )

                # 只要有 turn_packet 就傳送（代表有按下LT或RT）
                if turn_packet:
                    if arduinoB and arduinoB.is_open:
                        arduinoB.write((turn_packet + "\n").encode())

            except pygame.error:
                text_print.tprint(screen, "錯誤: 無法讀取搖桿軸值")

        pygame.display.flip()
        clock.tick(30)  # 刷新率限制

    # 程序退出前关闭串口
    if arduinoA and arduinoA.is_open or arduinoB and arduinoB.is_open:
        arduinoA.close()
        arduinoB.close()
    pygame.quit()

if __name__ == "__main__":
    main()
