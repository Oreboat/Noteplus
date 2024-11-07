#ifndef INPUT_H
#define INPUT_H
#include "../common.h"
#include "../window/windowManager.h"

void initialize_input(void);

void close_input(void);

/* Polls `event` and puts result in `buffer` */
char* get_input(SDL_Event* event, char buffer[]);

/* Renders `text` in `color` at the window position `x`, `y` */
int render_input(const char* text, int x, int y, SDL_Color* color);

#endif