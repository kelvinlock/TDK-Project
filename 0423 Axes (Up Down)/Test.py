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
    pygame.display.set_caption("Xbox Controller to Arduino")

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
                print(f"手柄 {joy.get_instance_id()} 已連接")
            elif event.type == pygame.JOYDEVICEREMOVED:
                del joysticks[event.instance_id]
                print(f"手柄 {event.instance_id} 已斷開")

        # 清空畫面
        screen.fill((255, 255, 255))
        text_print.reset()

        # 處理所有已連接手柄
        for joystick in joysticks.values():
            # 取得左搖桿Y軸值（通常是axis 1）
            axis_value = joystick.get_axis(1)  # Xbox左搖桿Y軸
            pwm_value = map_axis_to_pwm(-axis_value)  # 反轉軸向並轉換為PWM

            # 在畫面顯示數據
            text_print.tprint(screen, f"Y軸原始值: {axis_value:.3f}")
            text_print.tprint(screen, f"轉換PWM值: {pwm_value}")

            # 發送數據到Arduino（格式範例: "PWM:255\n"）
            try:
                arduino.write(f"PWM:{pwm_value}\n".encode())
            except Exception as e:
                print(f"數據發送失敗: {e}")
                done = True
        arduino.write(f"PWM:{pwm_value}\n".encode())
        # 更新畫面
        pygame.display.flip()
        clock.tick(30)  # 控制更新速率

    # 清理資源
    arduino.close()
    pygame.quit()


if __name__ == "__main__":
    main()
