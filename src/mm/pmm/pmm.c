#include <stdint.h>
#include <stddef.h>

#include <kprintf.h>

#include <boot/requests.h>

#include <mm/pmm/pmm.h>
#include <mm/hhdm.h>

// Orders start at 0, so PMM_MAX_ORDER being 10 gives us 11 orders
// free_lists[n].head would give us the first free block of order n
static struct pmm_free_list free_lists[PMM_MAX_ORDER + 1];

static void pmm_add_block(uintptr_t phys, uint8_t order)
{
    struct pmm_block *head = free_lists[order].head;
    struct pmm_block *block = (struct pmm_block *)phys_to_virt(phys);

    block->next = head;
    free_lists[order].head = block;
}

static uintptr_t pmm_remove_block(uint8_t order)
{
    struct pmm_block *head = free_lists[order].head;
    if (head == nullptr)
    {
        return 0;
    }
    free_lists[order].head = head->next;
    // Old head
    return virt_to_phys((uintptr_t)head);
}

// phys is the physical address of a block's start 
// NOTE: this function only handles a singular split
static uintptr_t pmm_split_block(uintptr_t phys, uint8_t order)
{
    // Order 0 represents a single page (4KiB), therefore return the address the caller passed
    if (order == 0)
    {
        return phys;
    }
    uint64_t block_size = 4096 * (1UL << order);

    // To split the block into 2, we have to find out the size of a half and add it to the original physical address
    uint64_t  block_half_size   = block_size / 2;
    uintptr_t block_half        = phys;
    uintptr_t block_second_half = phys + block_half_size;

    // Orders start at 0
    pmm_add_block(block_second_half, order - 1);

    return block_half;
}

// phys is the physical address of the block being freed
// NOTE: this function handles merging until it stops being possible
static uintptr_t pmm_merge_block(uintptr_t phys, uint8_t order)
{
    uintptr_t final_block_phys = phys;
    uint8_t   current_order = order;

    while (current_order < PMM_MAX_ORDER)
    {
        uint64_t  block_size = 4096 * (1UL << current_order);
        uintptr_t buddy_phys = final_block_phys ^ block_size;

        // head points to the first free block in the list
        struct pmm_block *current = free_lists[current_order].head; 
        struct pmm_block *prev    = nullptr;

        bool found = false;

        while (current != nullptr)
        {
            uintptr_t current_phys = virt_to_phys((uintptr_t)current);
            if (current_phys == buddy_phys)
            {
                if (prev == nullptr)
                {
                    // Buddy is the head
                    free_lists[current_order].head = current->next;
                }
                else
                {
                    // Buddy is somewhere else
                    prev->next = current->next; 
                }

                if (buddy_phys < final_block_phys)
                {
                    final_block_phys = buddy_phys;
                }

                found = true;
                break;
            }

            prev = current;
            current = current->next;
        }

        if (!found)
            break;

        current_order++;
    }
    return final_block_phys;
}

static uint64_t memmap_entry_count;
static struct limine_memmap_entry **memmap_entries;

static void pmm_init_region(uintptr_t base, uint64_t length)
{
    uintptr_t current = PAGE_ALIGN(base);
    uintptr_t end     = PAGE_ALIGN_DOWN(base + length);

    while (current < end)
    {
        uint8_t order = PMM_MAX_ORDER;

        while (order > 0)
        {
            uint64_t block_size = 4096ULL << order;

            if (block_size <= end - current && current % block_size == 0)
            {
                break;
            }

            order--;
        }

        pmm_add_block(current, order);

        current += 4096ULL << order;
    }
}

void pmm_init(void)
{
    memmap_entry_count = memmap_request.response->entry_count;
    memmap_entries = memmap_request.response->entries;
    kprintf("[PMM] Memory map entry count: %llu\n", memmap_entry_count);

    for (uint64_t i = 0; i < memmap_entry_count; i++)
    {
        struct limine_memmap_entry *current_entry = memmap_entries[i];

        if (current_entry->type != LIMINE_MEMMAP_USABLE)
            continue;
        
        pmm_init_region(current_entry->base, current_entry->length);

        kprintf("[PMM] Found and initialized usable memory region %llx-%llx (%llu kilobytes)\n", 
                current_entry->base, current_entry->base + current_entry->length, 
                (current_entry->base + current_entry->length - current_entry->base) 
                / 4096);
    }
}
