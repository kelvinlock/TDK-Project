import pygame
import serial
import time
import math

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


def map_axis_to_speed(axis_value, deadzone=0.1):
    """將搖桿值(-1.0~1.0)映射到-255~255，並添加死區"""
    if abs(axis_value) < deadzone:
        return 0
    return int(axis_value * 255)


def calculate_mecanum_wheels(x_speed, y_speed):
    """計算麥克納姆輪四輪速度 (A:左前, B:右前, C:左後, D:右後)"""
    # 麥克納姆輪運動學公式
    wheel_A = y_speed + x_speed  # 左前
    wheel_B = y_speed - x_speed  # 右前
    wheel_C = y_speed + x_speed  # 左後
    wheel_D = y_speed - x_speed  # 右後

    # 限制速度範圍並歸一化
    max_speed = max(abs(s) for s in [wheel_A, wheel_B, wheel_C, wheel_D])
    if max_speed > 255:
        wheel_A = int(wheel_A * 255 / max_speed)
        wheel_B = int(wheel_B * 255 / max_speed)
        wheel_C = int(wheel_C * 255 / max_speed)
        wheel_D = int(wheel_D * 255 / max_speed)

    return wheel_A, wheel_B, wheel_C, wheel_D


def main():
    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Mecanum Wheel Controller")

    try:
        #arduino = serial.Serial("COM6", 9600, timeout=1)
        time.sleep(2)  # 等待Arduino初始化
    except Exception as e:
        print(f"Serial connection failed: {e}")
        arduino = None

    clock = pygame.time.Clock()
    text_print = TextPrint()
    joysticks = {}
    done = False

    while not done:
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

        screen.fill((255, 255, 255))
        text_print.reset()

        for joystick in joysticks.values():
            # 獲取搖桿值 (X軸:0, Y軸:1)
            x_axis = joystick.get_axis(0)
            y_axis = -joystick.get_axis(1)  # 反轉Y軸符合直覺

            # 映射到速度值 (-255~255)
            x_speed = map_axis_to_speed(x_axis)
            y_speed = map_axis_to_speed(y_axis)

            # 計算四輪速度
            wheel_A, wheel_B, wheel_C, wheel_D = calculate_mecanum_wheels(x_speed, y_speed)

            # 顯示調試信息
            text_print.tprint(screen, f"X軸: {x_axis:.2f} → Speed: {x_speed}")
            text_print.tprint(screen, f"Y軸: {y_axis:.2f} → Speed: {y_speed}")
            text_print.tprint(screen, f"輪A: {wheel_A} (方向: {'正轉' if wheel_A >= 0 else '反轉'})")
            text_print.tprint(screen, f"輪B: {wheel_B} (方向: {'正轉' if wheel_B >= 0 else '反轉'})")
            text_print.tprint(screen, f"輪C: {wheel_C} (方向: {'正轉' if wheel_C >= 0 else '反轉'})")
            text_print.tprint(screen, f"輪D: {wheel_D} (方向: {'正轉' if wheel_D >= 0 else '反轉'})")

            # 發送數據到Arduino (格式: A_DIR,A_PWM:B_DIR,B_PWM:...)

            data_str = (
                    f"{1 if wheel_A >= 0 else 0},{abs(wheel_A)}:"
                    f"{1 if wheel_B >= 0 else 0},{abs(wheel_B)}:"
                    f"{1 if wheel_C >= 0 else 0},{abs(wheel_C)}:"
                    f"{1 if wheel_D >= 0 else 0},{abs(wheel_D)}\n"
                )
            try:
                    #arduino.write(data_str.encode())
                print()
            except serial.SerialException as e:
                    print(f"Serial write error: {e}")
                    done = True

        pygame.display.flip()
        clock.tick(30)  # 控制更新速率


        #arduino.close()
    pygame.quit()


if __name__ == "__main__":
    main()