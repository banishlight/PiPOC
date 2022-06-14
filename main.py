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


class Main:
    exitcode = 0
    mainLock = Lock()
    promptObj = None
    connection = None
    clock = pygame.time.Clock()  # Object used to restrict framerate of program
    mainObj = None
    drawing = True

    def draw(self, displayPass):
        print("graphic thread starting!")
        while self.drawing:  # Loop Graphics
            self.mainLock.acquire()
            self.mainObj.draw(displayPass)
            pygame.display.flip()  # Update the entire display
            self.mainLock.release()
            self.clock.tick(MAXFPS)  # Cap Frame Rate
            while self.promptObj is not None:
                print("prompting!")

    def prompt(self, text):
        rect = pygame.Rect(256, 150, 512, 300)
        print(text)
        return PromptBox(rect, text)

    def set_obd_connection(self, result):
        self.connection = result
        return

    def __init__(self):
        pygame.init()
        display = pygame.display.set_mode((1024, 600))
        pygame.font.init()
        self.mainObj = Menu.Screen()

        print("Before drawing thread")
        # create separate thread for drawing the screen
        drawingthread = Thread(target=self.draw, args=(display,))
        drawingthread.start()

        looping = True
        while looping:  # Main loop of program
            # Check for input
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    print("Program Ending!")
                    self.drawing = False  # kill DrawingThread before closing
                    self.mainLock.acquire()
                    pygame.display.quit()  # fixes a bug where the window won't close
                    self.mainLock.release()
                    pygame.font.quit()
                    pygame.quit()
                    sys.exit()
                # detect a click event, call click event in MainObj, pass cursor coordinates
                elif event.type == pygame.MOUSEBUTTONUP:
                    if event.button == 1:  # Button 1 is left mouse button
                        self.mainObj.click(pygame.mouse.get_pos())

            # run main loop
            exitcode = self.mainObj.on_loop(self)

            # Check exit code
            if exitcode != 0:
                self.mainLock.acquire()  # Lock the Main Object from being drawn over
                if exitcode == 1:  # Main Menu
                    self.mainObj = Menu.Screen()
                elif exitcode == 2:  # Diagnostics
                    self.mainObj = Diag.Screen()
                elif exitcode == 3:  # Visualizer
                    self.mainObj = Vis.Screen()
                elif exitcode == 4:  # Settings
                    self.mainObj = Settings.Screen()
                elif exitcode == 5:  # Connecting
                    self.mainObj = Connecting.Screen()
                elif exitcode == 99:
                    looping = False
                self.mainLock.release()  # Release the Main Object

            self.clock.tick(TICKRATE)  # Cap Logic Rate


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
