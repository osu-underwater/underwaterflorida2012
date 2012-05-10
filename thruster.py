import ConfigParser
import string


class Thruster:
    max_input = None

    def __init__(self, device, config):

        self.config         = config
        if not Thruster.max_input:
            Thruster.max_input = self.config.getint("controls", "max_input")

        self.mount          = self.config.get(device, "mount").split()
        self.orientation    = self.config.get(device, "orientation")
        self.magnitude      = self.config.getint(device, "magnitude")

        self.servo = self.config.getboolean(device, 'servo') if \
                self.config.has_option(device, 'servo') else False

        self.forward = self.config.getint(device, 'forward') if \
                self.config.has_option(device, 'forward') else 0
        self.reverse = self.config.getint(device, 'reverse') if \
                self.config.has_option(device, 'reverse') else 0

        self.fscale = self.config.getfloat(device, 'forward_scale') if \
                self.config.has_option(device, 'forward_scale') else 1.0
        self.rscale = self.config.getfloat(device, 'reverse_scale') if \
                self.config.has_option(device, 'reverse_scale') else 1.0
        self.rscale = self.rscale * -1

        self.vector         = 0
        self.instructions   = []

    def go(self, parameters):
        instack = []
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
        modifier = instack.pop()
        powval = min((sum([powval[i]**2 for i in powval])**0.5 / Thruster.max_input), 1)
        powval = powval * min(abs(modifier/Thruster.max_input), 1)
        powval = powval * (self.rscale if modifier < 0 else self.fscale)
        self.vector = powval
        #print self.vector

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
