#!/bin/bash

gcc -Wall -o example example.c  ../twitchlib.c -lssl -lcrypto -ggdb
