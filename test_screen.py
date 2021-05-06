import pygame
import obd

print("Starting connection to OBD")
connection = obd.OBD(portstr=None, baudrate=500000, protocol=None, fast=True, timeout=0.1, check_voltage=True)
print("OBD connection finished")

exitImage = pygame.image.load("icons/Exit_button.png")
sensorList = [{"Label": "RPM", "Command": obd.commands.RPM, "Value": 0, "Unit": ""},
              {"Label": "Speed", "Command": obd.commands.SPEED, "Value": 0, "Unit": " KM/h"},
              {"Label": "Throttle Position", "Command": obd.commands.THROTTLE_POS, "Value": 0, "Unit": "%"},
              {"Label": "Engine Load", "Command": obd.commands.ENGINE_LOAD, "Value": 0, "Unit": "%"},
              {"Label": "Timing Advance", "Command": obd.commands.TIMING_ADVANCE, "Value": 0, "Unit": "°"},
              {"Label": "Ambient Air Temp", "Command": obd.commands.AMBIANT_AIR_TEMP, "Value": 0, "Unit": "°C"},
              {"Label": "Engine Temp", "Command": obd.commands.COOLANT_TEMP, "Value": 0, "Unit": "°C"},
              {"Label": "Intake Temp", "Command": obd.commands.INTAKE_TEMP, "Value": 0, "Unit": "°C"}]

green = (0, 255, 0)
black = (0, 0, 0)

pygame.font.init()
spacing = 24
font = pygame.font.Font("cnr.otf", spacing)


def draw(display):
    display.fill(black)
    display.blit(exitImage, (900, 0))
    for a in range(len(sensorList)):
        text = font.render(sensorList[a]["Label"] + ": " + str(sensorList[a]["Value"]) + "" + sensorList[a]["Unit"],
                           True, green)
        display.blit(text, (0, a * spacing))


def read_OBD(sensorList):
    for a in range(len(sensorList)):
        command = sensorList[a]["Command"]
        response = connection.query(command)
        sensorList[a]["Value"] = response.value


def run(display):  # Main loop of test_screen
    read_OBD(sensorList)
    draw(display)
