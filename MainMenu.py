import pygame
import os


class Screen:
    BACKGROUND_COLOUR = (112, 112, 112)  # Grey
    # in use
    buttonList = []
    EXITCODE = 0

    def __init__(self):
        self.EXITCODE = 0

        # Power Button - index 0
        obj = Button((112, 72), "icons/power-button.png", 192, 192)
        self.buttonList.append(obj)
        # Visualizer Button - index 1
        obj = Button((416, 72), "icons/sound-waves.png", 192, 192)
        self.buttonList.append(obj)
        # OBD Button - index 2
        obj = Button((720, 72), "icons/speedometer.png", 192, 192)
        self.buttonList.append(obj)
        # Settings Button - index 3
        obj = Button((112, 328), "icons/settings-knobs.png", 192, 192)
        self.buttonList.append(obj)
        # Exit Button - index 4
        obj = Button((0, 0), "icons/Exit_button.png", 64, 64)
        self.buttonList.append(obj)
        return

    def on_loop(self):
        return self.EXITCODE

    def draw(self, display):
        display.fill(self.BACKGROUND_COLOUR)
        for a in self.buttonList:
            display.blit(a.image, a.coord)
        return

    def click(self, coord):
        if self.buttonList[0].collision.collidepoint(coord):  # power button clicked
            os.system("bash scripts/shutdown.sh")
        elif self.buttonList[1].collision.collidepoint(coord):  # visualizer button clicked
            self.EXITCODE = 3
        elif self.buttonList[2].collision.collidepoint(coord):  # OBD button clicked
            self.EXITCODE = 2
        elif self.buttonList[3].collision.collidepoint(coord):  # Settings button clicked
            self.EXITCODE = 4
        elif self.buttonList[4].collision.collidepoint(coord):  # Exit button clicked
            pygame.event.post(pygame.event.Event(pygame.QUIT))
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
