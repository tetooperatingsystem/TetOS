#include "ports.h"
#include <stdint.h>
#include "drive_io.h"
#include <stddef.h>
#include "../terminal/terminal.h"

int ata_wait() {
    uint8_t status;

    while ((status = inb(0x1f7)) & 0x80) {
    };

    if (status & 0x1) return -1;    // ERR
    if (status & 0x20) return -1;   // DF

    while (!(status & 0x8))
        status = inb(0x1f7);

    return 0;
}

void ata_wait_busy() {
    while (inb(0x1F7) & 0x80);
}

int ata_wait_drq() {
    uint8_t status;

    while (1) {
        status = inb(0x1f7);

        if (!(status & 0x80)) {
            if (status & 0x01) return -1; // ERR
            if (status & 0x20) return -1; // DF
            if (status & 0x08) return 0;  // DRQ ready
        }
    }

    return 0;
}

void ata_read_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive) {

    outb(0x1F6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));

    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);

    outb(0x1F2, sector_count);

    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));

    outb(0x1F7, 0x20);   // READ

    for (size_t s = 0; s < sector_count; s++) {

        if (ata_wait_drq() != 0) {
            uint8_t status = inb(0x1f7);
            //debug("[ATA]: READ ERROR: ", status, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
            return;
        }

        for (size_t i = 0; i < 256; i++) {
            ((uint16_t*)buffer)[s*256 + i] = inw(0x1F0);
        }
    }
}

void ata_write_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive) {
    ata_wait_busy();

    outb(ATA_DRIVE_HEAD, drive | 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT, sector_count);
    outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_WRITE);

    uint16_t* data = (uint16_t*) buffer;

    for (int s = 0; s < sector_count; s++) {
        ata_wait_drq();

        for (int i = 0; i < 256; i++) {
            outw(ATA_DATA, data[s * 256 + i]);
        }
    }

    outb(ATA_COMMAND, ATA_CMD_CACHE_FLUSH);

    ata_wait_busy();
}