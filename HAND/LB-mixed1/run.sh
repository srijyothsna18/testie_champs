date
LITMUSOPTS="${@:-$LITMUSOPTS}"
SLEEP=0
if [ ! -f LB-mixed1.no ]; then
cat <<'EOF'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Results for LB-mixed1.litmus %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RISCV LB+mixed1
"at some point this was allowed by the axiomatic and forbidden by the
operational (not sure which was is correct)"

{uint64_t z=0; 0:x6=x; 0:x8=y; 1:x6=y; 1:x8=z; 1:x13=x;}

 P0          | P1                     ;
 lw x5,0(x6) | lw x5,0(x6)            ;
 fence rw,rw | ori x7,x0,1            ;
 ori x7,x0,1 | amoadd.w.rl x1,x7,(x8) ;
 sw x7,0(x8) | ld x9,0(x8)            ;
             | lw x10,4(x8)           ;
             | xor x11,x10,x10        ;
             | ori x11,x11,1          ;
             | sw x11,0(x13)          ;

exists (0:x5=1 /\ 1:x1=0 /\ 1:x5=1 /\ 1:x9=1 /\ 1:x10=0)
Generated assembler
EOF
cat LB-mixed1.t
./LB-mixed1.exe -q $LITMUSOPTS
fi
sleep $SLEEP

cat <<'EOF'
Revision exported, version 7.52
Command line: litmus7 LB-mixed1.litmus -o LB-mixed1
Parameters
#define SIZE_OF_TEST 100000
#define NUMBER_OF_RUN 10
#define AVAIL 1
#define STRIDE (-1)
#define MAX_LOOP 0
/* gcc options: -Wall -std=gnu99 -O2 -pthread */
/* barrier: user */
/* launch: changing */
/* affinity: none */
/* alloc: dynamic */
/* memory: direct */
/* safer: write */
/* preload: random */
/* speedcheck: no */
EOF
head -1 comp.sh
echo "LITMUSOPTS=$LITMUSOPTS"
date
