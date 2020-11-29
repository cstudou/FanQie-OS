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

nasm -f elf -I ./lib/include  -o ./build/print.o ./lib/kernel/print.S
i386-elf-gcc -m32 -c -o ./build/main.o ./kernel/main.c
#gcc -m32 -c -o ./build/main.o ./kernel/main.c
i386-elf-ld -melf_i386 -Ttext 0xc0001000 -e main -o ./build/kernel.bin ./build/main.o ./build/print.o 
dd if=./build/kernel.bin of=./hd3M.img bs=512 count=200 seek=9 conv=notrunc
rm -rf ./*.bin
