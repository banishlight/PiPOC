import pygame
import obd

connection = obd.OBD()

exitImage = pygame.image.load("icons/Exit_button.png")
sensorDict = {"RPM": 0,
              "Speed": 0}

green = (0, 255, 0)
black = (0, 0, 0)
font = pygame.font.Font("cnr.otf", 12)


def draw(display):
    display.fill(black)
    display.blit(exitImage, (0, 540))
    textRPM = font.render("Engine RPM: " + str(sensorDict["RPM"]), True, green)
    display.blit(textRPM, (0, 0))


def read_OBD(sensors):
    sensors["RPM"] = obd.commands.RPM


def run(display):  # Main loop of test_screen
    read_OBD(sensorDict)
    draw(display)
