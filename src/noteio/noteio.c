#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 10000
#define MAX_INPUT_LENGTH 2048

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

    FILE *file = fopen(filename, "w");
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