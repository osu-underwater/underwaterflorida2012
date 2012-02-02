import ConfigParser
import string


class Thruster:
    max_input = 100

    def __init__(self, device, config):

        self.config         = config
        Thruster.max_input  = self.config.getint("controls", "max_input")

        self.mount          = self.config.get(device, "mount").split()
        self.orientation    = self.config.get(device, "orientation")
        self.forward        = self.config.getint(device, "forward")
        self.reverse        = self.config.getint(device, "reverse")
        self.magnitude      = self.config.getint(device, "magnitude")

        self.vector         = 0
        self.instructions   = []
        self.max_power      = 100

    def go(self, parameters):
        instack = [0]
        powval = {}
        for inst in self.instructions:
            if inst == "+":
                instack.append(instack.pop() + instack.pop())
            elif inst == "-":
                instack.append(-1*instack.pop() + instack.pop())
            elif inst == "neg":
                instack.append(-1*instack.pop())
            else:
                instack.append(parameters[inst])
                if inst not in powval:
                    powval[inst] = parameters[inst]
        powval = min(sum([powval[i]**2 for i in powval])**0.5, self.max_power)
        modifier = instack.pop()
        powval = powval * (-1 if modifier < 0 else 1)
        self.vector = int(powval * min(abs(modifier/Thruster.max_input), 1))

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
