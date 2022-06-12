import pygame
import sys
from threading import Thread, Lock
# My creations
import Diagnostics as Diag
import MainMenu as Menu
import Visualizer as Vis
import Settings
import Connecting

MAXFPS = 60  # Maximum rate the screen will draw at
TICKRATE = 12  # Maximum rate the logic will loop at
DRAWING = True


class Main:

    def draw(self, displayPass):
        print("graphic thread starting!")
        while DRAWING:  # Loop Graphics
            self.MainLock.acquire()
            self.MainObj.draw(displayPass)
            pygame.display.flip()  # Update the entire display
            self.MainLock.release()
            self.CLOCK.tick(MAXFPS)  # Cap Frame Rate
            while self.promptObj is not None:
                print("prompting!")

    def prompt(self, text):
        rect = pygame.Rect(256, 150, 512, 300)

        return

    def set_obd_connection(self, result):
        self.connection = result
        return

    pygame.init()
    display = pygame.display.set_mode((1024, 600))
    pygame.font.init()
    MainObj = Menu.Screen()
    exitcode = 0
    CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
    MainLock = Lock()
    promptObj = None
    connection = None

    # create separate thread for drawing the screen
    DrawingThread = Thread(target=draw, args=(display,))
    DrawingThread.start()

    looping = True
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                print("Program Ending!")
                DRAWING = False  # kill DrawingThread before closing
                MainLock.acquire()
                pygame.display.quit()  # fixes a bug where the window won't close
                MainLock.release()
                pygame.font.quit()
                pygame.quit()
                sys.exit()
            # detect a click event, call click event in MainObj, pass cursor coordinates
            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button == 1:  # Button 1 is left mouse button
                    MainObj.click(pygame.mouse.get_pos())

        # run main loop
        exitcode = MainObj.on_loop()

        # Check exit code
        if exitcode != 0:
            MainLock.acquire()  # Lock the Main Object from being drawn over
            if exitcode == 1:  # Main Menu
                MainObj = Menu.Screen()
            elif exitcode == 2:  # Diagnostics
                MainObj = Diag.Screen()
            elif exitcode == 3:  # Visualizer
                MainObj = Vis.Screen()
            elif exitcode == 4:  # Settings
                MainObj = Settings.Screen()
            elif exitcode == 5:  # Connecting
                MainObj = Connecting.Screen()
            elif exitcode == 99:
                looping = False
                pygame.QUIT
            MainLock.release()  # Release the Main Object

        CLOCK.tick(TICKRATE)  # Cap Logic Rate


class PromptBox:
    result = None
    rect = None
    text = "None was given"

    def __init__(self, rect, text):
        self.rect = rect
        self.text = text

    def get_result(self):
        return self.result

    def draw(self):
        return


if __name__ == '__main__':
    Main()
