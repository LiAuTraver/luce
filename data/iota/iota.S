    .section .text
    .global _start

_start:
    li t0, 0          # i = 0, set register 5 to 0
    li t1, 0          # j = 0, set register 6 to 0

loop:
    add t1, t1, t0    # j += i, res6 += reg5
    addi t0, t0, 1    # i += 1, reg5 += 1
    li   t2, 10
    blt  t0, t2, loop # if i < 10, continue loop

hang:
    j hang            # infinite loop
