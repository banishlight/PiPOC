import pygame

BACKGROUND_COLOUR = (112, 112, 112)  # Grey

# in use
buttonList = []


def initialize():
    # Power Button
    obj = Button((112, 72), "icons/power-button.png", 192, 192)
    buttonList.append(obj)
    # Visualizer Button
    obj = Button((416, 72), "icons/sound-waves.png", 192, 192)
    buttonList.append(obj)
    # OBD Button
    obj = Button((720, 72), "icons/speedometer.png", 192, 192)
    buttonList.append(obj)
    # Settings Button
    obj = Button((112, 416), "icons/settings-knobs.png", 192, 192)
    buttonList.append(obj)
    return


def on_loop(display):
    return


def unload(self):
    self.buttonList = []
    return


class Button():
    image = 0
    coord = (0, 0)
    collision = pygame.rect

    def __init__(self, newXY, imgPath, width, height):
        self.coord = newXY
        self.image = pygame.image.load(imgPath)
        self.collision = pygame.Rect(newXY[0], newXY[1], width, height)

    def set_coord(self, newXY):
        self.coord = newXY

    def set_image(self, imgPath):
        self.image = pygame.image.load(imgPath)
