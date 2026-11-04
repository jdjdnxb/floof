#pragma once

#include <stdint.h>

// HHDM allows us to easily access physical memory through virtual addresses
// Virtual HHDM address = Physical base address + HHDM offset
// We receive the HHDM offset from Limine (boot/requests.h)
uintptr_t phys_to_virt(uintptr_t phys);
uintptr_t virt_to_phys(uintptr_t virt);
