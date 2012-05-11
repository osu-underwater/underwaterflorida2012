import pygame
import time
import subprocess
import ConfigParser

import rov

TEXT_RED = "\033[91m"
TEXT_GREEN = "\033[92m"
TEXT_BLUE = "\033[94m"
TEXT_END = "\033[0m"

def starbar(value):
    retval = "[" + (TEXT_GREEN if value > 0 else TEXT_RED)
    value = int(round(abs(value)*10.0))
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

    r = rov.Rov(cfg="temp_settings.cfg")
    r.mode_change("default")
    while True:
        pygame.event.pump()
        buttons = 0x00
        for i in range(0,8):
            buttons = buttons | (gamepad.get_button(i) << i)
        parameters = {'xh': gamepad.get_axis(0) * 100, \
                'yh': gamepad.get_axis(1) * -100, \
                'xv': gamepad.get_axis(3) * 100, \
                'yv': gamepad.get_axis(2) * -100, \
                'buttons': buttons}
        r.go(parameters)
        r.command()
        subprocess.call("clear")
        for v in r.report():
            print starbar(v)
        print buttons
        time.sleep(0.1)

    gamepad.quit()
    pygame.quit()
