//
// Created by trist on 11/30/2024.
//
#include "buttonManager.h"

static struct button buttons[2];
static void extendArray(int length);

void createButton(struct button newButton,char* name, int row, int order, int corner, int width, int height)
{
    newButton.row = row;
    newButton.name = name;
    SDL_Rect rect;
     int windowX, windowY;
    SDL_GetWindowPosition(get_window(), &windowX, &windowY);
    // Adjust button position based on the window's top-left corner
    rect.x = corner + (windowX + (order * width));  // You can add or adjust `corner` to change its position
    rect.y = corner + (windowY);  // Similarly adjust y if needed
    rect.w = width;
    rect.h = height;

    newButton.rect = rect;
    newButton.order = order;
    buttons[order] = newButton;
}


void drawButtons()
{
    int i = 0;
    int length = sizeof(buttons) / sizeof(buttons[0]);
    
    // Load font once (ensure TTF_Init has been called)
    TTF_Font* font = TTF_OpenFont("TitilliumWeb-Regular.ttf", 20);  // Replace with your font path and size
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    while (i < length)
    {   if (buttons[i].name == NULL || strlen(buttons[i].name) == 0) {
            printf("Button name is empty or NULL\n");
            break;
        }

        // Draw button
        SDL_SetRenderDrawColor(get_renderer(), 0, 0, 0, 255);
        SDL_RenderFillRect(get_renderer(), &buttons[i].rect);

        // Set text color (white in this case)
        SDL_Color textColor = {255, 255, 255, 255};
        
        // Render text texture
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, buttons[i].name, textColor);
        if (textSurface)
        {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(get_renderer(), textSurface);
            SDL_FreeSurface(textSurface);

            if (textTexture)
            {
                // Get the text width and height
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
                
                // Set the position to center the text within the button
                SDL_Rect textRect = {
                    buttons[i].rect.x + (buttons[i].rect.w - textWidth) / 2,
                    buttons[i].rect.y + (buttons[i].rect.h - textHeight) / 2,
                    textWidth,
                    textHeight
                };

                // Render the text texture
                SDL_RenderCopy(get_renderer(), textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }

        SDL_SetRenderDrawColor(get_renderer(), 255, 255, 255, 255);
        SDL_RenderPresent(get_renderer());
        i++;
    }

    // Cleanup font (done outside the loop, so only once)
    TTF_CloseFont(font);
}


