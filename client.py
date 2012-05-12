import pygame
import time
import subprocess
import ConfigParser

import rov

TEXT_RED = "\033[91m"
TEXT_GREEN = "\033[92m"
TEXT_BLUE = "\033[94m"
TEXT_END = "\033[0m"
VERBOSE = True

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

    if pygame.joystick.get_count() == 1:
        gamepad = pygame.joystick.Joystick(0)
    else:
        gamepad = pygame.joystick.Joystick(1)

    gamepad.init()

    r = rov.Rov(cfg="temp_settings.cfg")
    r.mode_change("default")
    while True:
        pygame.event.pump()
        buttons = 0x00
        for i in range(0,8):
            buttons = buttons | (gamepad.get_button(i) << i)
        parameters = {'lh': gamepad.get_axis(0) * 100, \
                'lv': gamepad.get_axis(1) * -100, \
                'rh': gamepad.get_axis(3) * 100, \
                'rv': gamepad.get_axis(2) * -100, \
                'dh': gamepad.get_hat(0)[0], \
                'dv': gamepad.get_hat(0)[1], \
                'buttons': buttons}
        r.go(parameters)
        r.command()
        if VERBOSE:
            print "\n\n\n\n\n"
            for v in r.report():
                print starbar(v)
            print "Buttons: ", buttons
            print "PID: ",
            for i in r.pid:
                print i,
            print "\nDCM: ",
            for i in r.dcm:
                print i,
            print
        time.sleep(0.05)


    gamepad.quit()
    pygame.quit()
