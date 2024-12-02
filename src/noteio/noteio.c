#include "noteio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/dirent/dirent-1.23.1/include/dirent.h"
#include "../input/input.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
/* Written by Kenyon Tiner */

#define MAX_FILE_SIZE 10000

// Variables used for tracking when a program opens
// key - app path 
// value - noteset

#ifdef _WIN32
    int isUNIX = 1;
#endif
int paths_size = 0;
char *paths[INT16_MAX];
char *notesets[INT16_MAX];

// Functions
// char* load_file(const char *filename);
// int save_file(const char *filename, const char *content);
// int open_file(const char *filename, char *buffer);
// void new_note(char *buffer);

// load content of file into buffer
int save_file(const char *filename, const char *content, int mode){
    
    char filepath[256];
    struct stat st = {0};

    /* if directly saving to note directory */
    if (mode == 1) {
        const char *notes_dir = "notes/";
    
    
        //ensure notes/ exists
        if (stat(notes_dir, &st) == -1) {
            if (mkdir(notes_dir) != 0) {
                perror("Failed to create notes directory");
                return -1;
            }
        }

        //construct path for file
        snprintf(filepath, sizeof(filepath), "%s%s", notes_dir, filename);

        FILE *file = fopen(filepath, "w+");
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
    if (mode == 2) {
        const char *notes_dir = "";

        snprintf(filepath, sizeof(filepath), "%s%s", notes_dir, filename);

        FILE *file = fopen(filepath, "w+");
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
}

int open_note(const char *notes_dir, const char *filename, char *buffer, size_t buffer_size) {
    if (notes_dir == NULL || filename == NULL || buffer == NULL) {
        fprintf(stderr, "Invalid arguments to open_note\n");
        return -1;
    }

    char filepath[MAX_FILE_SIZE];
    snprintf(filepath, sizeof(filepath), "%s/%s", notes_dir, filename);

    char *file_content = load_file(filepath);
    if (file_content == NULL) {
        return -1;  // Error already logged by load_file
    }

    // Copy file content to the provided buffer
    strncpy(buffer, file_content, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';  // Ensure null-termination

    free(file_content);
    return 0;  // Success
}

void new_note(char *buffer, size_t buffer_size) {
   if (buffer != NULL) {
        memset(buffer, 0, buffer_size);  // Safely clear the entire buffer
    }
}

int create_noteset(const char *noteset_name, const char *app_path) {
    char noteset_path[256];

    //construct noteset path
    snprintf(noteset_path, sizeof(noteset_path), "notes/notesets/%s", noteset_name);

    //check if noteset is created, if not create it
    struct stat st = {0};
    if (stat(noteset_path, &st) == -1) {
        if (mkdir("notes/notesets") != 0) {
            printf("Noteset File Exists!\n");
        }
        if (mkdir(noteset_path) != 0) {
            perror("Failed to create noteset!");
            return -1;
        }
        //if a path is specified add that path to detectable programs along with the associated noteset
        if (app_path != NULL) {
            char* trimmed_path = trim_path(app_path);
            if (path_cmp(trimmed_path)) {add_path(noteset_name ,trimmed_path);}
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
    struct stat st = {0};
    
    if(stat("notes", &st) == -1) {
        if (mkdir("notes") != 0 && errno != EEXIST) {
            perror("Failed to create notes directory");
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'notes' exists but is not a directory");
    }

    
    if (stat("notes/notesets", &st) == -1) {
        if (mkdir("notes/notesets") != 0 && errno != EEXIST) {
            perror("Failed to create notes/notesets directory");
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'notes/notesets' exists but is not a directory");
    }
    
    //check if the noteset exists
    if (create_noteset(noteset_name, NULL) != 0) {
        return -1;
    }

    snprintf(filepath, sizeof(filepath), "notes/notesets/%s", noteset_name);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath) != 0 && errno != EEXIST) {
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

int open_noteset(const char *notes_dir, const char *noteset_name, char *buffer, size_t buffer_size) {
    if (notes_dir == NULL || noteset_name == NULL || buffer == NULL) {
        fprintf(stderr, "Invalid arguments to open_noteset\n");
        return -1;
    }

    // Construct the path to the noteset directory
    char noteset_path[MAX_FILE_SIZE];
    snprintf(noteset_path, sizeof(noteset_path), "%s/notesets/%s", notes_dir, noteset_name);

    // Open the directory containing the noteset files
    DIR *dir = opendir(noteset_path);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open noteset directory: %s\n", strerror(errno));
        return -1;
    }

    // Prepare an array to hold file names
    char *file_names[10];  
    int file_count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip directories (.) and (..)
        if (entry->d_type == DT_REG) {
            file_names[file_count] = strdup(entry->d_name);
            file_count++;
            if (file_count >= 10) break;  // Avoid buffer overflow
        }
    }
    closedir(dir);

    // Display the list of files for the user to choose from
    char message[MAX_INPUT_LENGTH * 10] = "Enter the number of the note to open:\n";
    for (int i = 0; i < file_count; i++) {
        snprintf(message + strlen(message), sizeof(message) - strlen(message), "%d. %s\n", i + 1, file_names[i]);
    }

    // Get the user's choice via prompt_user
    char *input = prompt_user(message);
    if (input == NULL) {
        fprintf(stderr, "Failed to get user input.\n");
        return -1;
    }

    // Parse user input to select the file
    int selected_file = atoi(input) - 1;
    free(input);  // Free the dynamically allocated string returned by prompt_user

    if (selected_file < 0 || selected_file >= file_count) {
        fprintf(stderr, "Invalid selection.\n");
        return -1;
    }

    // Open the selected file
    if (open_note(noteset_path, file_names[selected_file], buffer, buffer_size) != 0) {
        fprintf(stderr, "Failed to open note.\n");
        return -1;
    }

    // Clean up
    for (int i = 0; i < file_count; i++) {
        free(file_names[i]);
    }

    return 0;  // Success
}

char *load_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", filepath, strerror(errno));
        return NULL;
    }

    // Allocate memory for file content
    char *content = (char *)malloc(MAX_FILE_SIZE);
    if (content == NULL) {
        fprintf(stderr, "Error allocating memory for file content\n");
        fclose(file);
        return NULL;
    }

    // Read file into the buffer
    size_t bytesRead = fread(content, 1, MAX_FILE_SIZE - 1, file);
    if (ferror(file)) {
        fprintf(stderr, "Error reading file %s\n", filepath);
        free(content);
        fclose(file);
        return NULL;
    }

    content[bytesRead] = '\0';  // Null-terminate the string
    fclose(file);
    return content;
}

char* prompt_user(const char* message) {
    static char input_buffer[MAX_INPUT_LENGTH] = "";
    char render_buf[MAX_INPUT_LENGTH * 10] = "";
    SDL_Event event;
    uint8_t running = 1;

    //create small pop up window for the prompt
    SDL_Window* prompt_window = SDL_CreateWindow(
        "Input Needed!", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 
        400, 400, SDL_WINDOW_SHOWN
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

// ------------------------------ App Detect Handling --------------------------------

int getIndex(const char *key) 
{ 
    for (int i = 0; i < paths_size; i++) { 
        if (strcmp(paths[i], key) == 0) { 
            return i; 
        } 
    } 
    return -1; // Key not found 
} 

int path_cmp(const char *app_path){
    for (int i = 0; i < paths_size; i++){
        if (strcmp(app_path, paths[i]) == 0) {return 0;}
    }
    return 1;
}

char* trim_path(const char *app_path) {
    // Trim any spaces around the application name
    char* exeName = app_path;
    while (*exeName == ' ') exeName++;  // Skip leading spaces
    char* end = exeName + strlen(exeName) - 1;
    while (end > exeName && (*end == ' ' || *end == '\n')) end--;  // Trim trailing spaces
        
    // Null-terminate the string after trimming spaces
    *(end + 1) = '\0';

    return exeName;
}

int add_path(const char *noteset_name, const char *app_path){
    int index = getIndex(app_path); 
    if (index == -1) { // Key not found 
        strcpy(paths[paths_size], app_path); 
        notesets[paths_size] = noteset_name; 
        paths_size++; 
    } 
    else { // Key found 
        notesets[index] = noteset_name; 
    } 

    save_path_file();

    return 0;
}

int remove_path(const char *app_path){
    int index = getIndex(app_path);
    int offset = 0;
    char* new_path_arr[INT16_MAX];
    char* new_noteS_arr[INT16_MAX];
    for (int i = 0; i < paths_size && offset < paths_size; i++){
        if (i == index){
            offset++;
        }
        if (offset < paths_size) {
            new_path_arr[i] = paths[i + offset];
            new_noteS_arr[i] = notesets[i + offset];
        }
    }
    
    //set both array to NULL before appending them
    for (int i = 0; i < paths_size; i++) { paths[i] = '\0';}
    for (int i = 0; i < paths_size; i++) { notesets[i] = '\0';}
    paths_size--;
    for (int i = 0; i < paths_size; i++) {paths[i] = new_path_arr[i];}
    for (int i = 0; i < paths_size; i++) {notesets[i] = new_noteS_arr[i];}    

    return 0;
}

int save_path_file(){
    const char *notes_dir = "notes/";
    char filepath[256];

    //ensure notes dir exists
    struct stat st = {0};
    if (stat(notes_dir, &st) == -1) {
        if (mkdir(notes_dir) != 0) {
            perror("Failed to create notes directory");
            return -1;
        }
    }

    // construct content to be written to file
    char *size;
    sprintf(size, "%d", paths_size);
    char *content = size;
    strcat(content,"\n");

    for (int i = 0; i < paths_size; i++){
        strcat(content,paths[i]);
        strcat(content," ");
    }
    strcat(content,"\n");
    for (int i = 0; i < paths_size; i++){
        strcat(content,notesets[i]);
        strcat(content," ");
    }

        //construct path for file
    snprintf(filepath, sizeof(filepath), "%s%s", notes_dir, "appdetect");

    FILE *file = fopen(filepath, "w+");
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
    printf("appdetect File successfully saved to %s\n", filepath);

    return 0;
}

int load_path_vars() {
     FILE *file = fopen("notes/appdetect", "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return -1;
    }

    // memory allocation for content of file
    char *buffer = (char *)malloc(MAX_FILE_SIZE);
    if (buffer == NULL) {
        perror("Error allocating memory for file content");
        fclose(file);
    }

    // read file into buffer
    size_t bytesRead = fread(buffer, sizeof(char), MAX_FILE_SIZE - 1, file);
    if (bytesRead == 0 && ferror(file)) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
    }

    // Null-terminate buffer
    buffer[bytesRead] = '\0';

    // Close file and return buffer to pass for rendering
    fclose(file);
    
    //with buffer now read the file will now allocate its contents into its respective variables
    buffer;


    return 0;
}

