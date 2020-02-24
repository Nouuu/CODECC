#include <stdio.h>
#include <stdlib.h>

#include "gtkFunctions.h"
#include "functions.h"

char *gladeFile = "window.glade";


int main(int argc, char **argv) {
    // Generate a decimal to binary translation table
    fillC2B();

    // Launch the app and run the main loop until gtk_main_quit() is called
    startGTK(&argc, &argv, gladeFile);

    printf("Exit program\n");
}