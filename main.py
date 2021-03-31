import pygame
import test_screen as TS

menuState = "test"  # a state object
MAXFPS = 60  # Maximum rate the loop will run at
CLOCK = pygame.time.Clock()  # Object used to restrict framerate of program
if __name__ == '__main__':
    pygame.init()
    display = pygame.display.set_mode((1024, 600))

    looping = True
    while looping:  # Main loop of program
        # Check for input
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                looping = False

        # Check menu state
        if menuState == "test":
            TS.run(display)

        pygame.display.flip() # Update the entire display
        CLOCK.tick(MAXFPS)  # Cap Framerate
