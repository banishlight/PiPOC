import pygame
import sys
import Diagnostics as Diag
import MainMenu as Menu
import Visualizer as Vis
from threading import Thread, Lock


def draw(displayPass):
    print("graphic thread starting!")
    while 1:  # Loop Graphics
        print("tick")
        MainLock.acquire()
        MainObj.draw(displayPass)
        pygame.display.flip()  # Update the entire display
        MainLock.release()
        CLOCK.tick(MAXFPS)  # Cap Frame Rate


menuState = "test"  # a state variable
MAXFPS = 60  # Maximum rate the loop will run at
TICKRATE = 12
CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
MainLock = Lock()
if __name__ == '__main__':
    pygame.init()
    display = pygame.display.set_mode((1024, 600))

    MainObj = Menu.Screen()
    ExitCode = 0

    # create separate thread for drawing the screen
    DrawingThread = Thread(target=draw, args=(display,))
    DrawingThread.start()

    looping = True
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                print("Program Ending!")
                pygame.quit()
                sys.exit()
            # detect a click event, call click event in MainObj, pass cursor coordinates

        # run menu state
        ExitCode = MainObj.on_loop()

        # Check exit code
        if ExitCode != 0:
            MainLock.acquire()  # Lock the Main Object from being drawn over
            if ExitCode == 1:
                MainObj = Menu.Screen()
            elif ExitCode == 2:
                MainObj = Diag.Screen()
            elif ExitCode == 3:
                MainObj = Vis.Screen()
            elif ExitCode == 99:
                looping = False
            MainLock.release()  # Release the Main Object

        CLOCK.tick(TICKRATE)  # Cap Logic Rate
