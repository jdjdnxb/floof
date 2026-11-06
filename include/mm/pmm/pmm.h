// For the PMM, we're using a buddy allocator
// https://www.geeksforgeeks.org/operating-systems/buddy-system-memory-allocation-technique/
#pragma once

#define PAGE_SIZE 4096
#define PAGE_MASK   (~(PAGE_SIZE - 1))

#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(addr) ((addr) & PAGE_MASK)

// *next points to the next element in the linked list 
struct pmm_block
{
    struct pmm_block *next;
};

// List of all free blocks
// *head points to the first free block in the list.
// To remove one element from the linked list: head = head->next;
struct pmm_free_list
{
    struct pmm_block *head;
};

// Max order that is valid (biggest block is 4MiB)
#define PMM_MAX_ORDER 10

void pmm_init(void);
