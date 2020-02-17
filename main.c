#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "gtkFunctions.h"
#include "functions.h"

char *gladeFile = "../window.glade";


int main(int argc, char **argv) {
    int showConsole = 1;

/*
    HWND hWnd = GetConsoleWindow();
    if (showConsole) {
        ShowWindow(hWnd, SW_SHOW);
    } else {
        ShowWindow(hWnd, SW_HIDE);
    }
*/
    fillC2B();

    startGTK(&argc, &argv, gladeFile);

    printf("Exit program\n");
}