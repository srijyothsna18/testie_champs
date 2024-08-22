#!/bin/sh

date
LITMUSOPTS="${@:-$LITMUSOPTS}"
SLEEP=0
if [ ! -f test.no ]; then
cat <<'EOF'
%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Results for test.litmus %
%%%%%%%%%%%%%%%%%%%%%%%%%%%
X86 SB
"Fre PodWR Fre PodWR"

{
 [x]=0;
 [y]=0;
}
 P0          | P1          ;
 MOV [x],$1  | MOV [y],$1  ;
 MOV EAX,[y] | MOV EAX,[x] ;

locations [x; y;]
exists (0:EAX=0 /\ 1:EAX=0)
Generated assembler
EOF
cat test.t
./test.exe -q $LITMUSOPTS
fi
sleep $SLEEP

cat <<'EOF'
Revision exported, version 7.57
Command line: litmus7 test.litmus -o test_C
Parameters
#define SIZE_OF_TEST 100000
#define NUMBER_OF_RUN 10
#define AVAIL 1
#define STRIDE (-1)
#define MAX_LOOP 0
/* gcc options: -Wall -std=gnu99 -fomit-frame-pointer -O2 -pthread */
/* barrier: user */
/* launch: changing */
/* affinity: none */
/* memory: direct */
/* safer: write */
/* preload: random */
/* speedcheck: no */
/* alloc: dynamic */
EOF
sed '2q;d' comp.sh
echo "LITMUSOPTS=$LITMUSOPTS"
date
