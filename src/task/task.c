/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: task.c
*/

#include <tss.h>
#include <kheap.h>
#include <stdio.h>
#include <string.h>
#include <task/task.h>

extern u32 initial_esp;
extern u32 read_eip();
extern u32 read_edx();

task_t *crt_task;
task_t *rqueue;

u32 next_pid = 1;

void move_stack(void *new_stack_start, u32 size) {
    u32 i;

    for (i = (u32) new_stack_start; i >= ((u32) new_stack_start - size); i -= 0x1000) {
        alloc_frame(get_page(i, 1, crt_dir), 0, 1);
    }

    u32 pd_addr, old_stack_ptr, old_base_ptr;
    asm volatile("mov %%cr3, %0" : "=r"(pd_addr));
    asm volatile("mov %0, %%cr3" :: "r"(pd_addr));
    asm volatile("mov %%esp, %0" : "=r"(old_stack_ptr));
    asm volatile("mov %%ebp, %0" : "=r"(old_base_ptr));

    u32 off = (u32) new_stack_start - initial_esp;
    u32 new_stack_ptr = old_stack_ptr + off;
    u32 new_base_ptr = old_base_ptr + off;
    memcpy((void *) new_stack_ptr, (void *) old_stack_ptr, initial_esp - old_stack_ptr);

    for (i = (u32) new_stack_start; i > (u32) new_stack_start - size; i -= 4) {
        u32 tmp = *(u32 *) i;
        if ((old_stack_ptr < tmp) && (tmp < initial_esp)) {
            tmp += off;
            u32 *tmp2 = (u32 *) i;
            *tmp2 = tmp;
        }
    }

    asm volatile("mov %0, %%esp" :: "r"(new_stack_ptr));
    asm volatile("mov %0, %%ebp" :: "r"(new_base_ptr));
}

void init_tasking() {
    asm volatile("cli");

    move_stack((void *) 0xE0000000, 0x2000);
    crt_task = rqueue = (task_t *) kmalloc(sizeof(task_t));
    crt_task->id = next_pid++;
    crt_task->regs.esp = crt_task->regs.ebp = 0;
    crt_task->regs.eip = 0;
    crt_task->dir = crt_dir;
    crt_task->next = 0;
    crt_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);

    asm volatile("sti");
}

int fork() {
    asm volatile("cli");

    task_t *parent = (task_t *) crt_task;
    pagedir_t *dir = clone_dir(crt_dir);

    task_t *new_task = (task_t *) kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->regs.esp = new_task->regs.ebp = 0;
    new_task->regs.eip = 0;
    new_task->dir = dir;
    new_task->next = 0;
    crt_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);

    task_t *tmp_task = (task_t *) rqueue;
    while (tmp_task->next) {
        tmp_task = tmp_task->next;
    }
    tmp_task->next = new_task;

    u32 eip = read_eip();

    if (crt_task == parent) {
        u32 esp;
        u32 ebp;

        asm volatile("mov %%esp, %0" : "=r"(esp));
        asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_task->regs.esp = esp;
        new_task->regs.ebp = ebp;
        new_task->regs.eip = eip;
        asm volatile("sti");

        return new_task->id;
    } else {
        return 0;
    }
}

void task_switch() {
    if (!crt_task) {
        return;
    }

    u32 esp, ebp, eip, edx;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    eip = read_eip();

    if (eip == 0x6969) {
        return; //we have just switched tasks
    }

    crt_task->regs.eip = eip;
    crt_task->regs.esp = esp;
    crt_task->regs.ebp = ebp;

    crt_task = crt_task->next;
    if (!crt_task) {
        crt_task = rqueue;
    }

    esp = crt_task->regs.esp;
    ebp = crt_task->regs.ebp;
    //eip = crt_task->regs.eip;

    crt_dir = crt_task->dir;
    set_kernel_stack(crt_task->kernel_stack + KERNEL_STACK_SIZE);

    asm volatile(" \
        cli; \
        mov %0, %%ecx; \
        mov %1, %%esp; \
        mov %2, %%ebp; \
        mov %3, %%cr3; \
        mov $0x6969, %%eax; \
        sti; \
        jmp *%%ecx" :: "r"(eip), "r"(esp), "r"(ebp), "r"(crt_dir->addr));
}

int getpid() {
    return crt_task->id;
}

void switch_to_usermode() { //not working
    set_kernel_stack(crt_task->kernel_stack + KERNEL_STACK_SIZE);

    asm volatile(" \
        cli;            \
        mov $0x23, %ax; \
        mov %ax, %ds;   \
        mov %ax, %es;   \
        mov %ax, %fs;   \
        mov %ax, %gs;   \
        mov %esp, %eax; \
        pushl $0x23;    \
        pushl %eax;     \
        pushf;          \
        pop %eax;       \
        or $0x200, %eax;\
        push %eax;      \
        pushl $0x1B;    \
        push $1f;       \
        iret;           \
        1:"); /* we cannot enable interrupts with sti again because we will get a gpf (L159-161)*/
}