#ifndef INPUT_H
#define INPUT_H
#include "../common.h"
#include "../window/windowManager.h"

/**
 * Initializes SDL input for typing.
 * 
 * Must be called before using any typing-style input with SDL.
 */
void initialize_input(void);

/**
 * Tells SDL to stop expecting typing-style input.
 * 
 * Call this to clean up when users are done typing.
 */
void close_input(void);

/**
 * Polls `event` for text input and stores
 * that input into `buffer`.
 * 
 * Must be called after initialize_input()
 * and before close_input().
 * 
 * \param `event` is the address of the event to be polled
 * \param `buffer` is the buffer to which the input is written, 
 *        if there is space.
 * 
 * Returns `buffer` for flexibility, but using this is optional.
 */
char* get_input(SDL_Event* event, char buffer[]);

/**
 * Renders `color` `text` at position `x`, `y` on the text editing window.
 * Does nothing and returns 0 when `text` is empty. 
 * 
 * \param `text` is the text to be rendered.
 * \param `x` is the x coordinate of rendering position.
 * \param `y` is the y coordinate of rendering position.
 * \param `color` is the color of the text to render.
 * 
 * Returns 1 on success, 0 on failure.
 */
int render_input(const char* text, int x, int y, SDL_Color* color);

#endif