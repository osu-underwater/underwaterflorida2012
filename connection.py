import socket
import ConfigParser

class Connection:
    def __init__(self, ip = None, port = None, timeout = None):
        '''
        Initializes a Connection object using a TCP/IP socket.
        PARAMETERS: Custom values for ip address, port, and timeout.
            A config file is used where parameters are not given.
        '''
        self.ip         = ip
        self.port       = port
        self.timeout    = timeout
        config = ConfigParser.ConfigParser()
        config.read("rov.cfg")

        if not self.ip:
            self.ip         = config.get("connection","ip")
        if not self.port:
            self.port       = config.getint("connection","port")
        if not self.timeout:
            self.timeout    = config.getint("connection","timeout")

        self.connected  = False
        self.sock2micro = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    def connect(self):
        '''
        Establishes the connection to the configured IP.
        '''
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
        '''
        Closes the connection.
        '''
        self.sock2micro.close()
        self.connected = False

    def send(self, data):
        '''
        Writes specified data to the socket.
        PARAMETERS: data - Byte array to send
        '''
        if self.connected:
            try:
                self.sock2micro.send(data)
            except Exception, e:
                print '%s error sending "%s"' % (e, data)
                self.connected = False

    def recv(self, length = 1024):
        '''
        Reads a specified number of bytes from the socket.
        PARAMETERS: length - Number of bytes to read
        RETURNS: received - bytes read, None if none are read
        '''
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
    while not conn.connected:
        conn.connect()
    conn.send("2")
    #data = conn.recv(4)
    conn.close()
