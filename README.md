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
  - [File encoding matrix table](#file-encoding-matrix-table)
  - [File decoding matrix table](#file-decoding-matrix-table)
  - [File encode process](#file-encode-process)
  - [File decode process](#file-decode-process)
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

Result : `1010 0101` => `0010 1011 0101 0101`\ So as we see, we have one
byte in input and we get 2 encoded byte at the output.\ That mean our
output file will be twice bigger than the input one.

#### Decoding

We will process the file we want to decode 2 byte per 2 byte.

The first step is to find our identity matrix in our G4C matrix columns
:

|                                    1234 5678                                     | :arrow_right: |                     5234                     |
|:--------------------------------------------------------------------------------:|:-------------:|:--------------------------------------------:|
| 1**000** **1**111<br>1**100** **0**111<br>1**010** **0**100<br>1**001** **0**010 | :arrow_right: | **1**000<br>0**1**00<br>00**1**0<br>000**1** |

Once we got the position of the column **(5234)** we save it.\ We take
the previously encoded byte `0010 1011 0101 0101`.\ For each byte, we
take the four bits corresponding to the columns we saved :

|   1234 5678   | :arrow_right: |   5234   |
|:-------------:|:-------------:|:--------:|
| 0**010 1**011 | :arrow_right: | **1010** |
| 0**101 0**101 | :arrow_right: | **0101** |

Et voilà !\ We decoded these two byte and recovered our original one :
`1010 0101`


## Usage

The program GUI is pretty simple to understand :

![image_01.png](pictures/image_01.png)

### Load G4C Matrix

First of all, you need to load your G4C matrix text file.\  
:warning: Your key must be in this format : `G4C=[10001111 11000111 10100100 10010010]`, otherwise it won't work.

![image_04.png](pictures/image_04.png)

### Load file

Then, you can choose the file you want to encode / decode

![image_02.png](pictures/image_02.png)


### Start encode

Press the **Encode** button (no kidding ! :upside_down_face:).\
The program interface will freeze during the process but don't panic, it's working.

![image_08.png](pictures/image_08.png)

The encoded file is saved on the same location than the original one, and it has **e** letter added at the end of the file

![image_11.png](pictures/image_11.png)

### Start decode

Press the **Decode** button (haha again no kidding ! :upside_down_face:).\
The program interface will freeze during the process but don't panic, it's working.

![image_05.png](pictures/image_05.png)

The decoded file is saved on the same location than the original one, and it has **d** letter added at the end of the file

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
`msys2/mingw64/include` and copy them outside of there *version
folder*:\ **Exemple :** copy **gtk** and **gdk** folder inside
`msys2/mingw64/include/gtk-3-0/` directly inside
`msys2/mingw64/include/`

## Code

### Loading key

### File encoding matrix table

### File decoding matrix table

### File encode process

### File decode process

## Authors

This project was carried out by myself :sunglasses:.

| [Noé LARRIEU-LACOSTE](https://github.com/Nouuu)      |     ![](https://img.shields.io/github/followers/Nouuu)      |
|:-----------------------------------------------------|:-----------------------------------------------------------:|

