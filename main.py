import pygame
import sys
import test_screen as TS
import Diagnostics as Diag
import MainMenu as Menu
import Visualizer as Vis
import _thread


def draw(displayPass):
    MainObj.draw(displayPass)
    CLOCK.tick(MAXFPS)
    return 0


menuState = "test"  # a state variable
MAXFPS = 60  # Maximum rate the loop will run at
TICKRATE = 12
CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
if __name__ == '__main__':
    pygame.init()
    display = pygame.display.set_mode((1024, 600))

    MainObj = Menu.Screen()
    ExitCode = 0

    _thread.start_new_thread(draw, display)

    looping = True
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        # run menu state
        ExitCode = MainObj.on_loop()

        # Check exit code
        if ExitCode != 0:
            if ExitCode == 1:
                MainObj = Menu.Screen()
            elif ExitCode == 2:
                MainObj = Diag.Screen()
            elif ExitCode == 3:
                MainObj = Vis.Screen()
            elif ExitCode == 99:
                looping = False

        pygame.display.flip()  # Update the entire display
        CLOCK.tick(TICKRATE)  # Cap Framerate