#include <stdint.h>
#include <stddef.h>

#include <kprintf.h>

#include <boot/requests.h>

#include <mm/pmm/pmm.h>

// Orders start at 0, so PMM_MAX_ORDER being 10 gives us 11 orders
// free_lists[n].head would give us the first free block of order n
static struct pmm_free_list free_lists[PMM_MAX_ORDER + 1];

static void pmm_add_block(uintptr_t phys, uint8_t order)
{
    struct pmm_block *head = free_lists[order].head;
    struct pmm_block *block = (struct pmm_block *)phys;

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
    return (uintptr_t)head;
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
    uint64_t  block_size = 4096 * (1UL << order);
    uintptr_t buddy_phys = phys ^ block_size;

    // head points to the first free block in the list
    struct pmm_block *current = free_lists[order].head; 

    while (current != nullptr)
    {
        uintptr_t current_phys = (uintptr_t)current;
        if (current_phys == buddy_phys)
        {
            // 
        }
        current = current->next;
    }
}

static uint64_t memmap_entry_count;
static struct limine_memmap_entry **memmap_entries;

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

        kprintf("[PMM] Found usable memory region %llx-%llx (%llu kilobytes)\n", current_entry->base, current_entry->base + current_entry->length, (current_entry->base + current_entry->length - current_entry->base) / 4096);
    }
}
