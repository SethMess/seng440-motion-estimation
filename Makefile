##
# Seng 440 Motion Estimation
#
# @version 0.1

# Default target that runs when you just type 'make'
all: baseline neon asm pipeline pipeline_unroll

baseline: main.c
	gcc -DSAD_BASELINE main.c -o baseline

pipeline: main.c
	gcc -DSAD_PIPELINE main.c -o pipeline

pipeline_unroll: main.c
	gcc -DSAD_PIPELINE_UNROLL main.c -o pipeline_unroll

neon: main.c
	gcc -DSAD_NEON main.c -o neon

asm: main.c
	gcc -DSAD_ASM main.c -o asm

# Clean target to delete executables and start fresh
clean:
	rm -f baseline neon asm pipeline pipeline_unroll


# end
