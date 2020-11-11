rm -rf *.bin
rm -rf *.lock
rm -rf *.img
bximage -hd=10 -mode=create -sectsize=512 -q hd3M.img
nasm -I include -o mbr.bin mbr.S
nasm -I include -o loader.bin loader.S
dd if=./mbr.bin of=./hd3M.img bs=512 count=1 conv=notrunc
dd if=./loader.bin of=./hd3M.img bs=512 count=2 seek=2 conv=notrunc
rm -rf *.bin
