import pygame
import math


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
        f = ((current / self.max) * 270)
        self.line_x = (self.radius * -math.cos(225 - f))  # radians multiplier: 0.0174532925
        self.line_y = (self.radius * -math.cos(225 - f))

    def draw(self):
        print("hello world")
