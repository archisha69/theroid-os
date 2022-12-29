MBOOT_PAGE_ALIGN equ 1 << 0
MBOOT_MEM_INFO   equ 1 << 1

MAGIC      equ 0x1BADB002
FLAGS      equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
CHECKSUM   equ -(MAGIC + FLAGS)

bits 32

global mboot
extern code
extern bss
extern end

mboot:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd mboot
	dd code
	dd bss
	dd end
	dd start

global start
extern start_kernel

start:
	push esp
	push ebx

	cli
	call start_kernel
	jmp $