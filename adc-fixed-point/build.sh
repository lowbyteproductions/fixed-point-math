#!/bin/bash

LIBFLAGS="-lm"
COMPFLAGS="-Wall"

gcc main.c $COMPFLAGS $LIBFLAGS -o main
