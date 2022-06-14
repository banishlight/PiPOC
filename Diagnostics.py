# coding=utf-8
import pygame
import obd

# Constants
BACKGROUND_COLOUR = (0, 0, 0)  # Black



BUFFERMAX = 6


class Screen:
    exitCode = 0

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
                   "unit": "°"},
                  {"label": "Engine Temp",
                   "command": obd.commands.COOLANT_TEMP,
                   "final": 0,
                   "buffer": [],
                   "unit": "°C"}
                  ]
    test_meter = None

    def __init__(self):
        self.exitCode = 0
        # Load Icons
        obj = Button((0, 0), "icons/Exit_button.png", 64, 64)  # Exit button
        self.buttonList.append(obj)
        # Load font
        self.number_font = pygame.font.Font("cnr.otf", 80)
        self.text_font = pygame.font.Font("cnr.otf", 30)
        self.status_font = pygame.font.Font("cnr.otf", 24)
        # Connect to OBD
        self.connect_obd()
        # Fuel Meter Initialized
        self.fuelMeter = Meter(100, 64, 128, 40, 200, self.SensorList[3]["label"])
        # Objects for testing
        self.test_meter = Meter(100, 64, 128, 40, 200, "Fuel %")
        return

    def on_loop(self, coreObj):
        # OBD connected
        if self.state == "connected":
            self.update_sensors()
            self.fuelMeter.update(self.SensorList[3]["final"])

        # OBD not connected
        else:
            self.test_meter.update(80)

        return self.exitCode

    def draw(self, display):
        display.fill(BACKGROUND_COLOUR)
        status_surf = self.status_font.render(self.state, True, (255, 255, 255))
        display.blit(status_surf, (0, 576))
        # Draw exit button
        display.blit(self.buttonList[0].image, self.buttonList[0].coord)
        #  OBD connected, draw values
        if self.state == "connected":
            self.draw_sensors(display)
        else:
            self.test_draw_sensors(display)
        return

    def click(self, coord):
        if self.buttonList[0].collision.collidepoint(coord):  # Exit button to main menu
            self.exitCode = 1
        return

    def draw_sensors(self, display):
        # Draw RPM
        rpm_surf = self.number_font.render((str(self.SensorList[1]["final"])), True,
                                           (255, 255, 255))
        display.blit(rpm_surf, (720, 72))
        rpm_label = self.text_font.render(self.SensorList[1]["label"], True, (255, 255, 255))
        display.blit(rpm_label, (800, 150))

        # Draw Engine Temp
        temp_surf = self.number_font.render((str(self.SensorList[5]["final"]) + self.SensorList[5]["unit"]), True,
                                            (255, 255, 255))
        display.blit(temp_surf, (400, 72))
        temp_label = self.text_font.render(self.SensorList[5]["label"], True, (255, 255, 255))
        display.blit(temp_label, (390, 150))

        # Draw Throttle Position
        throttle_surf = self.number_font.render((str(self.SensorList[2]["final"]) + self.SensorList[2]["unit"]), True,
                                                (255, 255, 255))
        display.blit(throttle_surf, (720, 360))
        throttle_label = self.text_font.render(self.SensorList[2]["label"], True, (255, 255, 255))
        display.blit(throttle_label, (670, 430))

        # Draw Timing Advance
        timing_surf = self.number_font.render((str(self.SensorList[4]["final"]) + self.SensorList[4]["unit"]), True,
                                              (255, 255, 255))
        display.blit(timing_surf, (400, 360))
        timing_label = self.text_font.render(self.SensorList[4]["label"], True, (255, 255, 255))
        display.blit(timing_label, (350, 430))

        # Draw Fuel Meter
        self.fuelMeter.draw(display)

    def update_sensors(self):
        # Gather sensors
        for a in range(len(self.SensorList)):
            query = self.connection.query(self.SensorList[a]["command"])
            if query is not None:
                current_value = query.value.magnitude
            else:
                current_value = 0

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

    def connect_obd(self):
        self.connection = obd.OBD(portstr=PORTSTR, baudrate=BAUDRATE, protocol=PROTOCOL, fast=FAST, timeout=TIMEOUT,
                                  check_voltage=CHECKVOLTAGE)
        if self.connection.status() == obd.OBDStatus.CAR_CONNECTED:
            self.state = "connected"
        else:
            self.state = "failed connection"
        return None

    def test_draw_sensors(self, display):
        # Draw test sensors
        # RPM test
        rpm_surf = self.number_font.render("10000", True, (255, 255, 255))
        display.blit(rpm_surf, (720, 72))
        rpm_label = self.text_font.render("RPM", True, (255, 255, 255))
        display.blit(rpm_label, (800, 150))

        # Coolant temp test
        temp_surf = self.number_font.render("80°C", True, (255, 255, 255))
        display.blit(temp_surf, (400, 72))
        rpm_label = self.text_font.render("Coolant Temp", True, (255, 255, 255))
        display.blit(rpm_label, (390, 150))

        # Throttle position test
        throttle_surf = self.number_font.render("30%", True, (255, 255, 255))
        display.blit(throttle_surf, (720, 360))
        throttle_label = self.text_font.render("Throttle Position", True, (255, 255, 255))
        display.blit(throttle_label, (670, 430))

        # Timing advance test
        timing_surf = self.number_font.render("15°", True, (255, 255, 255))
        display.blit(timing_surf, (400, 360))
        timing_label = self.text_font.render("Timing Advance", True, (255, 255, 255))
        display.blit(timing_label, (350, 430))

        self.test_meter.draw(display)
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


class Meter:
    max_val = 100
    rect = None
    height = 0
    width = 0
    coordX = 0
    coordY = 0
    inner_rect = None
    inner_colour = (255, 0, 0)
    text_font = 0
    new_height = 0
    value = 0
    label = "None"

    def __init__(self, max_val, X, Y, width, height, label):
        self.max_val = max_val
        self.height = height
        self.width = width
        self.coordX = X
        self.coordY = Y
        self.rect = pygame.Rect(X, Y, width, height)
        self.text_font = pygame.font.Font("cnr.otf", 24)
        self.label = label
        return

    def draw(self, display):
        if self.rect is not None and self.inner_rect is not None:
            pygame.draw.rect(display, self.inner_colour, self.inner_rect)
            pygame.draw.rect(display, (255, 255, 255), self.rect, 3)
            max_text = self.text_font.render(str(self.max_val), True, (255, 255, 255))
            display.blit(max_text, (self.coordX + self.width + 4, self.coordY))
            val_text = self.text_font.render(str(self.value), True, (255, 255, 255))
            display.blit(val_text, (self.coordX + self.width + 4, self.coordY + (self.height - self.new_height)))
            label_text = self.text_font.render(self.label, True, (255, 255, 255))
            display.blit(label_text, (self.coordX - 4, self.coordY - 24))

    def update(self, value):
        percentage = value / self.max_val
        self.value = value
        self.new_height = int(self.height * percentage)
        self.inner_rect = pygame.Rect(self.coordX, self.coordY + (self.height - self.new_height), self.width,
                                      self.new_height)
