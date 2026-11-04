#include <stdint.h>
#include <boot/requests.h>

#include <drivers/video/framebuffer.h>
#include <drivers/video/text.h>
#include <drivers/terminal/terminal.h>
#include <drivers/acpi/acpi.h>

#include <arch/x86_64/idt/idt.h>
#include <arch/x86_64/gdt/gdt.h>
#include <arch/x86_64/drivers/apic/lapic.h>

#include <mm/pmm/pmm.h>

#include <kernel/kassert.h>
#include <kprintf.h>

struct terminal kernel_terminal;

void main(void)
{
    framebuffer_init();
    framebuffer_clear(0x00000000);
    terminal_init(&kernel_terminal);
    
    KASSERT(hhdm_request.response != nullptr);
    
    kprintf("HHDM Offset: %llx\n", hhdm_request.response->offset);

    x86_64_gdt_init();
    x86_64_idt_init();

    pmm_init();

    acpi_init();
    lapic_init();

    kprintf("Welcome to floof!\n");
    
    while (1)
    {
        __asm__ volatile("hlt");
    }
}
