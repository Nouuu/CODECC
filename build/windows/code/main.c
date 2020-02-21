#include <stdio.h>
#include <stdlib.h>

#include "gtkFunctions.h"
#include "functions.h"

char *gladeFile = "window.glade";


int main(int argc, char **argv) {
    fillC2B();

    startGTK(&argc, &argv, gladeFile);

    printf("Exit program\n");
}