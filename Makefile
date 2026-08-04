##
# Seng 440 Motion Estimation
#
# @version 0.1

# Default target that runs when you just type 'make'

SRCS = main.c test_utils.c

CFLAGS = -O3 -mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard
NONVM_CFLAGS = -O3

VM_VARIANTS = baseline unroll neon asm pipeline
NONVM_VARIANTS = non-vm-baseline non-vm-unroll non-vm-pipeline non-vm-asm

all: baseline neon asm pipeline

non-vm: non-vm-baseline non-vm-unroll non-vm-pipeline non-vm-asm

non-vm-baseline: $(SRCS)
	gcc $(NONVM_CFLAGS) -DSAD_BASELINE $(SRCS) -o non-vm-baseline

non-vm-unroll: $(SRCS)
	gcc $(NONVM_CFLAGS) -DSAD_UNROLL $(SRCS) -o non-vm-unroll

non-vm-pipeline: $(SRCS)
	gcc $(NONVM_CFLAGS) -DSAD_PIPELINE $(SRCS) -o non-vm-pipeline

non-vm-asm: $(SRCS)
	gcc $(NONVM_CFLAGS) -DSAD_ASM $(SRCS) -o non-vm-asm

baseline: $(SRCS)
	gcc $(CFLAGS) -DSAD_BASELINE $(SRCS) -o baseline

unroll: $(SRCS)
	gcc $(CFLAGS) -DSAD_UNROLL $(SRCS) -o unroll

pipeline: $(SRCS)
	gcc $(CFLAGS) -DSAD_PIPELINE $(SRCS) -o pipeline

neon: $(SRCS)
	gcc $(CFLAGS) -DSAD_NEON $(SRCS) -o neon

asm: $(SRCS)
	gcc $(CFLAGS) -DSAD_ASM $(SRCS) -o asm

# Clean target to delete executables and start fresh
clean:
	rm -f baseline unroll neon asm pipeline non-vm-baseline non-vm-unroll non-vm-pipeline non-vm-asm

# CFLAGS above is ARM-only (cortex-a15/NEON), so on a non-ARM host we build
# and test the non-vm-* targets instead. SAD_NEON is skipped off-ARM since
# sad_neon() only exists under __ARM_NEON.
ifeq ($(filter arm%,$(shell uname -m)),)
TEST_VARIANTS = $(NONVM_VARIANTS)
else
TEST_VARIANTS = $(VM_VARIANTS)
endif

test: $(TEST_VARIANTS)
	@for v in $(TEST_VARIANTS); do \
		echo "=== $$v ==="; \
		./$$v --selftest && echo "$$v: PASS" || echo "$$v: FAIL"; \
	done

# end
