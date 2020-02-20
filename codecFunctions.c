//
// Created by Unknow on 16/02/2020.
//

#include "codecFunctions.h"
#include "gtkFunctions.h"
#include "functions.h"


bool codecKeyLoaded = FALSE;
extern char *filePath;
unsigned char *readBuffer = NULL;
size_t readBufferSize;
unsigned char *writeBuffer = NULL;
size_t writeBufferSize;
int ZERO = 0;
int ONE = 1;


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
        fclose(fp);
        return 1;
    }

//    int fileDS = fileno(fp);
//    int fileDD = fileno(dest);
    _fseeki64(fp, 0, SEEK_END);
    size_t size = _ftelli64(fp);
    _fseeki64(fp, 0, SEEK_SET);
    size_t size2 = size;
    pthread_t threads[2];

    if (size2 >= 10485760) {
        readBufferSize = 10485760;
        writeBufferSize = readBufferSize * 2;
        readBuffer = malloc(readBufferSize);
        writeBuffer = malloc(writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), 10485760, fp);
//            _read(fileDS, readBuffer, readBufferSize)
            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            /*if (fread(readBuffer, sizeof(char), 10485760, fp) != readBufferSize) {
                updateStatus("Error while encoding, on reading buffer 10485760");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }*/

            pthread_create(threads, NULL, worker, &ZERO);
            pthread_create(threads + 1, NULL, worker, &ONE);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);

//            fwrite(writeBuffer, sizeof(char), 20971520, dest);
//            _write(fileDD, writeBuffer, writeBufferSize)
            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);

            /*if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 10485760*2");
                fclose(fp);
                fclose(dest);
                return 1;
            }*/
        }

    }

    if (size2 >= 1048576) {
        readBufferSize = 1048576;
        writeBufferSize = readBufferSize * 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), readBufferSize, fp);
//            _read(fileDS, readBuffer, readBufferSize);
            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);
/*
            if (_read(fileDS, readBuffer, readBufferSize) != readBufferSize) {
                updateStatus("Error while encoding, on reading buffer 1048576");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
*/
            pthread_create(threads, NULL, worker, &ZERO);
            pthread_create(threads + 1, NULL, worker, &ONE);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
//            fwrite(writeBuffer, sizeof(char), 20971520, dest);
//            _write(fileDD, writeBuffer, writeBufferSize)
            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
/*
            if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 1048576*2");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
*/
        }
    }

    if (size2 >= 1024) {
        readBufferSize = 1024;
        writeBufferSize = readBufferSize * 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), readBufferSize, fp);
//            _read(fileDS, readBuffer, readBufferSize);
            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);
/*
            if (_read(fileDS, readBuffer, readBufferSize) != readBufferSize) {
                updateStatus("Error while encoding, on reading buffer 1024");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
*/

            pthread_create(threads, NULL, worker, &ZERO);
            pthread_create(threads + 1, NULL, worker, &ONE);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
//            fwrite(writeBuffer, sizeof(char), 20971520, dest);
//            _write(fileDD, writeBuffer, writeBufferSize)
            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
/*
            if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 1024*2");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
*/
        }
    }


    readBufferSize = 1;
    writeBufferSize = readBufferSize * 2;
    readBuffer = realloc(readBuffer, readBufferSize);
    writeBuffer = realloc(writeBuffer, writeBufferSize);

//    _read(fileDS, readBuffer, readBufferSize)
    while (fread(readBuffer, 1, readBufferSize, fp) == readBufferSize) {
        writeBuffer[0] = encodeMatrix[*readBuffer][0];
        writeBuffer[1] = encodeMatrix[*readBuffer][1];
        assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
/*
        if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
            updateStatus("Error while encoding, on writing buffer 1*2");
            fclose(fp);
            fclose(dest);
            free(readBuffer);
            free(writeBuffer);
            return 1;
        }
*/
    }

    free(readBuffer);
    free(writeBuffer);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fileSize fileSize1 = readableFileSize(size);
    fileSize fileSize2 = readableFileSize(_ftelli64(dest));
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

int decode() {
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
    strcat(strcpy(destPath, filePath), "d");

    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        updateStatus("Can't open file !");
        return 1;
    }
    FILE *dest = fopen(destPath, "wb");
    if (dest == NULL) {
        updateStatus("Can't create new file !");
        fclose(fp);
        return 1;
    }

    int fileDS = fileno(fp);
    int fileDD = fileno(dest);
    _lseeki64(fileDS, 0, SEEK_END);
    size_t size = _telli64(fileDS);
    _lseeki64(fileDS, 0, SEEK_SET);
    size_t size2 = size;
    int i;

    if (size2 > 20971520) {
        readBufferSize = 20971520;
        writeBufferSize = readBufferSize / 2;
        readBuffer = malloc(readBufferSize);
        writeBuffer = malloc(writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), readBufferSize, fp);
            if (_read(fileDS, readBuffer, readBufferSize) != readBufferSize) {
                updateStatus("Error while decoding, on reading buffer 20971520");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }
//            fwrite(writeBuffer, sizeof(char), 1048576, dest);
            if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 20971520/2");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
        }
        free(readBuffer);
        free(writeBuffer);
    }

    if (size2 > 2097152) {
        readBufferSize = 2097152;
        writeBufferSize = readBufferSize / 2;
        readBuffer = malloc(readBufferSize);
        writeBuffer = malloc(writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), readBufferSize, fp);
            if (_read(fileDS, readBuffer, readBufferSize) != readBufferSize) {
                updateStatus("Error while decoding, on reading buffer 2097152");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }
//            fwrite(writeBuffer, sizeof(char), 1048576, dest);
            if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 2097152/2");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
        }
        free(readBuffer);
        free(writeBuffer);
    }

    if (size2 > 2048) {
        readBufferSize = 2048;
        writeBufferSize = readBufferSize / 2;
        readBuffer = malloc(readBufferSize);
        writeBuffer = malloc(writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;
//            fread(readBuffer, sizeof(char), readBufferSize, fp);
            if (_read(fileDS, readBuffer, readBufferSize) != readBufferSize) {
                updateStatus("Error while decoding, on reading buffer 2048");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }
//            fwrite(writeBuffer, sizeof(char), 1048576, dest);
            if (_write(fileDD, writeBuffer, writeBufferSize) != writeBufferSize) {
                updateStatus("Error while encoding, on writing buffer 2048/2");
                fclose(fp);
                fclose(dest);
                free(readBuffer);
                free(writeBuffer);
                return 1;
            }
        }
        free(readBuffer);
        free(writeBuffer);
    }


    readBufferSize = 2;
    readBuffer = malloc(readBufferSize);

    while (_read(fileDS, readBuffer, readBufferSize) == readBufferSize) {
        if (_write(fileDD, &decodeMatrix[readBuffer[0]][readBuffer[1]], readBufferSize / 2) != readBufferSize / 2) {
            updateStatus("Error while encoding, on writing buffer 1*2");
            fclose(fp);
            fclose(dest);
            free(readBuffer);
            return 1;
        }
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fileSize fileSize1 = readableFileSize(size);
    fileSize fileSize2 = readableFileSize(_telli64(fileDD));
    char message[255];
    sprintf(message, "File decoded ! Decoding time: %lf seconds, source size : %.2lf %s, dest size : %.2lf %s",
            cpu_time_used,
            fileSize1.size, fileSize1.unit, fileSize2.size, fileSize2.unit);
    updateStatus(message);
    levelBarSetValue(1.);
    fclose(fp);
    fclose(dest);
    return 0;
}

void *worker(void *arg) {
    int *inc = (int *) arg;
    for (int i = 0; i < readBufferSize; ++i) {
        writeBuffer[i * 2 + *inc] = encodeMatrix[readBuffer[i]][0];
    }
    return (NULL);
}