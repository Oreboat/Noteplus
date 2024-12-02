#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

int isProcessRunning(char* app, HANDLE hSnapshot);
int detectProcess(char* apps[], int appCount);
