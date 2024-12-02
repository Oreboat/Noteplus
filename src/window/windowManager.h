#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "../common.h"

#include <GL/glew.h>
#include <stdio.h>


/**
 * MUST BE CALLED BEFORE ANY WINDOW-RELATED LOGIC.
 * If anything is done before initializing the window,
 * behavior is undefined.
 * 
 * Returns 0 on failure, 1 on success.
 */
int initialize_window(void);



/**
 * Logically should be called after initialize_window(),
 * otherwise will always return NULL.
 * 
 * Returns pointer to the application window, NULL on failure.
 */
SDL_Window* get_window(void);

/**
 * Logically should be called after initialize_window(),
 * otherwise will always return NULL.
 * 
 * Returns pointer to the renderer, NULL on failure.
 */
SDL_Renderer* get_renderer(void);

/**
 * Logically should be called after initialize_window(),
 * otherwise will always return NULL.
 * 
 * Returns pointer to the font, NULL on failure.
 */
TTF_Font* get_font(void);

/**
 * Always returns a pointer to the currently used color.
 */
SDL_Color* get_color(void);

/**
 * Cleans up application window.
 * 
 * Must be called after all window logic is done, as this destroys it.
 * Must be called at the end of the program / window logic to prevent memory leaks.
 */
void destroy_window(void);

#endif // WINDOW_MANAGER_H