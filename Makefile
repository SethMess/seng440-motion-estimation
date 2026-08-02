##
# Seng 440 Motion Estimation
#
# @version 0.1

# Default target that runs when you just type 'make'
all: baseline neon asm

# Build the admin executable
baseline: main.c
	gcc -DSAD_BASELINE main.c -o baseline

# Build the client executable
neon: main.c
	gcc -DSAD_NEON main.c -o neon

asm: main.c
	gcc -DSAD_ASM main.c -o asm

# Clean target to delete executables and start fresh
clean:
	rm -f baseline neon asm


# end
