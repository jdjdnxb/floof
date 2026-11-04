#include <stdint.h>

#include <arch/x86_64/idt/idt.h>

#include <kprintf.h>
#include <panic.h>

void exception_divide_error(struct interrupt_frame *frame)
{
    panic("Exception: Divide Error.", frame);
}

void exception_non_maskable_interrupt(struct interrupt_frame *frame)
{
    panic("Exception: NMI.", frame);
}

void exception_overflow(struct interrupt_frame *frame)
{
    panic("Exception: Overflow.", frame); 
}

void exception_invalid_opcode(struct interrupt_frame *frame)
{
    panic("Exception: Invalid Opcode.", frame); 
}

void exception_device_not_available(struct interrupt_frame *frame)
{
    panic("Exception: Device Not Available.", frame);
}

void exception_double_fault(struct interrupt_frame *frame)
{
    panic("Exception: Double Fault.", frame);
}

void exception_invalid_tss(struct interrupt_frame *frame)
{
    panic("Exception: Invalid TSS.", frame);
}

void exception_general_protection(struct interrupt_frame *frame)
{
    panic("Exception: General Protection.", frame);
}

void exception_page_fault(struct interrupt_frame *frame)
{
    panic("Exception: Page Fault.", frame);

}

void exception_control_protection(struct interrupt_frame *frame)
{
    panic("Exception: Control Protection.", frame);
}
