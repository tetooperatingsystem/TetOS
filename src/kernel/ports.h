//
// Just a small file for writing to ports.
//

#include <stdint.h>

void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
uint8_t inb(unsigned short port);
uint16_t inw(uint16_t port);