#!/bin/bash

gcc -Wall -o example example_ssl.c  twitchlib.c -lssl -lcrypto -ggdb
