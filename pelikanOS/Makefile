all: kernel.bin

kernel.bin:	linker.ld boot.o kernel.o gdt.o idt.o idt_asm.o pic.o gdt_asm.o isr.o isr_asm.o vga.o pmm.o string.o paging.o
	i686-linux-gnu-ld -T linker.ld -m elf_i386 boot.o kernel.o gdt.o gdt_asm.o idt.o idt_asm.o pic.o isr.o isr_asm.o vga.o pmm.o string.o paging.o -o kernel.bin

boot.o: src/boot.asm
	nasm -f elf32 src/boot.asm -o boot.o

kernel.o: src/kernel.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/kernel.c -o kernel.o

gdt.o:	src/gdt.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/gdt.c -o gdt.o

gdt_asm.o:	src/gdt_asm.asm
	nasm -f elf32 src/gdt_asm.asm -o gdt_asm.o

idt.o:	src/idt.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/idt.c -o idt.o

idt_asm.o:	src/idt_asm.asm
	nasm -f elf32 src/idt_asm.asm -o idt_asm.o

pic.o:	src/pic.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/pic.c -o pic.o

vga.o:	src/vga.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/vga.c -o vga.o

isr.o:	src/isr.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/isr.c -o isr.o

isr_asm.o:	src/isr_asm.asm
	nasm -f elf32 src/isr_asm.asm -o isr_asm.o

pmm.o:	src/pmm.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/pmm.c -o pmm.o

string.o:	src/string.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/string.c -o string.o

paging.o:	src/paging.c
	i686-linux-gnu-gcc -ffreestanding -Wall -Wextra -I include -c src/paging.c -o paging.o

iso: kernel.bin	
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o pelikanos.iso iso

run: iso
	qemu-system-i386 -cdrom pelikanos.iso

clean:	
	rm -f boot.o kernel.o gdt.o gdt_asm.o idt.o idt_asm.o pic.o isr.o isr_asm.o vga.o pmm.o string.o paging.o kernel.bin pelikanos.iso iso/boot/kernel.bin