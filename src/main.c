#include <stdio.h>
#include <string.h>
#include "window/windowManager.h"
#include "input/input.h"
#include "common.h"

int newline_counter(char buffer[]) 
{
    int i;
    int newline_count = 0;
    while (buffer[i] != '\0') 
    {
        if(buffer[i] == '\n') 
        {
            ++newline_count;
        }

        ++i;
    }

    return newline_count;
}

int main(int argc, char* argv[]) {

    /* Buffer exists in main: in main.c, have something to delimit  */
    /* the buffer by so that in the main loop, when a newline       */
    /* is detected you lower the render coords by the length        */
    /* of font                                                      */

    uint32_t start_time, frame_time;
    uint32_t render_x, render_y;
    uint32_t newline_count;
    uint32_t font_height;
    TTF_Font* font;

    /* Init phase */
    int running = initialize_window();
    initialize_input();

    font = get_font();
    if (font == NULL)
    {
        puts("font failed in main.");
        return -1;
    }

    font_height = TTF_FontHeight(font); // magic number for proper alignment
    printf("Font height: %d\n", font_height);

    /* Buffer to store and check user input */
    char inputText[MAX_INPUT_LENGTH] = ""; 

    while (running) {
        /* Maybe keep running count of newlines, so you can backspace them too. */
        
        start_time = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
    
            get_input(&event, inputText);

            /* Trivial solution for now, optimize later */
            newline_count = newline_counter(inputText); 

            /* render down newline_count * fontsize */
            render_y = newline_count * font_height;
            render_x = 0;

            render_input(inputText, render_x, render_y, get_color());
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