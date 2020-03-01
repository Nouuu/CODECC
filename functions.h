#ifndef CODECC_FUNCTIONS_H
#define CODECC_FUNCTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    double size;
    char unit[3];
} fileSize;

// Decimal to byte array
char c2b[256][8];

void fillC2B();

unsigned char b2C(const char *value);

fileSize readableFileSize(long double size);

#endif //CODECC_FUNCTIONS_H
