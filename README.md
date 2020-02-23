# CODECC

| ![Release Version](https://img.shields.io/github/v/release/Nouuu/CODECC) | ![Release Date](https://img.shields.io/github/release-date/Nouuu/CODECC) | ![Contributors](https://img.shields.io/github/contributors/Nouuu/CODECC) | ![Status](https://img.shields.io/badge/Status-ended-red) | ![Platform](https://img.shields.io/badge/Platform-Windows%20%2F%20Linux-blue) |
|:------------------------------------------------------------------------:|:------------------------------------------------------------------------:|:------------------------------------------------------------------------:|:--------------------------------------------------------:|:-----------------------------------------------------------------------------:|

> It's CODEC, but in C

## Table of contents

- [Description](#description)
  - [Features](#features)
  - [How it works](#how-it-works)
    - [Encoding](#encoding)
    - [Decoding](#decoding)
- [Usage](#usage)
  - [Load G4C Matrix](#load-g4c-matrix)
  - [Load file](#load-file)
  - [Start encode](#start-encode)
  - [Start decode](#start-decode)
  - [Warning :warning: :warning: :warning: !!!](#warning----)
    - [Using msys2](#using-msys2-)
- [Code](#code)
  - [Loading key](#loading-key)
  - [File encoding matrix table](#fill-encoding-matrix-table)
  - [File decoding matrix table](#fill-decoding-matrix-table)
  - [File encode process](#file-encode-process)
  - [File decode process](#file-decode-process)
  - [Cross compile issue fix](#cross-compile-issue-fix)
- [Contributing](#contributing)
- [Authors](#authors)


## Description

This application is G4C Matrix encryption program developed in C.

### Features

| Feature     | Description                                   |
|:------------|:----------------------------------------------|
| Encode file | Encode the give file the the given G4C Matrix |
| Decode file | Encode the give file the the given G4C Matrix |

### How it works

Basically, we will use a G4C encoding matrix (matrix of 4 lines each
containing the value of one bytes expressed in 8 bits) which we will
load from a text file in this format :

`G4C=[10001111 11000111 10100100 10010010]`


#### Encoding

We will process the file we want to encode byte per byte by making a
matrix product with our encoding matrix like this:

- Our matrix : `G4C=[10001111 11000111 10100100 10010010]`
- Our byte : `1010 0101`

We separate our byte in two parts : `1010` and `0101`

And we make a matrix product (in this case this like we do a **XOR**
between the byte and the matrix)

| Byte / Matrix |               | 1000 1111<br>1100 0111<br>1010 0100<br>1001 0010 |
|:-------------:|:-------------:|:-------------------------------------------------|
|     1010      | :arrow_right: | 0010 1011                                        |
|     0101      | :arrow_right: | 0101 0101                                        |

Result : `1010 0101` => `0010 1011 0101 0101`  
So as we see, we have one byte in input and we get 2 encoded byte at the
output.  
That mean our output file will be twice bigger than the input one.

#### Decoding

We will process the file we want to decode 2 byte per 2 byte.

The first step is to find our identity matrix in our G4C matrix columns
:

|                                    1234 5678                                     | :arrow_right: |                     5234                     |
|:--------------------------------------------------------------------------------:|:-------------:|:--------------------------------------------:|
| 1**000** **1**111<br>1**100** **0**111<br>1**010** **0**100<br>1**001** **0**010 | :arrow_right: | **1**000<br>0**1**00<br>00**1**0<br>000**1** |

Once we got the position of the column **(5234)** we save it.  
We take the previously encoded byte `0010 1011 0101 0101`.  
For each byte, we take the four bits corresponding to the columns we
saved :

|   1234 5678   | :arrow_right: |   5234   |
|:-------------:|:-------------:|:--------:|
| 0**010 1**011 | :arrow_right: | **1010** |
| 0**101 0**101 | :arrow_right: | **0101** |

Et voilà !  
We decoded these two byte and recovered our original one : `1010 0101`


## Usage

The program GUI is pretty simple to understand :

![image_01.png](pictures/image_01.png)

### Load G4C Matrix

First of all, you need to load your G4C matrix text file.  
:warning: Your key must be in this format : `G4C=[10001111 11000111
10100100 10010010]`, otherwise it won't work.

![image_04.png](pictures/image_04.png)

### Load file

Then, you can choose the file you want to encode / decode

![image_02.png](pictures/image_02.png)


### Start encode

Press the **Encode** button (no kidding ! :upside_down_face:).  
The program interface will freeze during the process but don't panic,
it's working.

![image_08.png](pictures/image_08.png)

The encoded file is saved on the same location than the original one,
and it has **e** letter added at the end of the file

![image_11.png](pictures/image_11.png)

### Start decode

Press the **Decode** button (haha again no kidding !
:upside_down_face:).  
The program interface will freeze during the process but don't panic,
it's working.

![image_05.png](pictures/image_05.png)

The decoded file is saved on the same location than the original one,
and it has **d** letter added at the end of the file

![image_10.png](pictures/image_10.png)

### Warning :warning: :warning: :warning: !!!

In both case, you will need to have mingw installed with gtk+3.20 at
least.  
I didn't figured out yet how to compile it in static in all-in-one
executable file...

#### Using [msys2](https://www.msys2.org/) :

Here's the command to execute inside [msys2](https://www.msys2.org/) to
be able to compile this project :

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gtk3
```

Then, you need to link the `msys2/mingw64/bin` folder to your `system
environment variable PATH`

During the build, if some libraries are not found, go to the folder
`msys2/mingw64/include` and copy them outside of there *version folder*:  
**Exemple :** copy **gtk** and **gdk** folder inside
`msys2/mingw64/include/gtk-3-0/` directly inside
`msys2/mingw64/include/`

## Code

### Loading key

First of all, we need to load our key, otherwise the program won't start
encoding / decoding process.  
The key must be in valid format, we will store it in a local array `char
codecKey[4][8]`.

Function `int readKey(const char *path)` in `codecFunction.c` open the
key text file first check valid format of the key :

```c
int readKey(const char *path) {
     
     ...
    
    char c1[9], c2[9], c3[9], c4[9];
    
    ...
    
    if (fscanf(fp, "G4C=[%[01] %[01] %[01] %[01]]", c1, c2, c3, c4) != 4)
    
    ...
    
    if (strlen(c1) != 8 || strlen(c2) != 8 || strlen(c3) != 8 || strlen(c4) != 8)
```

Then, it store the key in our `codecKey[4][8]` array :

```c
    for (i = 0; i < 8; ++i) {
        codecKey[0][i] = c1[i] == '1' ? 1 : 0;
        codecKey[1][i] = c2[i] == '1' ? 1 : 0;
        codecKey[2][i] = c3[i] == '1' ? 1 : 0;
        codecKey[3][i] = c4[i] == '1' ? 1 : 0;
    }
    
    ...
    
}
```

### Fill encoding matrix table

During the encoding process, we don't want to process each bytes with
[encoding](#encoding) method.  
If we think about it, there is only 256 bytes, each input byte give two
at output.  
So we will fill a local array `unsigned char encodeMatrix[256][2]` with
all the possibilities.  
Then in our encoding process, we just access the right index of the
array, wich is the value of the byte !

Function `int fillMatrixEncode()` in `codecFunction.c` will process the
256 * 2 bytes possibilities depending of the key:

```c
int fillMatrixEncode() {
    char array1[8], array2[8];
    int i, j;
    
    for (i = 0; i < 256; ++i) {
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
```

As we see, we process with a XOR function because a matrix product
between bits is the same as if we do XOR on these.

### Fill decoding matrix table

During the decoding process, we don't want to process each bytes with
[decoding](#decoding) method.  
If we think about it, there is only 256 * 256 combination of bytes when
we process these two by two.  
So we will fill a local array `unsigned char decodeMatrix[256][256]`
with all the possibilities.  
Then in our decoding process, we just access the right index of the
array, wich is the value of the first byte for the first array
dimension, then the second one for the second dimension !

Function `int fillMatrixDecode()` in `codecFunction.c` will process the
256 * 256 bytes possibilities depending of the key:

```c
int fillMatrixDecode() {
    
    ...
    
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
```

As we say in [decoding](#decoding), the first step is to find our
identity matrix in our G4C matrix columns. Once we have it, we can
continue :

```c
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
```

As we see, wee fill our two dimensional array with all the
possibilities, depending of our identity matrix.

### File encode process

For this part, I will just describe the part where we read / write
bytes, the rest of the function is just classic file processing.  
Function `int encode()` in `codecFunction.c` will open file, create
encode file and fill it with encoded bytes.  
Treatments is faster when we do it in memory, so we will use 4
differents buffer size, depending of the file size (10MB, 1MB, 1KB, 1B).
Each buffer has the same working process so let's see one of them :

```c
int encode() {

    ...
    
        if (size2 >= 10485760) {
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
```

`size2` is at the beginning the source file size, and we decrement it
every time we process the file, it allow us to know how many bytes left
to process. `readBuffer` array contain the bytes we read from source
file, `writeBuffer` array will contain the encoded bytes, his size is 2x
`readBuffer` size.  
To fill `writeBuffer`, we use 2 threads, each one fill the `writeBuffer`
array with the first and second encoded byte from the source one.  
It call's worker for that, this function look like this :

```c
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
```

Once we processed all bytes, we just free buffers and close source and
destination file.


### File decode process

For this part, I will just describe the part where we read / write
bytes, the rest of the function is just classic file processing.  
Function `int decode()` in `codecFunction.c` will open file, create
decode file and fill it with decoded bytes.  
Treatments is faster when we do it in memory, so we will use 4
differents buffer size, depending of the file size (20MB, 2MB, 2KB, 2B).
Each buffer has the same working process so let's see one of them :

```c
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
```

`size2` is at the beginning the source file size, and we decrement it
every time we process the file, it allow us to know how many bytes left
to process. `readBuffer` array contain the bytes we read from source
file, `writeBuffer` array will contain the decode bytes, his size is
half `readBuffer` size.  
To fill `writeBuffer`, we just use readBuffer bytes two by two and use
them as index of our `decodeMatrix`.  
Once we processed all bytes, we just free buffers and close source and
destination file.

### Cross compile issue fix

If we want to compile on both Linux and Windows system, we have an issue
with `fseek() ftell()` functions.  
I use these functions to determine the size of my file but on Windows,
`ftell()` only return `int` type value.  
So if my file size is higher than `2147483647 bytes` (`~ 2.14 GB`), it
will return -1 on `size_t size`, which is unsigned type. That will cause
size to take his max value -1.  
This problem is not present on Linux system because `ftell()` return
`long` type value, which is enough.  
To fix this problem on Windows, we can use `_fseeki64() _ftelli64()`
functions from mingw which return `long long` type value. But these
functions are not present on Linux.  
That's why we use compilation macro to determine on which System we are
and which functions to use :

```c
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
```

## Contributing

If someone has the idea and know how to compile in static with GTK and
stuffs to be dependence free, be my guest !

## Authors

This project was carried out by myself :sunglasses:.

| [Noé LARRIEU-LACOSTE](https://github.com/Nouuu) | ![](https://img.shields.io/github/followers/Nouuu) |
|:------------------------------------------------|:--------------------------------------------------:|

