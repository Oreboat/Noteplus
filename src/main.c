#include <stdio.h>
#include <string.h>
#include "window/windowManager.h"
#include "input/input.h"
#include "common.h"

int main(int argc, char* argv[]) {

    int running = initialize_window();

    initialize_input();

    char inputText[MAX_INPUT_LENGTH] = "";  // C string to hold the input
    char displayText[MAX_INPUT_LENGTH] = "";

    int framecount = 0;
    while (running) {
        // TODO:
        /* Current model is CPU-hog, cap framerate. */
        /* However, do not sacrifice input responsiveness. */
        printf("frame number: %d\n", framecount++);
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            get_input(&event, inputText);
            render_input(inputText, 0, 0, get_color());
        }

        SDL_SetRenderDrawColor(get_renderer(), 0, 0, 0, 255);  // Clear screen
        SDL_RenderClear(get_renderer());
    }

    close_input();
    destroy_window();

    return 0;
}