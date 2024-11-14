#include "windowManager.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* font = NULL;
static SDL_Color color  = {
    255,
    255,
    255
};

int initialize_window(void) 
{
    int width = 768;
    int height = 432;
    SDL_GLContext mainContext = NULL;
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        puts("Failed to init SDL.");
        SDL_GetError();
        return 0;
    }

    if(TTF_Init() < 0) {
        puts("Failed to init TTF.");
        TTF_GetError();
        return 0;
    }

    

    /*
    int info = SDL_GetVideoInfo();
    if(!info) {
        puts("Failed Video Query.");
        return 0;
    }
    */

    window = SDL_CreateWindow(
        "NotePlus",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("TitilliumWeb-Regular.ttf", 14);
    if(font == NULL) 
    {
        puts("font not opened.");
        return 0;
    }

    TTF_SetFontSize(font, 14);
    TTF_SetFontStyle(font, 0); // regular style

/*
    if(TTF_FontFaceIsFixedWidth(font) == 0) {
        puts("Font width is not fixed.");
        return 0;
    }
*/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );

    mainContext = SDL_GL_CreateContext(window);
    //gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress); for glad, not glew
    
    return 1;
}

void destroy_window(void) 
{
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Renderer* get_renderer(void) 
{
    return renderer;
}

SDL_Window* get_window(void) 
{
    return window;
}

TTF_Font* get_font(void) 
{
    return font;
}

SDL_Color* get_color(void) 
{
    return &color;
}