#ifndef NOTEIO_H
#define NOTEIO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"


/* Written by Kenyon Tiner*/

/*
    -Main note saving function. The function has two modes:
    -Mode 1: This is for saving a note in the notes/ directory
    -Mode 2: Specifically for saving a note to a specific noteset

    -The function takes filename, content buffer, and mode.
    -Contructs the path for the current note, opens the file and loads the content of the content buffer
    into the file using fwrite.

    -For saving to a noteset, file path construction is handled by the save_to_note_set
    function, therefore save_note only opens up the path provided and fwrites to the file.

*/
int save_file(const char *filename, const char *content, int mode);

/* 
    -Opens specified note and render it
    -Takes the notes_dir path, desired file, content buffer, and the size of the buffer for memory safety
    -Contructs the proper filepath for opening, then calls the load_file function to load contents of the file into
    a rendering buffer to be displayed.
*/
int open_note(const char *notes_dir, const char *filename, char *buffer, size_t buffer_size);

/*
    -Clears the buffer on screen by using memset.
    -Takes content buffer and size for memory safety
*/
void new_note(char *buffer, size_t buffer_size);

/*
    -Creates a new noteset under the notes/notesets directory
    -Takes a noteset name
*/
int create_noteset(const char *noteset_name);

/*
    -Saves the current note to the specified noteset.
    -Takes a noteset name, filename, and content buffer.
    -contructs a full path of the form notes/notesets/<notesetname>/<filename>
    -calls savefile in mode 2
*/
int save_to_noteset(const char *noteset_name, const char *filename, const char *content);

/*
    -Function that displays a list of the desired noteset and waits for uer to select a file from the list
    then calls open_note on the file
    -takes the notes_dir, noteset_name, buffer, and size of buffer
    -using an open directory, populates a lis of files within the noteset and passes that list as a message to prompt user.
    Using the user input, open the selected file
*/
int open_noteset(const char *notes_dir, const char *noteset_name, char *buffer, size_t buffer_size);


/*
    -Function that opens the filepath and reads the bytes into a buffer
    -returns the buffer for use in other functions
*/
char* load_file(const char *filepath);

/*
    Main user input function
    -Takes a message that will be displayed to the user in a new window
    -Returns the users input for use in other functions
*/
char* prompt_user(const char* message);

/*
    Necessary rewritten version of render_input that allows for the use of a second renderer
    for the prompt user function.
*/
int render_input_to_renderer(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color* color);
#endif
