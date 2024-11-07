#include "input.h"

void initialize_input() {
    SDL_StartTextInput();
}

void close_input() {
    SDL_StopTextInput();
}

int render_input(const char* text, int x, int y, SDL_Color* color) {
    
    if(strlen(text) == 0) {
        // clears screen if there should be no text
        SDL_SetRenderDrawColor(get_renderer(), 0, 0, 0, 255);
        SDL_RenderClear(get_renderer());
        SDL_RenderPresent(get_renderer());
        return 0;
    }

    if(get_font() == NULL) {
        puts("Could not get font.");
        return 0;
    }

    SDL_Surface* text_surface = TTF_RenderText_Solid(get_font(), text, *color);
    if (!text_surface) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }

    SDL_Renderer* renderer = get_renderer();
    if(renderer == NULL) {
        puts("rendering failed, renderer is null.");
        return 0;
    }

    // Create texture from surface pixels
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return 0;
    }

    // Set rendering position and size
    SDL_Rect renderQuad = { x, y, text_surface->w, text_surface->h };

    // Render the texture to the screen
    SDL_RenderCopy(renderer, text_texture, NULL, &renderQuad);
    SDL_RenderPresent(renderer);

    // Free the surface and texture
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    return 1;
}

char* get_input(SDL_Event* event, char buffer[]) {
    
    if(strlen(buffer) == MAX_INPUT_LENGTH) {
        // TODO:
        /* force return string for saving to note, clear buffer */
        printf("Input: %s\n", buffer);
        buffer[0] = '\0';
    }

    // detects non-printable keys
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(buffer) > 0) {
            // Handle backspace: remove the last character
            buffer[strlen(buffer) - 1] = '\0';
        } else if (event->key.keysym.sym == SDLK_RETURN) {
            
            // if input is enter, i want a newline
            // (does not render newline properly)
            if(strlen(buffer) + 1 < MAX_INPUT_LENGTH) {
                strcat(buffer, "\n");
            }
            //printf("Input: %s\n", buffer);
            //buffer[0] = '\0';
        }
    }
    // detects string input
    else if (event->type == SDL_TEXTINPUT) {
        // Append new text to inputText if it's not full
        if (strlen(buffer) + strlen(event->text.text) < MAX_INPUT_LENGTH) {
            strcat(buffer, event->text.text);
        }
    }


    return buffer;
}   