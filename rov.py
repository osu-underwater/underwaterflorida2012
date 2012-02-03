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
        self.mode = ""
        self.micro = connection.Connection()

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

    def __str__(self):
        s = ""
        for t in self.thrusters:
            s = "%s%s\n" % (s, t)
        return s

if __name__ == "__main__":
    r = Rov()
    r.mode_change("pitch")
    r.mode_change("stdhorizontal")
    print r
