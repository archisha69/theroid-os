C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c lib/*.c kernel/core/*.c)
HEADERS = $(wildcard kernel/*.h  drivers/*.h cpu/*.h lib/*.h kernel/core/*.h)
OBJ_FILES = ${C_SOURCES:.c=.o cpu/interrupt.o}

build: os-image.img
	$(MAKE) clean

kernel.img: boot/kernel_entry.o ${OBJ_FILES}
	x86_64-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

os-image.img: boot/mbr.bin kernel.img
	cat $^ > $@

run:
ifeq ($(shell test -e os-image.img && echo -n yes),yes)
	qemu-system-i386 -fda os-image.img -device isa-debug-exit,iobase=0xf4,iosize=0x04
else
	$(MAKE) build 
	qemu-system-i386 -fda os-image.img -device isa-debug-exit,iobase=0xf4,iosize=0x04
endif

echo: os-image.img
	xxd $<

kernel.elf: boot/kernel_entry.o ${OBJ_FILES}
	x86_64-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^

debug: os-image.img kernel.elf
	qemu-system-i386 -s -S -fda os-image.img -d guest_errors,int &
	i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	x86_64-elf-gcc -g -m32 -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

%.dis: %.bin
	ndisasm -b 32 $< > $@

clean:
	$(RM) kernel.img
	$(RM) *.o *.dis *.elf
	$(RM) kernel/*.o
	$(RM) boot/*.o boot/*.bin
	$(RM) drivers/*.o
	$(RM) cpu/*.o
	$(RM) lib/*.o
	$(RM) kernel/core/*.o

compress:
ifeq ($(shell test -e os-image.img && echo -n yes),yes)
	zip -9 "os-image.zip" os-image.img
else
	$(MAKE) build
	zip -9 "os-image.zip" os-image.img
endif