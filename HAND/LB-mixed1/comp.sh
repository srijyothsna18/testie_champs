GCC=gcc
GCCOPTS="-Wall -std=gnu99 -O2 -pthread"
LINKOPTS=""
/bin/rm -f *.exe *.s
$GCC $GCCOPTS -O2 -c outs.c
$GCC $GCCOPTS -O2 -c utils.c
$GCC $GCCOPTS -O2 -c litmus_rand.c
$GCC $GCCOPTS $LINKOPTS -o LB-mixed1.exe outs.o utils.o litmus_rand.o LB-mixed1.c
$GCC $GCCOPTS -S LB-mixed1.c && awk -f show.awk LB-mixed1.s > LB-mixed1.t && /bin/rm LB-mixed1.s
