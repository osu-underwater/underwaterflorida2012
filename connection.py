import socket
import ConfigParser
from time import sleep

class Connection:
    def __init__(self, device="microcontroller", cfg="temp_settings.cfg", verbose=True):
        '''
        Initializes a Connection object using a TCP/IP socket.
        PARAMETERS: Custom values for ip address, port, and timeout.
            A config file is used where parameters are not given.
        '''
        self.verbose = verbose
        config = ConfigParser.ConfigParser()
        config.read(cfg)

        self.ip         = config.get(device,"ip")
        self.port       = config.getint(device,"port")
        self.timeout    = config.getint(device,"timeout")

        self.connected  = False
        self.sock2micro = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    def connect(self):
        '''
        Establishes the connection to the configured IP.
        '''
        try:
            self.close()
            self.log("Connecting to microcontroller at %s:%s." % \
                    (self.ip, self.port))
            self.sock2micro = socket.socket(socket.AF_INET, \
                                            socket.SOCK_STREAM)
            self.sock2micro.settimeout(self.timeout)
            self.sock2micro.connect((self.ip, self.port))
            self.log("Connected successfully.")
            self.connected = True
        except socket.error, e:
            self.log("%s error" % e)
            self.connected = False

    def close(self):
        '''
        Closes the connection.
        '''
        self.sock2micro.close()
        self.connected = False
        self.log("Connection closed.")

    def send(self, data):
        '''
        Writes specified data to the socket.
        PARAMETERS: data - Byte array to send
        '''
        if self.connected:
            try:
                self.sock2micro.send(data)
            except socket.error, e:
                self.log('%s error sending "%s"' % (e, data))
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
            except socket.error, e:
                self.log('%s error receiving %d bytes' % (e, length))
                self.connected = False
        return None

    def log(self, msg):
        if self.verbose:
            print msg


if __name__ == "__main__":
    conn = Connection()
    while not conn.connected:
        conn.connect()
        sleep(0.01);

    #conn.send("2")
    #data = conn.recv(4)
    #print data
    conn.close()
