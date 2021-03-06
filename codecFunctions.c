#include "codecFunctions.h"
#include "gtkFunctions.h"
#include "functions.h"

bool codecKeyLoaded = FALSE;
extern char *filePath;
unsigned char *readBuffer = NULL;
size_t readBufferSize;
unsigned char *writeBuffer = NULL;
size_t writeBufferSize;

// Read key from file and fill the codecKey[4][8] array
int readKey(const char *path) {
    int i;
    char c1[9], c2[9], c3[9], c4[9];

    // Open the file
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        updateStatus("Cannot open key file!");
        return 1;
    }

    // Fill the string values
    if (fscanf(fp, "G4C=[%[01] %[01] %[01] %[01]]", c1, c2, c3, c4) != 4) {
        updateStatus("Incorrect key file!");
        fclose(fp);
        return 1;
    }

    // Check number of characters in byte strings
    if (strlen(c1) != 8 || strlen(c2) != 8 || strlen(c3) != 8 || strlen(c4) != 8) {
        updateStatus("Incorrect key file!");
        fclose(fp);
        return 1;
    }

    // Fill the codecKey array
    for (i = 0; i < 8; ++i) {
        codecKey[0][i] = c1[i] == '1' ? 1 : 0;
        codecKey[1][i] = c2[i] == '1' ? 1 : 0;
        codecKey[2][i] = c3[i] == '1' ? 1 : 0;
        codecKey[3][i] = c4[i] == '1' ? 1 : 0;
    }

    fclose(fp);

    // Fill the encode and decode matrices with the key
    return fillMatrixEncode() || fillMatrixDecode();
}

// Fill encodeMatrix[256][2] with all the possibilities
int fillMatrixEncode() {
    char array1[8], array2[8];
    int i, j;

    for (i = 0; i < 256; ++i) {
        // XOR, equivalent to a matrix product
        for (j = 0; j < 8; ++j) {
            array1[j] = (c2b[i][0] && codecKey[0][j]) ^ (c2b[i][1] && codecKey[1][j]) ^ (c2b[i][2] && codecKey[2][j]) ^ (c2b[i][3] && codecKey[3][j]);
            array2[j] = (c2b[i][4] && codecKey[0][j]) ^ (c2b[i][5] && codecKey[1][j]) ^ (c2b[i][6] && codecKey[2][j]) ^ (c2b[i][7] && codecKey[3][j]);
        }
        encodeMatrix[i][0] = b2C(array1);
        encodeMatrix[i][1] = b2C(array2);
    }

    return 0;
}

// Fill decodeMatrix[256][2]
int fillMatrixDecode() {
    setSpinnerStatus(TRUE);
    levelBarSetValue(0.);
    int i, j, k;
    char i4[5], matrixI4[4], byte[8];

    // Looking for identity matrix
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

    // Decode
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
    }
    return 0;
}

int encode() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // Check if there's a file to encode
    if (filePath == NULL || strlen(filePath) == 0) {
        updateStatus("File input empty!");
        return 1;
    }

    // Check if there's a key
    if (!codecKeyLoaded) {
        updateStatus("Key not loaded!");
        return 1;
    }

    // Create destination path (sourcePath + "e")
    char *destPath = malloc(strlen(filePath) + 2);
    strcat(strcpy(destPath, filePath), "e");

    // Open the file to encode
    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        updateStatus("Can't open file !");
        return 1;
    }

    // Create the new encoded file
    FILE *dest = fopen(destPath, "wb");
    if (dest == NULL) {
        updateStatus("Can't create new file!");
        fclose(fp);
        return 1;
    }

    // Get size of file, method depending on the OS
    #ifdef _WIN64
        assert(!_fseeki64(fp, 0, SEEK_END));
        size_t size = _ftelli64(fp);
        assert(!_fseeki64(fp, 0, SEEK_SET));
    #elif __linux__
        assert(!fseek(fp, 0, SEEK_END));
        size_t size = ftell(fp);
        assert(!fseek(fp, 0, SEEK_SET));
    #else
    #error You need to compile on Linux or Windows 64bits
    #endif

    size_t size2 = size; // size2: cursor from end to beginning
    pthread_t threads[2]; // create 2 threads
    readBuffer = malloc(1); // init buffer
    writeBuffer = malloc(1);

    if (size2 >= 10485760) { // 10 Mo
        readBufferSize = 10485760;
        writeBufferSize = readBufferSize * 2;
        readBuffer = malloc(readBufferSize);
        writeBuffer = malloc(writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            pthread_create(threads, NULL, worker1, NULL);
            pthread_create(threads + 1, NULL, worker2, NULL);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }

    }

    if (size2 >= 1048576) { // 1Mo
        readBufferSize = 1048576;
        writeBufferSize = readBufferSize * 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            pthread_create(threads, NULL, worker1, NULL);
            pthread_create(threads + 1, NULL, worker2, NULL);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }
    }

    if (size2 >= 1024) {
        readBufferSize = 1024;
        writeBufferSize = readBufferSize * 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            pthread_create(threads, NULL, worker1, NULL);
            pthread_create(threads + 1, NULL, worker2, NULL);

            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }
    }

    readBufferSize = 1;
    writeBufferSize = readBufferSize * 2;
    readBuffer = realloc(readBuffer, readBufferSize);
    writeBuffer = realloc(writeBuffer, writeBufferSize);

    while (fread(readBuffer, 1, readBufferSize, fp) == readBufferSize) {
        writeBuffer[0] = encodeMatrix[readBuffer[0]][0];
        writeBuffer[1] = encodeMatrix[readBuffer[0]][1];
        assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
    }

    free(readBuffer);
    free(writeBuffer);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fileSize fileSize1 = readableFileSize(size);

// Get size of destination file, method depending on the OS
#ifdef _WIN64
    fileSize fileSize2 = readableFileSize(_ftelli64(dest));
#elif __linux__
    fileSize fileSize2 = readableFileSize(ftell(dest));
#else
#error You need to compile on Linux or Windows 64bits
#endif
    char message[255];
    sprintf(message, "File encoded! Encoding time: %lf seconds, source size: %.2lf %s, dest size: %.2lf %s",
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

    // Check if there's a file to decode
    if (filePath == NULL || strlen(filePath) == 0) {
        updateStatus("File input empty!");
        return 1;
    }

    // Check if there's a key
    if (!codecKeyLoaded) {
        updateStatus("Key not loaded !");
        return 1;
    }

    // Create destination path (sourcePath + "d")
    char *destPath = malloc(strlen(filePath) + 2);
    strcat(strcpy(destPath, filePath), "d");

    // Open the file to decode
    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        updateStatus("Can't open file!");
        return 1;
    }

    // Create the new decoded file
    FILE *dest = fopen(destPath, "wb");
    if (dest == NULL) {
        updateStatus("Can't create new file!");
        fclose(fp);
        return 1;
    }

#ifdef _WIN64
    assert(!_fseeki64(fp, 0, SEEK_END));
    size_t size = _ftelli64(fp);
    assert(!_fseeki64(fp, 0, SEEK_SET));
#elif __linux__
    assert(!fseek(fp, 0, SEEK_END));
    size_t size = ftell(fp);
    assert(!fseek(fp, 0, SEEK_SET));
#else
#error You need to compile on Linux or Windows 64bits
#endif
    size_t size2 = size;
    writeBuffer = malloc(1);
    readBuffer = malloc(1);
    int i;

    if (size2 > 20971520) {
        readBufferSize = 20971520;
        writeBufferSize = readBufferSize / 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }
    }

    if (size2 > 2097152) {
        readBufferSize = 2097152;
        writeBufferSize = readBufferSize / 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }
    }

    if (size2 > 2048) {
        readBufferSize = 2048;
        writeBufferSize = readBufferSize / 2;
        readBuffer = realloc(readBuffer, readBufferSize);
        writeBuffer = realloc(writeBuffer, writeBufferSize);
        while (size2 >= readBufferSize) {
            size2 -= readBufferSize;

            assert(fread(readBuffer, 1, readBufferSize, fp) == readBufferSize);

            for (i = 0; i < writeBufferSize; i++) {
                writeBuffer[i] = decodeMatrix[readBuffer[i * 2]][readBuffer[i * 2 + 1]];
            }

            assert(fwrite(writeBuffer, 1, writeBufferSize, dest) == writeBufferSize);
        }

    }

    readBufferSize = 2;
    writeBufferSize = readBufferSize / 2;
    readBuffer = realloc(readBuffer, readBufferSize);

    while (fread(readBuffer, 1, readBufferSize, fp) == readBufferSize) {
        assert(fwrite(&decodeMatrix[readBuffer[0]][readBuffer[1]], 1, writeBufferSize, dest) == writeBufferSize);
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fileSize fileSize1 = readableFileSize(size);
#ifdef _WIN64
    fileSize fileSize2 = readableFileSize(_ftelli64(dest));
#elif __linux__
    fileSize fileSize2 = readableFileSize(ftell(dest));
#else
#error You need to compile on Linux or Windows 64bits
#endif
    char message[255];
    sprintf(message, "File decoded ! Decoding time: %lf seconds, source size : %.2lf %s, dest size : %.2lf %s",
            cpu_time_used,
            fileSize1.size, fileSize1.unit, fileSize2.size, fileSize2.unit);
    updateStatus(message);
    levelBarSetValue(1.);
    free(readBuffer);
    free(writeBuffer);
    fclose(fp);
    fclose(dest);
    return 0;
}

void *worker1() {
    for (int i = 0; i < readBufferSize; ++i) {
        writeBuffer[i * 2] = encodeMatrix[readBuffer[i]][0];
    }
    return (NULL);
}

void *worker2() {
    for (int i = 0; i < readBufferSize; ++i) {
        writeBuffer[i * 2 + 1] = encodeMatrix[readBuffer[i]][1];
    }
    return (NULL);
}