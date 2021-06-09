import pygame
import sys
import test_screen as TS
import Diagnostics as Diag
import MainMenu as Menu

menuState = "test"  # a state variable
MAXFPS = 60  # Maximum rate the loop will run at
CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
if __name__ == '__main__':
    pygame.init()
    display = pygame.display.set_mode((1024, 600))

    looping = True
    ExitCode = 0
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        # Check menu state
        if menuState == "Menu":
            Menu.on_loop(display)

        elif menuState == "test":
            ExitCode = TS.on_loop(display)

        # Check exit code

        pygame.display.flip()  # Update the entire display
        CLOCK.tick(MAXFPS)  # Cap Framerate
