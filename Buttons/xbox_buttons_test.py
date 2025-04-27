import pygame
import serial

pygame.init()

# This is a simple class that will help us print to the screen.
# It has nothing to do with the joysticks, just outputting the
# information.
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
    # Set the width and height of the screen (width, height), and name the window.
    screen = pygame.display.set_mode((500, 700))
    pygame.display.set_caption("Joystick example")

    # Initialize serial inside main and handle exceptions
    try:
        arduino = serial.Serial("COM6", 9600, timeout=1)
    except Exception as e:
        print(f"Exception: {e}")
        return  # Exit if serial port can't be opened

    # Used to manage how fast the screen updates.
    clock = pygame.time.Clock()

    # Get ready to print.
    text_print = TextPrint()

    # This dict can be left as-is, since pygame will generate a
    # pygame.JOYDEVICEADDED event for every joystick connected
    # at the start of the program.
    joysticks = {}

    done = False
    while not done:
        # Event processing step.
        # Possible joystick events: JOYAXISMOTION, JOYBALLMOTION, JOYBUTTONDOWN,
        # JOYBUTTONUP, JOYHATMOTION, JOYDEVICEADDED, JOYDEVICEREMOVED
        for event in pygame.event.get():
            # Handle hotplugging
            if event.type == pygame.JOYDEVICEADDED: # True: when new device added
                # This event will be generated when the program starts for every
                # joystick, filling up the list without needing to create them manually.
                joy = pygame.joystick.Joystick(event.device_index)
                joysticks[joy.get_instance_id()] = joy
                print(f"Joystick {joy.get_instance_id()} connencted")

            # Drawing step
            # First, clear the screen to white. Don't put other drawing commands
            # above this, or they will be erased with this command.
            screen.fill((255, 255, 255))
            text_print.reset()

            # Get count of joysticks.
            joystick_count = pygame.joystick.get_count()

            text_print.tprint(screen, f"Number of joysticks: {joystick_count}")
            text_print.indent()

            if event.type == pygame.JOYBUTTONDOWN:
                print("Joystick button pressed.")
                if event.button == 0:
                    arduino.write(b'1')

            if event.type == pygame.JOYBUTTONUP:
                print("Joystick button released.")
                if event.button == 0:
                    arduino.write(b'0')

            # For each joystick:
            for joystick in joysticks.values(): # dic values
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

                buttons = joystick.get_numbuttons()
                text_print.tprint(screen, f"Number of buttons: {buttons}")
                text_print.indent()

                for i in range(buttons):
                    if joystick.get_button(i):
                        text_print.tprint(screen, f"Button {i} pressed")
                        if joystick.get_button(0):
                            text_print.tprint(screen,f"LED status: ON")
                text_print.unindent()

            if event.type == pygame.JOYDEVICEREMOVED:
                del joysticks[event.instance_id]
                print(f"Joystick {event.instance_id} disconnected")

            if event.type == pygame.QUIT:
                done = True  # Flag that we are done so we exit this loop.

            # Go ahead and update the screen with what we've drawn.
            pygame.display.flip()

            # Limit to 30 frames per second.
            clock.tick(30)
            arduino.close()


if __name__ == "__main__":
    main()
    # If you forget this line, the program will 'hang'
    # on exit if running from IDLE.
    pygame.quit()

"""
print("initialize pygame...")
print("initialize joystick...")
# 初始化pygame
pygame.init()
pygame.joystick.init()

if pygame.get_init() and pygame.joystick.get_init():
    print("initialization pygame success")
    print("initialization joystick success\n")

# 检查手柄连接
print("Detecting joystick...")
joystick_count = pygame.joystick.get_count() # 取得链接手柄数量
if joystick_count == 0:
    print("No joystick detected！")
    exit()
else :
    print("Detected: "+str(joystick_count)+" joystick")

# 初始化第一个手柄
print("\ninitialize joystick...")
joystick = pygame.joystick.Joystick(0)
if joystick.get_init():
    print("joystick initialization success")
else:
    print("joystick initialization fail")
    quit()

print("\nprinting joystick information...")
"""