C_SOURCES = $(shell find . -name "*.c")
OBJ_FILES = ${C_SOURCES:.c=.o \
	boot/boot.o \
	asm/interrupt.o \
	asm/disk_io.o \
	asm/util.o}

CCFLAGS = -I ./kernel/include -g -m32 -std=gnu99 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -no-pie -fno-pic -Wall -Wextra -O2 -lgcc

build: prepare os-image.iso

os-image.iso: kernel.bin
	cd build && cp ../$^ ./boot/$^ && cp ../boot/grub.cfg ./boot/grub/grub.cfg && cd .. && \
		grub-mkrescue -o $@ build

kernel.bin: boot/gdt.o ${OBJ_FILES}
	ld -m elf_i386 -o $@ -Tlinker.ld $^

prepare:
	mkdir -p ./build/boot/grub

run: os-image.iso
	qemu-system-i386 -cdrom $^

run-kernel: kernel.bin
	qemu-system-i386 -kernel $^

debug: os-image.iso
	qemu-system-i386 -cdrom $^ -d int --no-reboot

debug-kernel: kernel.bin
	qemu-system-i386 -kernel $^ -d int --no-reboot

boot/gdt.o:
	gcc -c -w -m32 -masm=intel boot/gdt.s -o $@

%.o: %.c
	gcc $(CCFLAGS) -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@
	
clean:
	$(RM) -rf build
	$(RM) $(shell find . -name "*.o")
	$(RM) os-image.iso
	$(RM) kernel.bin
