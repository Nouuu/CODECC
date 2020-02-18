//
// Created by Unknow on 16/02/2020.
//

#include "functions.h"
#include "gtkFunctions.h"

void fillC2B() {
    int i;
    unsigned char j, byte;

    for (i = 0; i < 256; i++) {
        byte = i;
        j = 8;
        do {
            c2b[i][--j] = byte % 2;
            byte /= 2;
        } while (j);
    }
}

unsigned char b2C(const char *value) {
    unsigned char result = 0;
    char i;
    for (i = 0; i < 8; i++) {
        result += value[7 - i] * pow(2, i);
    }
    return result;
}

fileSize readableFileSize(long double size) {
    int i = 0;
    char units[9][3] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    fileSize fileSize1;
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    fileSize1.size = (double) size;
    strcpy(fileSize1.unit, units[i]);
    return fileSize1;
}