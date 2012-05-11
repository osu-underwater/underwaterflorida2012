import ConfigParser
import thruster
import connection

class Rov:
    def __init__(self, cfg="settings.cfg"):

        self.config = ConfigParser.ConfigParser()
        self.config.read(cfg)
        self.thrusters = []
        for i in range(self.config.getint('rov', 'thrusters')):
            self.thrusters += \
                    [thruster.Thruster(device="rov.thruster%d" % i, config=self.config)]
        self.pwm_cap = self.config.getfloat('rov', 'pwm_cap')
        self.mode = ""
        self.micro = connection.Connection()
        self.micro.connect()
        self.buttons = 0x00
        self.dcm = []
        self.pid = []

    def mode_change(self, mode):
        self.mode = mode
        orientation = self.config.get("mode.%s" % mode, "orientation")
        for t in self.thrusters:
            if t.orientation == orientation:
                t.get_instructions(mode)

    def go(self, parameters):
        for t in self.thrusters:
            t.go(parameters)
        self.buttons = parameters['buttons']

    def report(self):
        return [t.vector for t in self.thrusters]

    def command(self):
        """
        'a' - Analog write (2 operands: pin, value)
        'd' - Digital write (2 operands: pin, high/low)
        's' - Servo write (2 operands: pin, angle [0-180])
        'b' - Buttons byte (1 operand: button value)
        """
        data = []
        for t in self.thrusters:
            if t.servo:
                value = int(max((t.vector+1)*90,0))
                data += ['s', t.magnitude, value]
            else:
                value = int(min(abs(t.vector)*255, 255*self.pwm_cap))
                data += ['a', t.magnitude, value]
                if t.forward > 0:
                    data += ['d', t.forward, 'H' if t.vector >= 0 else 'L']
                if t.reverse > 0:
                    data += ['d', t.reverse, 'H' if t.vector <  0 else 'L']
        data += ['b', self.buttons]
        data = bytearray(data)
        self.micro.send(data)
        data = self.micro.recv(1)
        if data == 'P':
            data = self.micro.recv(3)
            if data:
                self.pid = [float(ord(i)) for i in data]
                self.pid[2] = self.pid[2] / 100.0
        elif data == 'D':
            data = self.micro.recv(9)
            if data:
                self.dcm = [ord(i) for i in data]

    def __str__(self):
        s = ""
        for t in self.thrusters:
            s = "%s%s\n" % (s, t)
        return s

if __name__ == "__main__":
    r = Rov()
    r.command()
