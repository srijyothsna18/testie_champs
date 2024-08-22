date
LITMUSOPTS="${@:-$LITMUSOPTS}"
SLEEP=0
if [ ! -f LR-SC-mixed2.no ]; then
cat <<'EOF'
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Results for LR-SC-mixed2.litmus %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RISCV LR-SC-mixed2
"the footprint of the lr determines the size of reservation (and it is
not reduced to the part overlapping with the sc)"

{uint64_t x=0; 0:x6=x; 0:x9=y; 1:x6=x; 1:x9=z;}

 P0                  | P1                  ;
 lr.d x5,0(x6)       | lr.d x5,0(x6)       ;
 ori x7,x0,1         | ori x7,x0,1         ;
 sw x7,0(x6)         | sw x7,4(x6)         ;
 sc.w.rl x8,x7,0(x9) | sc.w.rl x8,x7,0(x9) ;

~exists (x=4294967297 /\ y=1 /\ z=1 /\ 0:x5=0 /\ 0:x8=0 /\ 1:x5=0 /\ 1:x8=0)
Generated assembler
EOF
cat LR-SC-mixed2.t
./LR-SC-mixed2.exe -q $LITMUSOPTS
fi
sleep $SLEEP

cat <<'EOF'
Revision exported, version 7.52
Command line: litmus7 LR-SC-mixed2.litmus -o LR-SC-mixed2
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
