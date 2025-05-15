    .section .text
    .global _start


_start:
    lui t1, 0xFFFF0;
    lw t0, 0x100(s2);
    and t0,t0,t1;
    andi t0,t0,0x555;
    sw t0, 0x100(s2);