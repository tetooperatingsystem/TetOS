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

int ata_wait_drq(uint16_t io) {
    uint8_t status;

    while (1) {
        status = inb(io+ATA_STATUS);

        if (!(status & 0x80)) {
            if (status & 0x01) return -1; // ERR
            if (status & 0x20) return -1; // DF
            if (status & 0x08) return 0;  // DRQ ready
        }
    }

    return 0;
}

void ata_read_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive) {
    uint16_t io;
    uint16_t ctrl;
    uint16_t slave;

    if (drive < 2) {
        io=ATA_PRIMARY_IO;
        ctrl=ATA_PRIMARY_CTRL;
    }
    else {
        io=ATA_SECONDARY_IO;
        ctrl=ATA_SECONDARY_CTRL;
    }

    slave=drive&1;

    outb(io+ATA_DRIVE_HEAD,0xE0 | (slave << 4) | ((lba >> 24) & 0xF));

    outb(io+ATA_SECTOR_COUNT,sector_count);
    outb(io+ATA_LBA_LOW,(uint8_t)lba);
    outb(io+ATA_LBA_MID,(uint8_t)(lba>>8));
    outb(io+ATA_LBA_HIGH,(uint8_t)(lba>>16));

    outb(io+ATA_COMMAND,ATA_CMD_READ);

    // 400ns delay
    inb(ctrl);
    inb(ctrl);
    inb(ctrl);
    inb(ctrl);

    uint16_t *buf = (uint16_t*) buffer;
    for (uint32_t s = 0; s < sector_count; s++) {
        if (ata_wait_drq(io)!=0) {
            return;
        }
        for (uint32_t i=0;i<256;i++) {
            buf[s*256+i] = inw(io);
        }
    }
}

/*
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
        ata_wait_drq(0);

        for (int i = 0; i < 256; i++) {
            outw(ATA_DATA, data[s * 256 + i]);
        }
    }

    outb(ATA_COMMAND, ATA_CMD_CACHE_FLUSH);

    ata_wait_busy();
}
*/