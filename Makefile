##
# Seng 440 Motion Estimation
#
# @version 0.1

# Default target that runs when you just type 'make'

CFLAGS = -O3 -mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard

all: baseline neon asm pipeline pipeline_unroll

non-vm-baseline: main.c
	gcc -O3 -DSAD_BASELINE main.c -o non-vm-baseline

baseline: main.c
	gcc $(CFLAGS) -DSAD_BASELINE main.c -o baseline

unroll: main.c
	gcc $(CFLAGS) -DSAD_UNROLL main.c -o unroll

pipeline: main.c
	gcc $(CFLAGS) -DSAD_PIPELINE main.c -o pipeline

pipeline_unroll: main.c
	gcc $(CFLAGS) -DSAD_PIPELINE_UNROLL main.c -o pipeline_unroll

neon: main.c
	gcc $(CFLAGS) -DSAD_NEON main.c -o neon

asm: main.c
	gcc $(CFLAGS) -DSAD_ASM main.c -o asm

# Clean target to delete executables and start fresh
clean:
	rm -f baseline unroll neon asm pipeline pipeline_unroll non-vm-baseline


# end
