import pygame
import obd
import main as core

BACKGROUND_COLOUR = (0, 0, 0)  # Black
PORTSTR = None
BAUDRATE = 500000  # Serial of the connection, varies from adapter to adapter
PROTOCOL = None
FAST = True
CHECKVOLTAGE = True
TIMEOUT = 1.0


class Screen:
    exitcode = 0
    buttonlist = []
    connection = None

    def __init__(self):
        self.text_font = pygame.font.Font("cnr.otf", 64)
        return

    def on_loop(self):
        if self.connect_obd():
            self.exitcode = 2
        else:
            self.exitcode = 0
        return self.exitcode

    def draw(self, display):
        display.fill(BACKGROUND_COLOUR)
        text_surf = self.text_font.render("Connecting to your OBD...", True, (255, 255, 255))
        display.blit(text_surf, (100, 120))
        return

    def click(self, coord):
        return

    def connect_obd(self):
        self.connection = obd.OBD(portstr=PORTSTR, baudrate=BAUDRATE, protocol=PROTOCOL, fast=FAST, timeout=TIMEOUT,
                                  check_voltage=CHECKVOLTAGE)
        if self.connection.status() == obd.OBDStatus.CAR_CONNECTED:
            return True
        else:
            return False
