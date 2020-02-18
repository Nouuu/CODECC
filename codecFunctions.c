//
// Created by Unknow on 16/02/2020.
//

#include "codecFunctions.h"
#include "gtkFunctions.h"
#include "functions.h"

bool codecKeyLoaded = FALSE;
extern char *filePath;

int readKey(const char *path) {
    int i;
    char c1[9], c2[9], c3[9], c4[9];
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        updateStatus("Cannot open key file !");
        return 1;
    }
    if (fscanf(fp, "G4C=[%[01] %[01] %[01] %[01]]", c1, c2, c3, c4) != 4) {
        updateStatus("Incorrect key file !");
        fclose(fp);
        return 1;
    }
    if (strlen(c1) != 8 || strlen(c2) != 8 || strlen(c3) != 8 || strlen(c4) != 8) {
        updateStatus("Incorrect key file !");
        fclose(fp);
        return 1;
    }

    for (i = 0; i < 8; ++i) {
        codecKey[0][i] = c1[i] == '1' ? 1 : 0;
        codecKey[1][i] = c2[i] == '1' ? 1 : 0;
        codecKey[2][i] = c3[i] == '1' ? 1 : 0;
        codecKey[3][i] = c4[i] == '1' ? 1 : 0;
    }


    fclose(fp);
    return fillMatrixEncode() || fillMatrixDecode();
}

int fillMatrixEncode() {
    char array1[8], array2[8];
    int i, j;


    for (i = 0; i < 256; ++i) {
        //c2b[i];
        for (j = 0; j < 8; ++j) {
            array1[j] = (c2b[i][0] && codecKey[0][j]) ^ (c2b[i][1] && codecKey[1][j]) ^ (c2b[i][2] && codecKey[2][j]) ^
                        (c2b[i][3] && codecKey[3][j]);
            array2[j] = (c2b[i][4] && codecKey[0][j]) ^ (c2b[i][5] && codecKey[1][j]) ^ (c2b[i][6] && codecKey[2][j]) ^
                        (c2b[i][7] && codecKey[3][j]);
        }
        encodeMatrix[i][0] = b2C(array1);
        encodeMatrix[i][1] = b2C(array2);
    }

    return 0;
}

int fillMatrixDecode() {
    setSpinnerStatus(TRUE);
    levelBarSetValue(0.);
    int i, j, k;
    char i4[5], matrixI4[4], byte[8];

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            i4[j] = codecKey[j][i];
        }
        i4[4] = '\0';

        if (i4[0] == 1 && i4[1] == 0 && i4[2] == 0 && i4[3] == 0)
            matrixI4[0] = i;
        else if (i4[0] == 0 && i4[1] == 1 && i4[2] == 0 && i4[3] == 0)
            matrixI4[1] = i;
        else if (i4[0] == 0 && i4[1] == 0 && i4[2] == 1 && i4[3] == 0)
            matrixI4[2] = i;
        else if (i4[0] == 0 && i4[1] == 0 && i4[2] == 0 && i4[3] == 1)
            matrixI4[3] = i;
    }

    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            for (k = 0; k < 4; k++) {
                byte[k] = c2b[i][matrixI4[k]];
            }
            for (k = 0; k < 4; k++) {
                byte[k + 4] = c2b[j][matrixI4[k]];
            }
            decodeMatrix[i][j] = b2C(byte);
        }
        levelBarSetValue((i + 256.) / 512.);
    }
    return 0;
}

int encode() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    if (filePath == NULL || strlen(filePath) == 0) {
        updateStatus("File input empty !");
        return 1;
    }
    if (!codecKeyLoaded) {
        updateStatus("Key not loaded !");
        return 1;
    }
    char *destPath = malloc(strlen(filePath) + 2);
    strcat(strcpy(destPath, filePath), "e");

    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        updateStatus("Can't open file !");
        return 1;
    }
    FILE *dest = fopen(destPath, "wb");
    if (dest == NULL) {
        updateStatus("Can't create new file !");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = _ftelli64(fp);
    fseek(fp, 0, SEEK_SET);
    size_t size2 = size;
    unsigned char *readBuffer;
    unsigned char *writeBuffer;
    int i;

    if (size2 > 1048576) {
        readBuffer = malloc(1048576);
        writeBuffer = malloc(2097152);
        while (size2 > 1048576) {
            size2 -= 1048576;
            fread(readBuffer, sizeof(char), 1048576, fp);
            for (i = 0; i < 1048576; i++) {
                writeBuffer[i * 2] = encodeMatrix[readBuffer[i]][0];
                writeBuffer[i * 2 + 1] = encodeMatrix[readBuffer[i]][1];
            }
            fwrite(writeBuffer, sizeof(char), 2097152, dest);
        }
        free(readBuffer);
        free(writeBuffer);
    }

    if (size2 > 1024) {
        readBuffer = malloc(1024);
        writeBuffer = malloc(2048);
        while (size2 > 1024) {
            size2 -= 1024;
            fread(readBuffer, sizeof(char), 1024, fp);
            for (i = 0; i < 1024; i++) {
                writeBuffer[i * 2] = encodeMatrix[readBuffer[i]][0];
                writeBuffer[i * 2 + 1] = encodeMatrix[readBuffer[i]][1];
            }
            fwrite(writeBuffer, sizeof(char), 2048, dest);
        }
        free(readBuffer);
        free(writeBuffer);
    }

    int byte;
    while ((byte = fgetc(fp)) != EOF) {
        fputc(encodeMatrix[byte][0], dest);
        fputc(encodeMatrix[byte][1], dest);
    }


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fileSize fileSize1 = readableFileSize(size);
    fileSize fileSize2 = readableFileSize(size * 2);
    char message[255];
    sprintf(message, "File encoded ! Encoding time: %lf seconds, source size : %.2lf %s, dest size : %.2lf %s",
            cpu_time_used,
            fileSize1.size, fileSize1.unit, fileSize2.size, fileSize2.unit);
    updateStatus(message);
    levelBarSetValue(1.);
    fclose(fp);
    fclose(dest);
    return 0;
}