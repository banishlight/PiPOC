import pygame
import obd

# Constants
BAUDRATE = 500000  # Serial of the connection, varies from adapter to adapter
BUFFERMAX = 6

class Screen:

    def __init__(self):
        # Load Icons

        # Connect to OBD
        connection = obd.OBD(portstr=None, baudrate=BAUDRATE, protocol=None, fast=True, timeout=0.1, check_voltage=True)
        return

    def on_loop(self):
        return 0

    def draw(self, display):
        return

    def click(self, coord):
        return
