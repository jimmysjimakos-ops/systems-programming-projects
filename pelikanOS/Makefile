
all: kernel.bin

kernel.bin:	linker.ld boot.o kernel.o
	i686-linux-gnu-ld -T linker.ld -m elf_i386 boot.o kernel.o -o kernel.bin

boot.o: src/boot.asm
	nasm -f elf32 src/boot.asm -o boot.o

kernel.o: src/kernel.c
	i686-linux-gnu-gcc -ffreestanding -Wall -c src/kernel.c -o kernel.o

iso: kernel.bin	
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o pelikanos.iso iso

run: iso
	qemu-system-i386 -cdrom pelikanos.iso

clean:	
	rm -f boot.o kernel.o kernel.bin pelikanos.iso iso/boot/kernel.bin
