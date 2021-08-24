import pygame
import sys
import test_screen as TS
import Diagnostics as Diag
import MainMenu as Menu
import Visualizer as Vis

menuState = "test"  # a state variable
MAXFPS = 60  # Maximum rate the loop will run at
CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
if __name__ == '__main__':
    pygame.init()
    display = pygame.display.set_mode((1024, 600))
    MainObj = TS
    looping = True
    ExitCode = 0
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        # run menu state
        ExitCode = MainObj.on_loop(display)

        # Check exit code
        if ExitCode != 0:
            MainObj.unload()
            if ExitCode == 1:
                MainObj = Menu
            elif ExitCode == 2:
                MainObj = Diag
            elif ExitCode == 3:
                MainObj = Vis
            MainObj.initialize()

        pygame.display.flip()  # Update the entire display
        CLOCK.tick(MAXFPS)  # Cap Framerate
