//
// Created by trist on 11/30/2024.
//

#ifndef BUTTON_H
#define BUTTON_H
#include "../common.h"
#include "../window/windowManager.h"
typedef struct button
{
    char* name;
    SDL_Rect rect;
    int row;
    int order;
}button;
void createButton(struct button newButton,char* name, int row, int order, int corner, int width, int height);

void drawButtons();
#endif //BUTTON_H
