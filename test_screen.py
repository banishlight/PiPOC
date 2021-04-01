import pygame
import obd

print("Starting connection to OBD")
connection = obd.OBD(portstr=None, baudrate=500000, protocol=None, fast=True, timeout=0.1, check_voltage=True)
print("OBD connection finished")

exitImage = pygame.image.load("icons/Exit_button.png")
sensorList = [{"Label": "RPM", "Command": obd.commands.RPM, "Value": 0, "Unit": ""},
              {"Label": "Speed", "Command": obd.commands.RPM, "Value": 0, "Unit": "KM/h"}]

green = (0, 255, 0)
black = (0, 0, 0)

pygame.font.init()
font = pygame.font.Font("cnr.otf", 24)


def draw(display):
    display.fill(black)
    display.blit(exitImage, (900, 0))
    for a in range(len(sensorList)):
        text = font.render(sensorList[a]["Label"] + ": " + str(sensorList[a]["Value"]) + " " + sensorList[a]["Unit"],
                           True, green)
        display.blit(text, (0, a * 24))


def read_OBD(sensorList):
    for a in range(len(sensorList)):
        command = sensorList[a]["Command"]
        sensorList[a]["Value"] = command.query(command)


def run(display):  # Main loop of test_screen
    read_OBD(sensorList)
    draw(display)
