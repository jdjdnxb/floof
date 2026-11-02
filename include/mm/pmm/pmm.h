// For the PMM, we're using a buddy allocator
// https://www.geeksforgeeks.org/operating-systems/buddy-system-memory-allocation-technique/
#pragma once

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

#define PMM_MAX_ORDER 10

void pmm_init(void);
