import os
import ConfigParser
import sys

config = ConfigParser.ConfigParser()
config.read("settings.cfg")

if len(sys.argv) <= 1:
    os.system("ip route add %s dev eth0" % config.get("microcontroller", "gateway"))
    os.system("route add default gw %s dev eth0" % config.get("microcontroller", "gateway"))
    os.system("ip route add %s dev eth0" % config.get("microcontroller", "ip"))
    os.system("ip route show")
elif sys.argv[1] == "undo":
    os.system("ip route del %s dev eth0" % config.get("microcontroller", "gateway"))
    os.system("route delete default gw %s dev eth0" % config.get("microcontroller", "gateway"))
    os.system("ip route del %s dev eth0" % config.get("microcontroller", "ip"))
    os.system("ip route show")
