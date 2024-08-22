#!/bin/bash

# Directory containing litmus tests
LITMUS_DIR="Riscv_firstRun"
#cd "$LITMUS_DIR"

count=1

# Convert litmus tests to C code
for litmus_file in *.litmus; do
    echo "Converting $litmus_file to C..."
    base_name="${litmus_file%.litmus}"
    mkdir -p "$base_name"
    count=$((count + 1))
    litmus7 "$litmus_file" -o "${base_name}"
done

for dir in L*/; do
    # Check if it is a directory
    if [ -d "$dir" ]; then
        echo "Processing directory $dir..."
        cd "$dir"
        
       
	riscv64-linux-gnu-gcc -c utils.c -o utils.o
	riscv64-linux-gnu-gcc -c outs.c -o outs.o
	riscv64-linux-gnu-gcc -c litmus_rand.c -o litmus_rand.o
	f_name=""
	for file in L*.c;do
		f_name="${file%.c}"
		 echo "Compiling $f_name..."
		riscv64-linux-gnu-gcc -c "$file" -o "${f_name}.o"
		riscv64-linux-gnu-gcc -o "${f_name}" utils.o outs.o litmus_rand.o "${f_name}.o" -pthread
	
                qemu-riscv64 -L /usr/riscv64-linux-gnu/ "${f_name}" > "${f_name}.output"
        done

        # Return to the parent directory
        cd ..
    fi
   
done



