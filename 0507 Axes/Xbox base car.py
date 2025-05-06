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


def map_axis_to_pwm(axis_value):
    """將搖桿軸值(-1.0~1.0)映射到PWM值(0~255)，127為停止點"""
    # 先將軸值縮放到 -127~127，然後偏移到 0~254
    pwm = int(axis_value * 127 + 127)
    # 確保 127 是唯一停止點（避免浮點誤差）
    if abs(pwm - 127) < 2:  # 允許微小誤差
        return 127
    return pwm


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
                axis_value0 = joystick.get_axis(0)
                axis_value1 = joystick.get_axis(1)  # 如果軸不存在會觸發異常
                pwm_value0 = map_axis_to_pwm(-axis_value0)
                pwm_value1 = map_axis_to_pwm(-axis_value1)

                text_print.tprint(screen, f"Original X-axis value: {axis_value0:.3f}")
                text_print.tprint(screen, f"PWM output value: {pwm_value0}")

                text_print.tprint(screen, f"Original Y-axis value: {axis_value1:.3f}")
                text_print.tprint(screen, f"PWM output value: {pwm_value1}")

                # 發送數據到Arduino
                try:
                    arduino.write(f"X:{pwm_value0},Y:{pwm_value1}\n".encode())
                except serial.SerialException as e:
                    print(f"串口寫入失敗: {e}")
                    done = True
            except pygame.error:
                text_print.tprint(screen, "錯誤: 無法讀取搖桿軸值")

        pygame.display.flip()
        clock.tick(30)  # 控制更新速率

    # 正確關閉資源（主迴圈結束後執行）
    arduino.close()
    pygame.quit()


if __name__ == "__main__":
    main()
