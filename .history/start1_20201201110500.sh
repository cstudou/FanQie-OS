i386-elf-gcc -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/main.o kernel/main.c

nasm -f elf -I lib/include/ -o build/print.o lib/kernel/print.S
nasm -f elf -o build/kernel.o kernel/kernel.S
i386-elf-gcc -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/interrupt.o kernel/interrupt.c

i386-elf-gcc -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/init.o kernel/init.c

i386-elf-ld -Ttext 0xc0001500 -e main -o build/kernel.bin build/main.o build/init.o \
    build/interrupt.o build/print.o build/kernel.o

dd if=build/kernel.bin of=hd3M.img bs=512 count=200 seek=9 conv=notrunc
