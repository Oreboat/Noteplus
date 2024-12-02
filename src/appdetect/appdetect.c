#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

int isProcessRunning(char* app, HANDLE hSnapshot){
    PROCESSENTRY32 processEntry;

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0; // Error creating snapshot
    }

    processEntry.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &processEntry)) {
        do {
            if (strcmp(processEntry.szExeFile, app) == 0) {
                CloseHandle(hSnapshot);
                return 1; // Process is running
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }

    CloseHandle(hSnapshot);

    return 0;
}

// Detects whichever listed apps are running format of input should follow the example:
// "Notepad.exe, firefox.exe"
// This process may need to be altered for other OS as this has only been tested on Windows
int detectProcess(char* apps[], int appCount){
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Error creating snapshot\n");
        return 1;
    }

    for (int i = 0; i < appCount; i++) {
        if (!isProcessRunning(apps[i], hSnapshot)) {
            CloseHandle(hSnapshot);
            hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnapshot == INVALID_HANDLE_VALUE) {
                printf("Error creating snapshot\n");
                return -1;
            }
        }
        else {
            printf("Yay! The application '%s' is running.\n", apps[i]);
            return i;
            }
        //Sleep(1000);
    }
    
    CloseHandle(hSnapshot);
    return -2;
}




// int main(int argc, char* argv[]) {
//     char path[MAX_PATH];
//     printf("Enter the path of the executable(s) (e.g., Notepad.exe, firefox.exe): ");
//     fgets(path, sizeof(path), stdin);
//     path[strcspn(path, "\n")] = 0;  // Remove the newline character
    
//     // Split the input into multiple application names
//     char* token = strtok(path, ",");
//     char* apps[10];  // Array to hold up to 10 app names
//     int appCount = 0;

//     while (token != NULL && appCount < 10) {

//         // Add the application name to the array
//         apps[appCount++] = exeName;

//         // Get the next token
//         token = strtok(NULL, ",");
//     }

//     // Display the detected applications
//     printf("Detecting the following programs:\n");
//     for (int i = 0; i < appCount; i++) {
//         printf(" - %s\n", apps[i]);
//     }

//     // Detect if any of the specified applications are running
//     detectProcess(apps, appCount);

//     return 0;
// }
