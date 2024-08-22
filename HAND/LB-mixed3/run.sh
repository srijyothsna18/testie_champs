date
LITMUSOPTS="${@:-$LITMUSOPTS}"
SLEEP=0
if [ ! -f LB-mixed3.no ]; then
cat <<'EOF'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Results for LB-mixed3.litmus %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RISCV LB+mixed3
"at some point this was allowed by the axiomatic and forbidden by the
operational (not sure which was is correct)"

{uint32_t y=0; 0:x6=x; 0:x8=y; 1:x6=y; 1:x12=x;}

 P0          | P1            ;
 lw x5,0(x6) | lw x8,0(x6)   ;
 fence rw,rw | lh x9,2(x6)   ;
 ori x7,x0,1 | xor x10,x9,x9 ;
 sh x7,0(x8) | ori x10,x10,1 ;
             | sw x10,0(x12) ;

exists (x=2 /\ y=1114113 /\ 0:x5=1 /\ 1:x7=1114113 /\ 1:x8=1114113 /\ 1:x9=17)
Generated assembler
EOF
cat LB-mixed3.t
./LB-mixed3.exe -q $LITMUSOPTS
fi
sleep $SLEEP

cat <<'EOF'
Revision exported, version 7.52
Command line: litmus7 LB-mixed3.litmus -o LB-mixed3
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
