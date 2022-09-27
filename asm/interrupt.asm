; Copyright (c) 2022 thatOneArchUser
; All rights reserved

[extern isr_handler]
[extern irq_handler]

%macro register_isr 1
    global isr%1
    isr%1:
        push byte 0
        push byte %1
        jmp isr_common_stub
%endmacro

%macro register_irq 1
    global irq%1
    irq%1:
        push byte %1
        push byte %1 + 32
        jmp irq_common_stub
%endmacro

isr_common_stub:
	pusha
	mov ax, ds
	push eax
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp
	call isr_handler
	pop eax

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8
	iret

irq_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    pop ebx

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret

; division by 0
register_isr 0

; debug
register_isr 1

; non maskable interrupt
register_isr 2

; breakpoint
register_isr 3

; into detected overflow
register_isr 4

; out of bounds
register_isr 5

; invalid opcode
register_isr 6

; no coprocessor
register_isr 7

; double fault
register_isr 8

; coprocessor segment overrun
register_isr 9

; bad tss
register_isr 10

; segment not present
register_isr 11

; stack fault
register_isr 12

; general protection fault
register_isr 13

; page fault
register_isr 14

; unknown interrupt
register_isr 15

; coprocessor fault
register_isr 16

; alignment check
register_isr 17

; machine check
register_isr 18

; reserved
register_isr 19
register_isr 20
register_isr 21
register_isr 22
register_isr 23
register_isr 24
register_isr 25
register_isr 26
register_isr 27
register_isr 28
register_isr 29
register_isr 30
register_isr 31

register_irq 0
register_irq 1
register_irq 2
register_irq 3
register_irq 4
register_irq 5
register_irq 6
register_irq 7
register_irq 8
register_irq 9
register_irq 10
register_irq 11
register_irq 12
register_irq 13
register_irq 14
register_irq 15
