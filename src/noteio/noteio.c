#include "noteio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../input/input.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_FILE_SIZE 10000

// Functions
char* load_file(const char *filename);
int save_file(const char *filename, const char *content, int mode);
int open_note(const char *filename, char *buffer);
void new_note(char *buffer);
char* prompt_user(const char* message);

//Save current buffer as file
int save_file(const char *filename, const char *content, int mode){
    
    char filepath[256];
    struct stat st = {0};

    /* if directly saving to note directory */
    if (mode == 1) {
        const char *notes_dir = "notes/";
    
    
        //ensure notes/ exists
        if (stat(notes_dir, &st) == -1) {
            if (mkdir(notes_dir, 0700) != 0 && errno != EEXIST) {
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

    /* if called by save to noteset*/
    
        
    FILE *file = fopen(filename, "w");
    if (file == NULL){
        printf("%s\n", filename);
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
    printf("File successfully saved to %s\n", filename);
    
    return 0;
    
}

// clear text buffer, creating blank slate (new note)
void new_note(char *buffer) {
    //save current file first
    buffer[0] = '\0';
}

/*int create_noteset(const char *noteset_name) {
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
*/

int save_to_noteset(const char *noteset_name, const char *filename, const char *content){
    char filepath[512];
    struct stat st = {0};
    
    if(stat("notes", &st) == -1) {
        if (mkdir("notes", 0700) != 0 && errno != EEXIST) {
            perror("Failed to create notes directory");
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'notes' exists but is not a directory");
    }

    if (stat("notes/notesets", &st == -1)) {
        if (mkdir("notes/notesets", 0700) != 0 && errno != EEXIST) {
            perror("Failed to create notes/notesets directory");
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'notes/notesets' exists but is not a directory");
    }

    snprintf(filepath, sizeof(filepath), "notes/notesets/%s", noteset_name);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath, 0700) != 0 && errno != EEXIST) {
            perror("Failed to create noteset directory");
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'%s' exists but is not a directory", filepath);
    }

    strncat(filepath, "/", sizeof(filepath) - strlen(filepath) - 1);
    strncat(filepath, filename, sizeof(filepath) - strlen(filepath) - 1);

    //save the file
    save_file(filepath, content, 2);
    return 0;
}

char* prompt_user(const char* message) {
    static char input_buffer[MAX_INPUT_LENGTH] = "";
    char render_buf[MAX_INPUT_LENGTH] = "";
    SDL_Event event;
    uint8_t running = 1;

    //create small pop up window for the prompt
    SDL_Window* prompt_window = SDL_CreateWindow(
        "Input Needed!", 
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
        snprintf(render_buf, sizeof(render_buf), "->%s", input_buffer);
        if (!render_input_to_renderer(prompt_renderer, font, render_buf, 10, 10 + font_height + 5, &color)) {
            fprintf(stderr, "Failed to render filename");
        }
        
        SDL_RenderPresent(prompt_renderer);

        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }

            get_input(&event, input_buffer);

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                running = 0;
            }
        }
    }

    SDL_StopTextInput();
    SDL_DestroyRenderer(prompt_renderer);
    SDL_DestroyWindow(prompt_window);

    size_t length = strlen(input_buffer);
    if (length > 0 && input_buffer[length - 1] == '\n') {
        input_buffer[length - 1] = '\0';
    }

    char* result = malloc(strlen(input_buffer) + 1);
    if(result) {
        strcpy(result, input_buffer);
    }
    return result;
}

//input rendering for secondary renderere
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

// Function to load file contents
char* load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char *content = (char *)malloc(MAX_FILE_SIZE);
    if (content == NULL) {
        perror("Error allocating memory for file content");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(content, sizeof(char), MAX_FILE_SIZE - 1, file);
    if (bytesRead == 0 && ferror(file)) {
        perror("Error reading file");
        free(content);
        fclose(file);
        return NULL;
    }

    content[bytesRead] = '\0';  // Null-terminate the loaded content
    fclose(file);
    return content;
}

// Function to open a note
int open_note(const char *filename, char *buffer) {
    
    if (filename == NULL || buffer == NULL) {
        fprintf(stderr, "Invalid filename or buffer.\n");
        return -1;
    }

    // Clear the current buffer content
    new_note(buffer);

    fprintf("%s\n", filename);
    // Load file content
    char *file_content = load_file(filename);
    if (file_content == NULL) {
        return -1;  // Error loading file
    }

    // Copy the content into the provided buffer
    strncpy(buffer, file_content, MAX_FILE_SIZE - 1);
    buffer[MAX_FILE_SIZE - 1] = '\0';  // Ensure null-termination

    // Free the allocated memory for file content
    free(file_content);

    printf("Note successfully loaded from %s\n", filename);
    return 0;
}