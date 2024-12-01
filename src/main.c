#include <stdio.h>
#include <string.h>
#include "window/windowManager.h"
#include "input/input.h"
#include "common.h"
#include "noteio/noteio.h"
#include "appdetect/appdetect.h"

/* getline() is POSIX-only, so here is a custom version. */
/* Returns bytes written, 0 on fail.                     */
size_t get_line(char dest_buf[], const char source_buf[], size_t len, size_t offset)
{
    if (!dest_buf || !source_buf || len == 0) 
    {
        puts("get_line: Invalid arguments.");
        return 0;
    }

    size_t i = 0;
    while (offset < len && source_buf[offset] != '\0' && source_buf[offset] != '\n') 
    {
        dest_buf[i++] = source_buf[offset++];
    }

    if (source_buf[offset] == '\n') 
    {
        dest_buf[i++] = '\n';  // Copy newline if present
        ++offset;
    }
    
    dest_buf[i] = '\0';  // Null-terminate the destination buffer
    return i;  // Return the number of characters written
}

int main(int argc, char* argv[]) 
{
    /* TODO: make a flag that represents wheter or not         */
    /* the input changed, and therefore needs to be redrawn.   */
    
    size_t offset, line_length;
    uint32_t start_time, frame_time;
    uint8_t save_flag, new_flag, open_flag,
            save_to_set_flag, create_set_flag, open_set_flag;
    uint32_t render_x, render_y;
    uint32_t newline_count;
    uint32_t font_height;
    TTF_Font* font;

    /* Init phase */
    int running = initialize_window();
    initialize_input();

    font = get_font();
    if (font == NULL)
    {
        puts("font failed in main.");
        return -1;
    }

    font_height = TTF_FontHeight(font); // magic number for proper alignment
    printf("Font height: %d\n", font_height);

    /* Buffer to store and check user input */
    char input_text[MAX_INPUT_LENGTH] = "";
    char render_buf[MAX_INPUT_LENGTH] = "";
    char save_buf[MAX_INPUT_LENGTH] = "";
    const char *filename;
    const char *noteset_name;
    SDL_Renderer *renderer = get_renderer();
    while (running) 
    {    
        start_time = SDL_GetTicks();
        save_flag = 0;
        new_flag = 0;
        save_to_set_flag = 0;
        open_flag = 0;
        open_set_flag = 0;
        create_set_flag = 0;

    
        /* SHORTCUTS:

            CTRL+S: Save a note to notes/
            CTRL+N: Create new note, saves first
            CTRL+O: Open a note
            CTRL+SHIFT+S: Save a note to noteset
            CTRL+SHIFT+O: Open a noteset
            CTRL+SHIFT+N: Create a new noteset

        */
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) 
            {
                running = 0;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                /* Check if CTRL+SHIFT+N is pressed for new note set*/
                if (event.key.keysym.sym == SDLK_n && 
                (event.key.keysym.mod & KMOD_CTRL) && 
                (event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl + Shift + N was pressed!\n");
                    create_set_flag = 1;
                }
                
                /* Check if CTRL+SHIFT+S is pressed for save current note to note set */
                if (event.key.keysym.sym == SDLK_s &&
                (event.key.keysym.mod & KMOD_CTRL) &&
                (event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl+ Shift + S was pressed!\n");
                    save_to_set_flag = 1;
                }
                /* Check if CTRL+SHIFT+O is pressed for opening a noteset */
                if (event.key.keysym.sym == SDLK_o &&
                    (event.key.keysym.mod & KMOD_CTRL) &&
                    (event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl + Shift + O was pressed!\n");
                    open_set_flag = 1;
                }
                
                /* Check if CRTL+S is pressed for save current note */
                if (event.key.keysym.sym == SDLK_s && 
                    (event.key.keysym.mod & KMOD_CTRL) 
                    && !(event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl + S was pressed!\n");
                    save_flag = 1;
                }

                /* Check if CTRL+N is pressed for new note */
                if (event.key.keysym.sym == SDLK_n &&
                    (event.key.keysym.mod & KMOD_CTRL) &&
                    !(event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl + N was pressed!\n");
                    new_flag = 1;
                }
                /* Check if CTRL+O is pressed for opening a note */
                if (event.key.keysym.sym == SDLK_o &&
                    (event.key.keysym.mod & KMOD_CTRL) &&
                    !(event.key.keysym.mod & KMOD_SHIFT)) {
                    printf("Ctrl + O was pressed!\n");
                    open_flag = 1;
                }
 
            } 
    
            /* get input unto input_text */
            get_input(&event, input_text);
        }
        /* For both saving a note to notes/ directory or to a specific noteset. Saving to noteset just adds an extra step */
        if(save_flag || save_to_set_flag)
        {
            /* Get noteset_namethen filename, call save_to_noteset */
            if (save_to_set_flag) {
                noteset_name = prompt_user("Enter Noteset Name: ");
                filename = prompt_user("Enter filename: ");
                
                if (noteset_name && filename) {
                    save_to_noteset(noteset_name, filename, input_text);
                    printf("Note (%s) successfully saved to Noteset (%s)", filename, noteset_name);
                }
                free((char*)noteset_name);
                free((char*)filename);
                save_to_set_flag = 0;
            }
            /* For saving and note to notes/ directory */
            else {
                filename = prompt_user("Enter Filename: ");
                if (filename && filename[0] != '\0') {
                    save_file(filename, input_text, 1);
                    printf("File saved successfully as: %s\n", filename);
                }
                free((char*)filename);
                save_flag = 0;
            }


            SDL_StopTextInput();
            SDL_StartTextInput();

        }
        /* If CTRL + N is pressed, called new_note, but prompt before wiping the buffer */
        if (new_flag) {
            if (strlen(input_text) > 0) {
                //prompt for filename
                filename = prompt_user("Enter Filename: ");
                //save
                if (filename && filename[0] != '\0') {
                    save_file(filename, input_text, 1);
                    //clear buffer
                    new_note(input_text, MAX_INPUT_LENGTH);
                }

            if(save_flag == 1)
            {
                /* If ctrl + s is hit, this will return a string to be saved to file. */
                strncpy(save_buf, input_text, MAX_INPUT_LENGTH - 1);
                save_buf[strlen(input_text)] = '\0';
                const char *filename = prompt_filename("Enter Filename: ");
                printf("Filename: %s\n", filename);
                save_file(filename, save_buf);
                //puts(save_buf);
                goto end;

            }
            else {new_note(input_text, MAX_INPUT_LENGTH);}
            
            new_flag = 0;
            SDL_StopTextInput();
            SDL_StartTextInput();
            continue;
        }
        /* If CTRL + O is pressed, call open note for specified note. */
        if (open_flag) {
            const char* notes_dir = "notes";
            filename = prompt_user("Enter filename to open: ");
            printf("Filename to open: '%s'\n", filename);
            open_note(notes_dir, filename, input_text, MAX_INPUT_LENGTH);
            //open_file(filename, input_text);
            //free((char*)filename);
            SDL_StopTextInput();
            SDL_StartTextInput();

            open_flag = 0;
        }

        /* If CTRL + SHIFT + O is pressed, call open_noteset to display list of files in desired noteset. */
        if (open_set_flag) {
            const char* notes_dir = "notes";
            noteset_name = prompt_user("Enter Noteset Name to open: ");
            open_noteset(notes_dir, noteset_name, input_text, MAX_INPUT_LENGTH);

            free((char*)noteset_name);

            SDL_StopTextInput();
            SDL_StartTextInput();

            open_set_flag = 0;
        }
        /* If CTRL+SHIFT+N is pressed, get notesetname, then call create_noteset to create a new noteset in the notesets directory */
        if (create_set_flag){
            const char* noteset_name;
            noteset_name = prompt_user("Enter name for New Noteset: ");
            create_noteset(noteset_name);
            free((char*)noteset_name);

            SDL_StopTextInput();
            SDL_StartTextInput();

            create_set_flag = 0;
        }

    
        
        /* Clear screen if empty input */
        if (strlen(input_text) == 0) 
        {
            SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            goto end;
        }

        /* Render screen one line at a time */
        render_x = 0;
        offset = 0;
        // Give room for File/Edit buttons
        render_y = font_height + 5;
        while (offset < strlen(input_text))
        {
            line_length = get_line(render_buf, input_text, strlen(input_text), offset);

            render_input(render_buf, render_x, render_y, get_color());

            render_y += font_height;
            
            offset += line_length;
        }

        /* Moved render_present out of render_input so it only renders once. */
        /* Flicker begone!                                                   */
        SDL_RenderPresent(renderer);

        /* Clear screen to render info for next frame*/
        SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
        SDL_RenderClear(renderer);
       
        end:
        frame_time = SDL_GetTicks() - start_time;
        if (frame_time < FRAME_DELAY) 
        {
            SDL_Delay(FRAME_DELAY - frame_time);  // Delay to match target frame time
        }

    }

    close_input();
    destroy_window();

    return 0;
}