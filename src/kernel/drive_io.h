//
// Simple Drive Input/Output library for protected mode using ATA
//

//       ATA Channels
//     /            \
//   0x1F0         0x170
//   (Primary)      (Secondary)
//       ATA Channel
//     /            \
//    Master        Slave
//
// We can have 4 drives at once.
//
// Primary master - 0xe0 IN 0x1F0
// Primary slave - 0xf0 IN 0x1F0
// Secondary master - 0x170
// Secondary slave - 0x170

#include "ports.h"
#include <stdint.h>

#define ATA_MASTER 0x0
#define ATA_SLAVE  0x1

#define ATA_DATA         0x1F0
#define ATA_ERROR        0x1F1
#define ATA_FEATURES     0x1F1
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW      0x1F3
#define ATA_LBA_MID      0x1F4
#define ATA_LBA_HIGH     0x1F5
#define ATA_DRIVE_HEAD   0x1F6
#define ATA_STATUS       0x1F7
#define ATA_COMMAND      0x1F7
#define ATA_CMD_CACHE_FLUSH 0xE7

#define ATA_CMD_WRITE  0x30

typedef enum {
    PRIMARY_D,
    SECONDARY_D
} drive_type;

// Wait until the drive is not busy anymore.
int ata_wait();

void ata_wait_busy();

int ata_wait_drq();

void ata_read_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive);
void ata_write_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive);