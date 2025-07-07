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

def main():
    global Servo
    Servo = "reset,0"

    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Joystick Axes Display")

    try:
        arduinoA = serial.Serial(port="COM3", baudrate=9600, timeout=1)
        arduinoB = serial.Serial(port="COM4", baudrate=9600, timeout=1)
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
            # 按钮事件 / Button events
            elif event.type == pygame.JOYBUTTONDOWN:
                # Platform A: X (2) increase, Y (3) decrease
                if event.button == 2:  # X
                    text_print.tprint(screen, "Platform A ↑")
                    arduinoB.write(b'platA,1\n')
                elif event.button == 3:  # Y
                    text_print.tprint(screen, "Platform A ↓")
                    arduinoB.write(b'platA,0\n')
                    # Platform B: A (0) increase, B (1) decrease
                elif event.button == 0:  # A
                    text_print.tprint(screen, "Platform B ↑")
                    arduinoB.write(b'platB,1\n')
                elif event.button == 1:  # B
                    text_print.tprint(screen, "Platform B ↓")
                    arduinoB.write(b'platB,0\n')
            # 帽子事件 / Hat events
            elif event.type == pygame.JOYHATMOTION:
                hat = event.value  # 直接取 event.value 即 (x,y)
                if hat == (0, 1):
                    text_print.tprint(screen, "Hat → Base of Platform A ↑")
                    Servo = "platAbase,1"
                elif hat == (0, -1):
                    text_print.tprint(screen, "Hat → Base of Platform B ↑")
                    Servo = "platBbase,1"
                elif hat == (1, 0):
                    text_print.tprint(screen, "Hat → Reset")
                    Servo = "reset,0"

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
                x_axis = joystick.get_axis(0)
                y_axis = -joystick.get_axis(1)

                # 只根據正負決定方向，不處理速度
                wheel_speeds = [
                    y_axis + x_axis,  # A
                    y_axis - x_axis,  # B
                    y_axis - x_axis,  # C
                    y_axis + x_axis  # D
                ]

                # 只發送方向，不發送速度
                data_packet = ""
                for speed in wheel_speeds:
                    direction = 1 if speed >= 0 else 0
                    data_packet += f"{direction}:"

                Send_str = f"{data_packet}:{Servo}\n"
                text_print.tprint(screen, Send_str)
                if arduinoA and arduinoA.is_open:
                    arduinoA.write(Send_str.encode())
            except pygame.error:
                text_print.tprint(screen, "錯誤: 無法讀取搖桿軸值")

        pygame.display.flip()
        clock.tick(30)  # 控制更新速率

    if arduinoA and arduinoA.is_open or arduinoB and arduinoB.is_open:
        arduinoA.close()
        arduinoB.close()
    pygame.quit()


if __name__ == "__main__":
    main()
