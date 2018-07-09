#!/bin/bash

make clean

touch user/user_main.c

make COMPILE=gcc BOOT=nono APP=0 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=6
