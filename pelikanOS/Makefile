
all: kernel.bin

kernel.bin:	linker.ld boot.o kernel.o gdt.o gdt_asm.o
	i686-linux-gnu-ld -T linker.ld -m elf_i386 boot.o kernel.o gdt.o gdt_asm.o -o kernel.bin

boot.o: src/boot.asm
	nasm -f elf32 src/boot.asm -o boot.o

kernel.o: src/kernel.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/kernel.c -o kernel.o

gdt.o:	src/gdt.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/gdt.c -o gdt.o

gdt_asm.o:	src/gdt_asm.asm
	nasm -f elf32 src/gdt_asm.asm -o gdt_asm.o

iso: kernel.bin	
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o pelikanos.iso iso

run: iso
	qemu-system-i386 -cdrom pelikanos.iso

clean:	
	rm -f boot.o kernel.o gdt.o gdt_asm.o kernel.bin pelikanos.iso iso/boot/kernel.bin
