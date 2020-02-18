//
// Created by Unknow on 16/02/2020.
//

#ifndef CODECC_CODECFUNCTIONS_H
#define CODECC_CODECFUNCTIONS_H

#include <stdlib.h>
#include <io.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>


char codecKey[4][8];
unsigned char encodeMatrix[256][2];
unsigned char decodeMatrix[256][256];


int readKey(const char *path);

int fillMatrixEncode();

int fillMatrixDecode();

int encode();

int decode();

void *worker(void *arg);


#endif //CODECC_CODECFUNCTIONS_H
