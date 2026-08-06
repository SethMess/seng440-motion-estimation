# seng440-motion-estimation


## How to Build with Make.

``` sh
make
```

Or if you are not on the VM use
``` sh
make non-vm
```

Clean up executables with

``` sh
make clean
```


## How to Run Selftest.

``` sh
./baseline --selftest
```

or run make test to run all the tests automatically.

``` sh
make test
```

## How to Compile.
(these instructions dont include all the flags)

``` sh
gcc -DSAD_BASELINE main.c test_utils.c -o baseline
```


Compile source code into Assembler instructions:
``` sh
gcc -O2 -mcpu=cortex-a15 -mfpu=neon -DSAD_BASELINE main.c -o baseline.s -S
```

High optimization level
``` sh
gcc -O3 -DSAD_BASELINE main.c test_utils.c -o baseline
```
