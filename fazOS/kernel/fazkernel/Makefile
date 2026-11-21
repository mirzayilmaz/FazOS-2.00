CFLAGS=-m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -mno-sse -mno-mmx -O2 -Wall -Wextra
all: fazkernel.elf
boot.o: boot.S
	as --32 boot.S -o boot.o
kernel.o: kernel.c
	gcc $(CFLAGS) -c kernel.c -o kernel.o
fazkernel.elf: boot.o kernel.o linker.ld
	ld -m elf_i386 -nostdlib -T linker.ld -o fazkernel.elf boot.o kernel.o
iso: fazkernel.elf
	mkdir -p iso/boot/grub && cp fazkernel.elf iso/boot/ && cp grub.cfg iso/boot/grub/
	grub-mkrescue -o FazKernel.iso iso
run: fazkernel.elf
	qemu-system-i386 -kernel fazkernel.elf
clean:
	rm -rf *.o *.elf iso FazKernel.iso
