import pygame


class Circle:
    radius = 0
    x = 0
    y = 0
    colour_inner = (0, 255, 0)
    colour_outer = (0, 0, 0)
    min = 0
    max = 0
    line_x = 0
    line_y = 0

    def __init__(self, r, x, y, colourInner, colourOuter):
        self.radius = r
        self.x = x
        self.y = y
        self.colour_inner = colourInner
        self.colour_outer = colourOuter

    def update(self, current):
        angle = ((current / max) * 270) + 45
        if angle > 315:
            angle = 315
