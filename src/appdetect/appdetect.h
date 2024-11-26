#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

int isProcessRunning(const char* app, HANDLE hSnapshot);
int detectProcess(const char* apps[], int appCount);
