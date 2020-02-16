//
// Created by Unknow on 16/02/2020.
//

#include "codecFunctions.h"
#include "gtkFunctions.h"
#include "functions.h"

int readKey(const char *path) {
    char c1[9], c2[9], c3[9], c4[9];
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        updateStatus("Cannot open key file !");
        return 1;
    }
    if (fscanf(fp, "G4C=[%[01] %[01] %[01] %[01]]", c1, c2, c3, c4) != 4) {
        updateStatus("Incorrect key file !");
        return 1;
    }
    return 0;
}