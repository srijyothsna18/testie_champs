GCC=gcc
GCCOPTS="-Wall -std=gnu99 -O2 -pthread"
LINKOPTS=""
/bin/rm -f *.exe *.s
$GCC $GCCOPTS -O2 -c outs.c
$GCC $GCCOPTS -O2 -c utils.c
$GCC $GCCOPTS -O2 -c litmus_rand.c
$GCC $GCCOPTS $LINKOPTS -o LB-mixed3.exe outs.o utils.o litmus_rand.o LB-mixed3.c
$GCC $GCCOPTS -S LB-mixed3.c && awk -f show.awk LB-mixed3.s > LB-mixed3.t && /bin/rm LB-mixed3.s
