rm -rf ./build/*.bin
rm -rf ./build/*.lock
rm -rf ./build/*.img
rm -rf ./*img
rm -rf ./build/*.o
rm -rf ./*.bin
bximage -hd=10 -mode=create -sectsize=512 -q ./hd3M.img
nasm -I ./boot/include -o mbr.bin ./boot/mbr.S
nasm -I ./boot/include -o loader.bin ./boot/loader.S
dd if=./mbr.bin of=./hd3M.img bs=512 count=1 conv=notrunc
dd if=./loader.bin of=./hd3M.img bs=512 count=4 seek=2 conv=notrunc


i386-elf-gcc -m32 -I lib/ -I lib/kernel/ -I kernel/ -c -fno-builtin \
    -o build/main.o kernel/main.c

nasm -f elf -I lib/include/ -o build/print.o lib/kernel/print.S
nasm -f elf -o build/kernel.o kernel/kernel.S
i386-elf-gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/interrupt.o kernel/interrupt.c
i386-elf-gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/timer.o device/timer.c
i386-elf-gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -c -fno-builtin \
    -o build/init.o kernel/init.c

i386-elf-ld -melf_i386 -Ttext 0xc0001000 -e main -o build/kernel.bin build/main.o build/init.o \
    build/interrupt.o build/print.o build/kernel.o build/timer.o

dd if=build/kernel.bin of=hd3M.img bs=512 count=200 seek=9 conv=notrunc
