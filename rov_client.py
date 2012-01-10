import socket
import ConfigParser

class Connection:

    def __init__(self, ip = None, port = None, timeout = None):
        self.config = ConfigParser.ConfigParser()
        self.config.read("rov.cfg")

        if not ip:
            self.ip         = self.config.get("connection","ip")
        if not port:
            self.port       = self.config.getint("connection","port")
        if not timeout:
            self.timeout    = self.config.getint("connection","timeout")

        self.connected  = False
        self.sock2micro = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        while not self.connected:
            self.connect()

    def connect(self):
        try:
            print "Connecting to microcontroller."
            self.close()
            self.sock2micro = socket.socket(socket.AF_INET, \
                                            socket.SOCK_STREAM)
            self.sock2micro.settimeout(self.timeout)
            self.sock2micro.connect((self.ip, self.port))
            print "Connected successfully."
            self.connected = True
        except Exception, e:
            print "%s error" % e
            self.connected = False

    def close(self):
        self.sock2micro.close()
        self.connected = False

    def send(self, data):
        if self.connected:
            try:
                self.sock2micro.send(data)
            except Exception, e:
                print '%s error sending "%s"' % (e, data)
                self.connected = False

    def recv(self, length = 1024):
        if self.connected:
            try:
                received = self.sock2micro.recv(length)
                return received
            except Exception, e:
                print '%s error receiving %d bytes' % (e, length)
                self.connected = False
        return None


if __name__ == "__main__":
    conn = Connection()
    conn.send("2")
    #data = conn.recv(4)
    conn.close()
