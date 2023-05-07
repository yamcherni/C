; file ps3.as
MAIN: sub r6 ,HIGH
END: stop
LOOP: prn #-8a8
mcr my_macro1
    add r5, r2
    bne HIGH
endmcr
L1: inc HIGH
my_macro1
.entry LOOP
bne LOOP(HIGH,LOW)
mcr my_macro2
    add r1, r7
    bne END
endmcr
HIGH: .data 45, 67, 22
my_macro2
.extern LOW
.entry HIGH