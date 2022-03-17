# coding=utf-8
import pygame
import obd

# Constants
PORTSTR = None
BAUDRATE = 500000  # Serial of the connection, varies from adapter to adapter
PROTOCOL = None
FAST = True
CHECKVOLTAGE = True
TIMEOUT = 0.1

BUFFERMAX = 6
SensorList = [{"label": "Speed",
               "command": obd.commands.SPEED,
               "final": 0,
               "buffer": [],
               "unit": "KM/h"},
              {"label": "RPM",
               "command": obd.commands.RPM,
               "final": 0,
               "buffer": [],
               "unit": ""},
              {"label": "Throttle Position",
               "command": obd.commands.THROTTLE_POS,
               "final": 0,
               "buffer": [],
               "unit": "%"},
              {"label": "Fuel Level",
               "command": obd.commands.FUEL_LEVEL,
               "final": 0,
               "buffer": [],
               "unit": "%"},
              {"label": "Timing",
               "command": obd.commands.TIMING_ADVANCE,
               "final": 0,
               "buffer": [],
               "unit": "°"}
              ]


class Screen:
    BACKGROUND_COLOUR = (0, 0, 0)  # Black
    connection = 0
    EXITCODE = 0
    buttonList = []
    number_font = 0
    text_font = 0

    def __init__(self):
        self.EXITCODE = 0
        # Load Icons
        obj = Button((0, 0), "icons/Exit_button.png", 64, 64)  # Exit button
        self.buttonList.append(obj)
        # Load font
        self.number_font = pygame.font.FONT("cnr.otf", 80)
        self.text_font = pygame.font.FONT("cnr.otf", 50)
        # Connect to OBD
        self.connection = obd.OBD(portstr=PORTSTR, baudrate=BAUDRATE, protocol=PROTOCOL, fast=FAST, timeout=TIMEOUT,
                                  check_voltage=CHECKVOLTAGE)
        return

    def on_loop(self):
        # Gather sensors
        for a in range(len(SensorList)):
            query = self.connection.query(SensorList[a]["command"])
            current_value = query.value.magnitude

            if len(SensorList[a]["buffer"]) == BUFFERMAX:  # buffer full, remove oldest value
                SensorList[a]["buffer"].pop(0)
            SensorList[a]["buffer"].append(current_value)  # add the newest value to buffer
            # total up buffer
            total = 0
            for b in range(len(SensorList[a]["buffer"])):
                total += SensorList[a]["buffer"][b]
            SensorList[a]["final"] = int(total / len(SensorList[a]["buffer"]))

        return 0

    def draw(self, display):
        display.fill(self.BACKGROUND_COLOUR)
        # Draw buttons
        for a in self.buttonList:
            display.blit(a.image, a.coord)
        # Draw RPM
        rpm_surf = self.number_font.render(SensorList[1]["final"] + SensorList[1]["label"], True, (255, 255, 255))
        display.blit(rpm_surf, (720, 72))
        return

    def click(self, coord):
        if self.buttonList[0].collision.collidepoint(coord):  # Exit button to main menu
            self.EXITCODE = 1
        return


class Button:
    image = 0
    coord = (0, 0)
    collision = None

    def __init__(self, newXY, imgLoaded, width, height):
        self.coord = newXY
        self.image = pygame.image.load(imgLoaded)
        self.collision = pygame.Rect(newXY[0], newXY[1], width, height)

    def set_coord(self, newXY):
        self.coord = newXY

    def set_image(self, imgPath):
        self.image = pygame.image.load(imgPath)
