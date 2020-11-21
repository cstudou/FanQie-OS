rm -rf *.bin
rm -rf *.lock
rm -rf *.img
bximage -hd=10 -mode=create -sectsize=512 -q hd3M.img
nasm -I ./boot/include -o mbr.bin ./boot/mbr.S
nasm -I ./boot/include -o loader.bin ./boot/loader.S
dd if=./mbr.bin of=./hd3M.img bs=512 count=1 conv=notrunc
dd if=./loader.bin of=./hd3M.img bs=512 count=4 seek=2 conv=notrunc
rm -rf *.bin
