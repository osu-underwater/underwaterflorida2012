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

    def mode_change(self, mode):
        self.mode = mode
        orientation = self.config.get("mode.%s" % mode, "orientation")
        for t in self.thrusters:
            if t.orientation == orientation:
                t.get_instructions(mode)

    def go(self, parameters):
        for t in self.thrusters:
            t.go(parameters)

    def report(self):
        return [t.vector for t in self.thrusters]

    def command(self):
        """
        'a' - Analog write (2 operands: pin, value)
        'd' - Digital write (2 operands: pin, high/low)
        """
        data = []
        for t in self.thrusters:
            data += ['a', t.magnitude, int(min(abs(t.vector)*255, 255*self.pwm_cap))]
            data += ['d', t.forward, 'H' if t.vector >= 0 else 'L']
            data += ['d', t.reverse, 'H' if t.vector <  0 else 'L']
        data = bytearray(data)
        self.micro.send(data)

    def __str__(self):
        s = ""
        for t in self.thrusters:
            s = "%s%s\n" % (s, t)
        return s

if __name__ == "__main__":
    r = Rov()
    r.command()
