
#ifndef APIC_H
#define APIC_H

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#include <stdint.h>

void cpu_set_apic_base(uintptr_t apic);
uintptr_t cpu_get_apic_base();
void enable_apic();

#endif