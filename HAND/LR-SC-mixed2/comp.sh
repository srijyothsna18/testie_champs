GCC=gcc
GCCOPTS="-Wall -std=gnu99 -O2 -pthread"
LINKOPTS=""
/bin/rm -f *.exe *.s
$GCC $GCCOPTS -O2 -c outs.c
$GCC $GCCOPTS -O2 -c utils.c
$GCC $GCCOPTS -O2 -c litmus_rand.c
$GCC $GCCOPTS $LINKOPTS -o LR-SC-mixed2.exe outs.o utils.o litmus_rand.o LR-SC-mixed2.c
$GCC $GCCOPTS -S LR-SC-mixed2.c && awk -f show.awk LR-SC-mixed2.s > LR-SC-mixed2.t && /bin/rm LR-SC-mixed2.s
