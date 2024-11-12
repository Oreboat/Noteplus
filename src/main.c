#include <stdio.h>
#include <string.h>
#include "window/windowManager.h"
#include "input/input.h"
#include "common.h"

int main(int argc, char* argv[]) {

    /* Buffer exists in main: in main.c, have something to delimit  */
    /* the buffer by so that in the main loop, when a newline       */
    /* is detected you lower the render coords by the length        */
    /* of font                                                      */

    uint32_t start_time, frame_time;
    int running = initialize_window();

    initialize_input();

    char inputText[MAX_INPUT_LENGTH] = "";  // C string to hold the input

    while (running) {
    
        start_time = SDL_GetTicks();

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

        frame_time = SDL_GetTicks() - start_time;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);  // Delay to match target frame time
        }

    }

    close_input();
    destroy_window();

    return 0;
}