import ConfigParser
import thruster

class Rov:
    def __init__(self, device="microcontroller", cfg="rov.cfg"):

        self.config = ConfigParser.ConfigParser()
        self.config.read(cfg)
        self.thrusters = []
        for i in range(self.config.getint('rov', 'thrusters')):
            self.thrusters += \
                    [thruster.Thruster(device="rov.thruster%d" % i, config=self.config)]
        self.mode = ""

    def mode_change(self, mode):
        orientation = self.config.get("mode.%s" % mode, "orientation")
        for t in self.thrusters:
            if t.orientation == orientation:
                t.get_instructions(mode)

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
