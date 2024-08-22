#!/bin/sh
GCC=gcc
GCCOPTS="-Wall -std=gnu99 -fomit-frame-pointer -O2 -pthread"
LINKOPTS=""
/bin/rm -f *.exe *.s
$GCC $GCCOPTS -O2 -c outs.c
$GCC $GCCOPTS -O2 -c utils.c
$GCC $GCCOPTS -O2 -c litmus_rand.c
$GCC $GCCOPTS $LINKOPTS -o test.exe outs.o utils.o litmus_rand.o test.c
$GCC $GCCOPTS -S test.c && awk -f show.awk test.s > test.t && /bin/rm test.s
