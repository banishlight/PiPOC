import pygame

BACKGROUND_COLOUR = (0, 0, 0)  # Black


class Screen:
    exitcode = 0
    buttonlist = []

    def __init__(self):
        self.text_font = pygame.font.Font("cnr.otf", 64)
        return

    def on_loop(self):
        return self.exitcode

    def draw(self, display):
        display.fill(BACKGROUND_COLOUR)
        text_surf = self.text_font.render("Connecting to your OBD...", True, (255, 255, 255))
        display.blit(text_surf, (100, 120))
        return

    def click(self, coord):
        return
