import pygame
import time
import subprocess

TEXT_RED = "\033[91m"
TEXT_END = "\033[0m"

def starbar(value):
    value = int(round(abs(value)*10))
    retval = "[" + TEXT_RED
    counter = 0
    for i in range(value):
        retval += '*'
        counter += 1
    while counter < 10:
        retval += ' '
        counter += 1
    retval += "%s%s" % (TEXT_END, "]")
    return retval

if __name__ == "__main__":
    pygame.init()
    gamepad = pygame.joystick.Joystick(0)
    gamepad.init()
    print gamepad.get_numaxes()
    while True:
        subprocess.call("clear")
        pygame.event.pump()
        print
        print starbar(gamepad.get_axis(0))
        print starbar(gamepad.get_axis(1))
        print starbar(gamepad.get_axis(2))
        print starbar(gamepad.get_axis(3))
        print
        time.sleep(0.1)


