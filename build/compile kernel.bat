arm-none-eabi-gcc -mcpu=cortex-a7 -fpic -ffreestanding -c "C:\Users\01jac\Git Projects\raspi-microkernel\src\kernel\boot.S" -o boot.o
arm-none-eabi-gcc -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -c "C:\Users\01jac\Git Projects\raspi-microkernel\src\kernel\kernel.c" -o kernel.o -O2 -Wall -Wextra
arm-none-eabi-gcc -T linker.ld -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o