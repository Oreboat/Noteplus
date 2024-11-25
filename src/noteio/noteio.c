#include "noteio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../input/input.h"
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILE_SIZE 10000

// Functions
char* load_file(const char *filename);
int save_file(const char *filename, const char *content);
int open_file(const char *filename, char *buffer);
void new_note(char *buffer);

// load content of file into buffer
char* load_file(const char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }

    // memory allocation for content of file
    char *buffer = (char *)malloc(MAX_FILE_SIZE);
    if (buffer == NULL) {
        perror("Error allocating memory for file content");
        fclose(file);
        return NULL;
    }

    // read file into buffer
    size_t bytesRead = fread(buffer, sizeof(char), MAX_FILE_SIZE - 1, file);
    if (bytesRead == 0 && ferror(file)) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate buffer
    buffer[bytesRead] = '\0';

    // Close file and return buffer to pass for rendering
    fclose(file);
    return buffer;
}

//Save current buffer as file
int save_file(const char *filename, const char *content){

    const char *notes_dir = "notes/";
    char filepath[256];

    //ensure notes/ exists
    struct stat st = {0};
    if (stat(notes_dir, &st) == -1) {
        if (mkdir(notes_dir, 0700) != 0) {
            perror("Failed to create notes directory");
            return -1;
        }
    }

    //construct path for file
    snprintf(filepath, sizeof(filepath), "%s%s", notes_dir, filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL){
        perror("Error opening file for writing");
        return -1;
    }

    //write content to file
    size_t contentLength = strlen(content);
    size_t bytesWritten = fwrite(content, sizeof(char), contentLength, file);
    if (bytesWritten != contentLength){
        perror("Buffer/Write missmatch, could not save.");
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("File successfully saved to %s\n", filepath);
    return 0;
}

//Open note
int open_note(const char *filename, char *buffer){
    char *file_content = load_file(filename);
    if (file_content != NULL) {
        strncpy(buffer, file_content, MAX_FILE_SIZE -1);
        free(file_content);
        return 0;
    }
    return -1;
}

// clear text buffer, creating blank slate (new note)
void new_note(char *buffer) {
    buffer[0] = '\0';
}

int create_noteset(const char *noteset_name) {
    char noteset_path[256];

    //construct noteset path
    snprintf(noteset_path, sizeof(noteset_path), "notes/notesets/%s", noteset_name);

    //check if noteset is created, if not create it
    struct stat st = {0};
    if (stat(noteset_path, &st) == -1) {
        if (mkdir(noteset_path, 0700) != 0) {
            perror("Failed to create noteset!");
            return -1;
        }

        printf("Noteset '%s' created successfully.\n", noteset_name);
    }
    else {
        printf("Noteset '%s' already exists.\n", noteset_name);
    }
    return 0; 
}

int save_to_noteset(const char *noteset_name, const char *filename, const char *content){
    char filepath[512];
    //construct filepath to save to
    snprintf(filepath, sizeof(filepath), "notes/%s/%s", noteset_name, filename);

    //check if the noteset exists
    if (create_noteset(noteset_name) != 0) {
        return -1;
    }
    //save the file
    return save_file(filepath, content);
}

char* prompt_filename(const char* message) {
    static char filename[MAX_INPUT_LENGTH] = "";
    char render_buf[MAX_INPUT_LENGTH] = "";
    SDL_Event event;
    uint8_t running = 1;

    //create small pop up window for the prompt
    SDL_Window* prompt_window = SDL_CreateWindow(
        "Save File", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 
        400, 200, SDL_WINDOW_SHOWN
    );
    if (!prompt_window) {
        fprintf(stderr, "Failed to create prompt window: %s\n", SDL_GetError());
        return NULL;
    }

    //create separate renderer for saving prompt
    SDL_Renderer* prompt_renderer = SDL_CreateRenderer(prompt_window, -1, SDL_RENDERER_ACCELERATED);
    if (!prompt_renderer) {
        fprintf(stderr, "Failed to create renderer for prompt window: %s\n", SDL_GetError());
        SDL_DestroyWindow(prompt_window);
        return NULL;
    }

    //start text
    SDL_StartTextInput();

    TTF_Font* font = get_font();
    if(!font){
        fprintf(stderr, "Font not initialized.\n");
        SDL_DestroyRenderer(prompt_renderer);
        SDL_DestroyWindow(prompt_window);
        return NULL;
    }

    int font_height = TTF_FontHeight(font);
    SDL_Color color = {255,255,255,255};

    while (running) {
        SDL_SetRenderDrawColor(prompt_renderer, 45, 45, 45, 255);
        SDL_RenderClear(prompt_renderer);

        //user render_input_to_renderer to force the use of prompt renderer
        if (!render_input_to_renderer(prompt_renderer, font, message, 10, 10, &color)){
            fprintf(stderr, "Failed to render message");
        }

        //get input for rendering
        snprintf(render_buf, sizeof(render_buf), "->%s", filename);
        if (!render_input_to_renderer(prompt_renderer, font, render_buf, 10, 10 + font_height + 5, &color)) {
            fprintf(stderr, "Failed to render filename");
        }
        
        SDL_RenderPresent(prompt_renderer);

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }

            get_input(&event, filename);

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                running = 0;
            }
        }
    }

    SDL_StopTextInput();
    SDL_DestroyRenderer(prompt_renderer);
    SDL_DestroyWindow(prompt_window);

    return filename;

}

//forces usage of prompt render
int render_input_to_renderer(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color* color) {
    if (!renderer || !font || !text || !color) {
        fprintf(stderr, "Invalid arguments for rendering input.\n");
        return 0;
    }

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, *color);
    if (!text_surface) {
        fprintf(stderr, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture){
        fprintf(stderr, "Unable to create texture from rendered tet! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return 0;
    }

    SDL_Rect renderQuad = {x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, NULL, &renderQuad);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    return 1;
}