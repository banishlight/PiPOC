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
    exitCode = 0

    def __init__(self):
        self.exitCode = 0

        return

    def on_loop(self):

        return self.exitCode

    def draw(self, display):
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

    class Startup:
        def __init__(self):
            return

        def on_loop(self):
            return

        def draw(self, display):
            return

        def click(self, coord):
            return

    class FailedConnection:
        def __init__(self):
            return

        def on_loop(self):
            return

        def draw(self, display):
            return

        def click(self, coord):
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


class Meter:
    maxVal = 100
    rect = None
    height = 0
    width = 0
    coordX = 0
    coordY = 0
    innerRect = None
    innerColour = (255, 0, 0)
    text_font = 0
    new_height = 0
    value = 0
    label = "None"

    def __init__(self, maxVal, X, Y, width, height, label):
        self.maxVal = maxVal
        self.height = height
        self.width = width
        self.coordX = X
        self.coordY = Y
        self.rect = pygame.Rect(X, Y, width, height)
        self.text_font = pygame.font.Font("cnr.otf", 24)
        self.label = label
        return

    def draw(self, display):
        if self.rect is not None and self.innerRect is not None:
            pygame.draw.rect(display, self.innerColour, self.innerRect)
            pygame.draw.rect(display, (255, 255, 255), self.rect, 3)
            max_text = self.text_font.render(str(self.maxVal), True, (255, 255, 255))
            display.blit(max_text, (self.coordX + self.width + 4, self.coordY))
            val_text = self.text_font.render(str(self.value), True, (255, 255, 255))
            display.blit(val_text, (self.coordX + self.width + 4, self.coordY + (self.height - self.new_height)))
            label_text = self.text_font.render(self.label, True, (255, 255, 255))
            display.blit(label_text, (self.coordX - 4, self.coordY - 24))

    def update(self, value):
        percentage = value / self.maxVal
        self.value = value
        self.new_height = int(self.height * percentage)
        self.innerRect = pygame.Rect(self.coordX, self.coordY + (self.height - self.new_height), self.width,
                                     self.new_height)
