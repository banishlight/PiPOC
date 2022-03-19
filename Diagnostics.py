# coding=utf-8
import pygame
import obd

# Constants
BACKGROUND_COLOUR = (0, 0, 0)  # Black

PORTSTR = None
BAUDRATE = 500000  # Serial of the connection, varies from adapter to adapter
PROTOCOL = None
FAST = True
CHECKVOLTAGE = True
TIMEOUT = 1.0

BUFFERMAX = 6


class Screen:
    EXITCODE = 0

    connection = 0
    state = "no connection"

    number_font = 0
    text_font = 0
    status_font = 0

    buttonList = []
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

    def __init__(self):
        self.EXITCODE = 0
        # Load Icons
        obj = Button((0, 0), "icons/Exit_button.png", 64, 64)  # Exit button
        self.buttonList.append(obj)
        # Load font
        self.number_font = pygame.font.Font("cnr.otf", 80)
        self.text_font = pygame.font.Font("cnr.otf", 50)
        self.status_font = pygame.font.Font("cnr.otf", 24)
        # Connect to OBD
        self.connect_obd()
        return

    def on_loop(self):
        # OBD connected
        if self.state == "connected":
            # Gather sensors
            for a in range(len(self.SensorList)):
                query = self.connection.query(self.SensorList[a]["command"])
                current_value = query.value.magnitude

                if len(self.SensorList[a]["buffer"]) == BUFFERMAX:  # buffer full, remove oldest value
                    self.SensorList[a]["buffer"].pop(0)
                self.SensorList[a]["buffer"].append(current_value)  # add the newest value to buffer
                # total up buffer
                total = 0
                size = len(self.SensorList[a]["buffer"])
                for b in range(size):
                    total += self.SensorList[a]["buffer"][b]
                # Average value in the buffer is final value
                self.SensorList[a]["final"] = int(total / size)
        # OBD not connected

        return self.EXITCODE

    def draw(self, display):
        display.fill(BACKGROUND_COLOUR)
        status_surf = self.status_font.render(self.state, True, (255, 255, 255))
        display.blit(status_surf, (0, 576))
        # Draw exit button
        display.blit(self.buttonList[0].image, self.buttonList[0].coord)
        if self.state == "connected":
            # Draw RPM
            rpm_surf = self.number_font.render((str(self.SensorList[1]["final"]) + self.SensorList[1]["label"]), True,
                                               (255, 255, 255))
            display.blit(rpm_surf, (720, 72))
        return

    def click(self, coord):
        if self.buttonList[0].collision.collidepoint(coord):  # Exit button to main menu
            self.EXITCODE = 1
        return

    def connect_obd(self):
        self.connection = obd.OBD(portstr=PORTSTR, baudrate=BAUDRATE, protocol=PROTOCOL, fast=FAST, timeout=TIMEOUT,
                                  check_voltage=CHECKVOLTAGE)
        if self.connection.status() == obd.OBDStatus.CAR_CONNECTED:
            self.state = "connected"
        else:
            self.state = "failed connection"
        return None


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
