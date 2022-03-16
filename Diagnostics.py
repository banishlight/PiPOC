import pygame
import obd

# Constants
BAUDRATE = 500000  # Serial of the connection, varies from adapter to adapter
BUFFERMAX = 6
PORTSTR = None
PROTOCOL = None
FAST = True
CHECKVOLTAGE = True

class Screen:

    def __init__(self):
        # Load Icons

        # Connect to OBD
        connection = obd.OBD(portstr=PORTSTR, baudrate=BAUDRATE, protocol=PROTOCOL, fast=FAST, timeout=0.1, check_voltage=CHECKVOLTAGE)
        return

    def on_loop(self):
        return 0

    def draw(self, display):
        return

    def click(self, coord):
        return
