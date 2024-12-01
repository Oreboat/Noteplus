#include "windowManager.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* font = NULL;
static SDL_Color color = {
    255,
    255,
    255
};

int initialize_window(void)
{
    SDL_GLContext mainContext = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        puts("Failed to init SDL.");
        SDL_GetError();
        return 0;
    }

    if (TTF_Init() < 0) {
        puts("Failed to init TTF.");
        TTF_GetError();
        return 0;
    }

    // Query display resolution
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        puts("Failed to get display mode.");
        SDL_GetError();
        return 0;
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;

    // Dynamically calculate window size
    int width = screenWidth / 4;       // One-third of screen width
    int height = (screenHeight * 2) / 4; // Two-thirds of screen height

    // Create the window in the top-left corner
    window = SDL_CreateWindow(
        "NotePlus",
        0,    // Top-left corner x position
        0,    // Top-left corner y position
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        puts("Failed to create SDL window.");
        SDL_GetError();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        puts("Failed to create SDL renderer.");
        SDL_GetError();
        return 0;
    }

    font = TTF_OpenFont("TitilliumWeb-Regular.ttf", 14);
    if (font == NULL) {
        puts("Font not opened.");
        return 0;
    }

    TTF_SetFontSize(font, 14);
    TTF_SetFontStyle(font, 0); // Regular style

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );

    mainContext = SDL_GL_CreateContext(window);
    if (!mainContext) {
        puts("Failed to create OpenGL context.");
        SDL_GetError();
        return 0;
    }

    // Uncomment if using GLAD
    // gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

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
