#include <stdint.h>

#include <boot/requests.h>

uintptr_t phys_to_virt(uintptr_t phys)
{
    return phys + hhdm_request.response->offset;
}

uintptr_t virt_to_phys(uintptr_t virt)
{
    return virt - hhdm_request.response->offset;
}
