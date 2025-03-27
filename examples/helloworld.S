# riscv64-linux-gnu-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles helloworld.S -o helloworld.elf
# riscv64-linux-gnu-objcopy -O binary -j .text helloworld.elf helloworld.bin
    .section .text
    .globl _start

_start:
    # syscall write(stdout=1, buf=message, len=13)
    li a7, 64         # syscall number for sys_write
    li a0, 1          # file descriptor 1 (stdout)
    la a1, message    # pointer to the message
    li a2, 13         # length of message
    ecall

    # syscall exit(0)
    li a7, 93         # syscall number for sys_exit
    li a0, 0          # exit code 0
    ecall

    .section .rodata
message:
    .asciz "Hello, World!"
