/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: kheap.c
*/

#include <kheap.h>
#include <paging.h>
#include <hal/panic.h>

heap_t *kheap;

extern u32 end;
u32 placement_addr = (u32) &end;

static int find_smallest_hole(u32 size, u8 align, heap_t *heap) {
    u32 iter = 0;

    while (iter < heap->index.size) {
        header_t *head = (header_t *) lookup_oarr(iter, &heap->index);

        if (align > 0) {
            u32 pos = (u32) head;
            int off = 0;

            if (((pos + sizeof(header_t)) & 0xFFFFF000) != 0) {
                off = 0x1000 - (pos + sizeof(header_t)) % 0x1000;
            }
            int hole_size = head->size - off;
            if (hole_size >= size) {
                break;
            }
        } else if (head->size >= size) {
            break;
        }

        iter++;
    }

    if (iter == heap->index.size) {
        return -1;
    }
    return iter;
}

//header less than
static char head_lt(void *a, void *b) {
    return (((header_t *) a)->size < ((header_t *) b)->size) ? 1 : 0;
}

static void expand(u32 size, heap_t *heap) {
    ASSERT(size > heap->end - heap->start);

    if ((size & 0xFFFFF000) != 0) {
        size &= 0xFFFFF000;
        size += 0x1000;
    }
    ASSERT(heap->start + size <= heap->max);

    u32 old = heap->end - heap->start;
    u32 i = old;
    while (i < size) {
        alloc_frame(get_page(heap->start + i, 1, kernel_dir), (heap->supervisor) ? 1 : 0, (heap->ro) ? 0 : 1);
        i += 0x1000;
    }

    heap->end = heap->start + size;
}

static u32 contract(u32 size, heap_t *heap) {
    ASSERT(size < heap->end - heap->start);

    if (size & 0x1000) {
        size &= 0x1000;
        size += 0x1000;
    }

    if (size < HEAP_MIN_SZ) {
        size = HEAP_MIN_SZ;
    }
    u32 old = heap->end - heap->start;
    u32 i = old - 0x1000;
    while (size < i) {
        free_frame(get_page(heap->start + i, 0, kernel_dir));
        i -= 0x1000;
    }

    heap->end = heap->start + size;
    return size;
}

u32 kmalloc_int(u32 sz, int align, u32 *phy) {
    if (kheap != 0) {
        void *addr = alloc(sz, (u8) align, kheap);
        if (phy != 0) {
            page_t *page = get_page((u32) addr, 0, kernel_dir);
            *phy = page->frame * 0x1000 + ((u32) addr & 0xFFF);
        }
        return (u32) addr;
    } else {
        if (align == 1 && (placement_addr & 0xFFFFF000)) {
            placement_addr &= 0xFFFFF000;
            placement_addr += 0x1000;
        }
        if (phy) {
            *phy = placement_addr;
        }
        u32 tmp = placement_addr;
        placement_addr += sz;
        return tmp;
    }
}

u32 kmalloc_a(u32 sz) {
    return kmalloc_int(sz, 1, 0);
}

u32 kmalloc_p(u32 sz, u32 *phy) {
    return kmalloc_int(sz, 0, phy);
}

u32 kmalloc_ap(u32 sz, u32 *phy) {
    return kmalloc_int(sz, 1, phy);
}

u32 kmalloc(u32 sz) {
    return kmalloc_int(sz, 0, 0);
}

heap_t *mkheap(u32 start, u32 end, u32 max, u8 supervisor, u8 ro) {
    heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));

    ASSERT(start % 0x1000 == 0);
    ASSERT(end % 0x1000 == 0);

    heap->index = place_oarr((void *) start, HEAP_INDEX_SZ, &head_lt);
    start += sizeof(type_t) * HEAP_INDEX_SZ;

    if ((start & 0xFFFFF000) != 0) {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    heap->start      = start;
    heap->end        = end;
    heap->max        = max;
    heap->supervisor = supervisor;
    heap->ro         = ro;

    header_t *hole = (header_t *) start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->hole = 1;
    ins_oarr((void *) hole, &heap->index);

    return heap;
}

void *alloc(u32 size, u8 align, heap_t *heap) {
    u32 new_size = size + sizeof(header_t) + sizeof(footer_t);
    int iter = find_smallest_hole(new_size, align, heap);

    if (iter == -1) {
        u32 old = heap->end - heap->start;
        u32 old_end = heap->end;

        expand(old + new_size, heap);
        u32 new = heap->end - heap->start;

        iter = 0;
        u32 idx = -1;
        u32 val = 0x0;

        while (iter < heap->index.size) {
            u32 tmp = (u32) lookup_oarr(iter, &heap->index);
            if (tmp > val) {
                val = tmp;
                idx = iter;
            }
            iter++;
        }

        if (idx == -1) {
            header_t *head = (header_t *) old_end;
            head->magic = HEAP_MAGIC;
            head->size = new - old;
            head->hole = 1;
            
            footer_t *foot = (footer_t *) (old_end + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->head = head;

            ins_oarr((void *) head, &heap->index);
        } else {
            header_t *head = lookup_oarr(idx, &heap->index);
            head->size = new - old;

            footer_t *foot = (footer_t *) ((u32) head + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->head = head;
        }

        return alloc(size, align, heap);
    }

    header_t *orig_head = (header_t *) lookup_oarr(iter, &heap->index);
    u32 orig_pos = (u32) orig_head;
    u32 orig_sz = orig_head->size;

    if (orig_sz - new_size < sizeof(header_t) + sizeof(footer_t)) {
        size += orig_sz - new_size;
        new_size = orig_sz;
    }

    if (align && orig_pos & 0xFFFFF000) {
        u32 new_pos = orig_pos + 0x1000 - (orig_pos & 0xFFF) - sizeof(header_t);
        
        header_t *head = (header_t *) orig_pos;
        head->size = 0x1000;
        head->magic = HEAP_MAGIC;
        head->hole = 1;

        footer_t *foot = (footer_t *) ((u32) new_pos - sizeof(footer_t));
        foot->magic = HEAP_MAGIC;
        foot->head = head;
        orig_pos = new_pos;
        orig_sz -= head->size;
    } else {
        rm_oarr(iter, &heap->index);
    }

    header_t *blk = (header_t *) orig_pos;
    blk->magic = HEAP_MAGIC;
    blk->hole = 0;
    blk->size = new_size;

    footer_t *blk_footer = (footer_t *) (orig_pos + sizeof(header_t) + size);
    blk_footer->magic = HEAP_MAGIC;
    blk_footer->head = blk;

    if (orig_sz - new_size > 0) {
        header_t *head = (header_t *) (orig_pos + sizeof(header_t) + size + sizeof(footer_t));
        head->magic = HEAP_MAGIC;
        head->hole = 1;
        head->size = orig_sz - new_size;

        footer_t *foot = (footer_t *) ((u32) head + orig_sz - new_size - sizeof(footer_t));
        if ((u32) foot < heap->end) {
            foot->magic = HEAP_MAGIC;
            foot->head = head;
        }

        ins_oarr((void *) head, &heap->index);
    }

    return (void *) ((u32) blk + sizeof(header_t));
}

static void free(void *p, heap_t *heap) {
    if (p == 0) {
        return;
    }

    header_t *head = (header_t *) ((u32)p -sizeof(header_t));
    footer_t *foot = (footer_t *) ((u32)head + head->size - sizeof(footer_t));

    ASSERT(head->magic == HEAP_MAGIC);
    ASSERT(foot->magic == HEAP_MAGIC);

    head->hole = 1;
    char do_add = 1;

    footer_t *testf = (footer_t *) ((u32) head - sizeof(footer_t));
    if (testf->magic == HEAP_MAGIC && testf->head->hole == 1) {
        u32 cache = head->size;
        head = testf->head;
        foot->head = head;
        head->size += cache;
        do_add = 0;
    }

    header_t *testh = (header_t *) ((u32) foot + sizeof(footer_t));
    if (testh->magic == HEAP_MAGIC && testh->hole == 1) {
        head->size += testh->size;
        testf = (footer_t *) ((u32) testh + testh->size - sizeof(footer_t));

        foot = testf;
        u32 iter = 0;
        while ((iter < heap->index.size) && (lookup_oarr(iter, &heap->index) != (void *) testh)) {
            iter++;
        }

        ASSERT(iter < heap->index.size);
        rm_oarr(iter, &heap->index);
    }

    if ((u32) foot + sizeof(footer_t) == heap->end) {
        u32 old = heap->end - heap->start;
        u32 new = contract((u32) head - heap->start, heap);

        if (head->size - (old - new) > 0) {
            head->size -= old - new;
            foot = (footer_t *) ((u32) head + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->head = head;
        } else {
            u32 iter = 0;
            while ((iter < heap->index.size) && (lookup_oarr(iter, &heap->index) != (void *) testh)) {
                iter++;
            }

            if (iter < heap->index.size) {
                rm_oarr(iter, &heap->index);    
            }
        }
    }

    if (do_add == 1) {
        ins_oarr((void *) head, &heap->index);
    }
}

void kfree(void *p) {
    free(p, kheap);
}