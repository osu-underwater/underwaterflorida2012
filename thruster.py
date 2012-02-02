import ConfigParser
import string

class Thruster:
    def __init__(self, device, config):

        self.config = config

        self.mount          = self.config.get(device, "mount").split()
        self.orientation    = self.config.get(device, "orientation")
        self.forward        = self.config.getint(device, "forward")
        self.reverse        = self.config.getint(device, "reverse")
        self.magnitude      = self.config.getint(device, "magnitude")

        self.vector         = 0
        self.instructions   = []

    def go(self, parameters):
        instack = [0]
        for inst in self.instructions:
            if inst == "+":
                instack.push(instack.pop() + instack.pop())
            if inst == "-":
                instack.push(-1*instack.pop() + instack.pop())
            if inst == "neg":
                instack.push(-1*instack.pop())
            else:
                instack.push(parameters[inst])
        self.vector = instack.pop()

    def get_instructions(self, mode):
        options = self.config.options("mode.%s" % mode)
        bestoption = None
        for option in options:
            if len([i for i in self.mount if i in option]) == len(self.mount):
                bestoption = option
                break
        if not bestoption:
            for option in options:
                subopts = option.split('.')
                if set(subopts).intersection(set(self.mount)) == set(subopts):
                    bestoption = option
                    break
        if not bestoption:
            self.instructions = []
            return
        self.instructions = self.config.get("mode.%s" % mode, option).split()

    def __str__(self):
        return "%s: %s" % (string.join(self.mount), self.vector)

if __name__ == "__main__":
    pass
