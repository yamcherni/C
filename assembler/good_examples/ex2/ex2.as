; file ps2.as
MAIN: sub r3 ,HIGH
END: stop
LOOP: prn #-5
mcr my_macro
    add r3, r2
    bne HIGH
endmcr
L1: inc HIGH
my_macro
.entry LOOP
bne LOOP(HIGH,LOW)
HIGH: .data 23
.extern LOW
.entry HIGH
.extern WE