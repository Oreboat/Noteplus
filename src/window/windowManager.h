#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "../common.h"
#include <GL/glew.h>
#include <stdio.h>


/* Initialize all necessary properties of program window */
int initialize_window(void);

SDL_Window* get_window(void);

SDL_Renderer* get_renderer(void);

TTF_Font* get_font(void);

SDL_Color* get_color(void);

/* Cleans up after window resources */
void destroy_window(void);

#endif // WINDOW_MANAGER_H