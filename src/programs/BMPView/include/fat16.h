
#ifndef _FAT16_H
#define _FAT16_H

#define ATA_SLAVE 0x1
#define ATA_MASTER 0x0

typedef unsigned long uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned short size_t;
typedef unsigned int uint;

// Port I/O

void outb(uint16_t port, uint8_t val);

void outw(uint16_t port, uint16_t val);

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

// Drive IO
void ata_read_sectors(uint32_t lba, char* buffer, uint8_t sector_count, uint8_t drive);


// FAT16 Variables

typedef struct {
    char jmp[3];                    // JMP instruction for bootable drives
    char OEM[8];                    // Drive OEM
    uint16_t bytes_per_sector;      // Bytes in a sector (usually 512 or rarely 4096)
    uint8_t sectors_per_cluster;    // Sectors in a cluster
    uint16_t reserved_sectors;      // Reserved for FAT, BBPB and EBPB
    uint8_t fat_count;              // Amount of FATs(Usually 2, main and backup)
    uint16_t count_root_dirs;       // Amount of Root Directories (Usually 1)
    uint16_t logical_sectors_1;     // Sectors in a logical volume. If it is 0, use logical_sectors_2
    uint8_t media_desc_type;        // Misc
    uint16_t sectors_per_fat;       // How many sectors in a FAT
    uint16_t sectors_per_track;     // Sectors in a track
    uint16_t head_count;            // Amount of heads
    uint32_t hidden_sector_count;   // Count of hidden sectors
    uint32_t logical_sectors_2;     // Sectors in a logical volume, but in a 32-bit integer.
} __attribute__((packed)) bpb_t;

typedef struct {
    uint8_t drive_number;           // BIOS drive number
    uint8_t flags_windows;           // Flags used by Windows NT.
    uint8_t signature;              // 0x28 or 0x29
    uint32_t volume_id;             // Volume serial number
    char volume_label[11];          // Volume label
    char sys_identifier[8];          // System Identifier String
    uint8_t boot_code[448];         // Bootloader for bootable drives. It is called by the 3 beginning bytes in BPB
    uint16_t bootable_part_sign;    // Bootable Partition Signature. 0xAA55
} __attribute__((packed)) ebpb_t;

typedef struct {
    uint8_t first_root_sector;
    uint root_dir_sectors;
    uint first_data_sector;
    uint cluster_count;
} __attribute__((packed)) f16_t;

// Files

typedef struct {
    char filename[11];                // Filename
    uint8_t file_attributes;          // Attributes
    uint8_t nt_reserve;              // Reserved by Windows NT

    uint8_t creation_time_hund;      // Creation time in hundreds of a second.
    uint16_t creation_time_hms2;     // Hour, minute, second / 2
    uint16_t creation_date;          // YYYY MM DD
    uint16_t last_access_date;       // YYYY MM DD

    uint16_t highword_cluster_index; // This is for FAT32; Unused on FAT16

    uint16_t modification_time;       // Last time modified. Hour, minute, second.
    uint16_t modification_date;       // Last date modified. Year, month, day
    
    short   first_cluster_number;     // First cluster number with data. (for directories, it has a bunch of these entries)
    uint32_t size_in_bytes;          // Size of the file in bytes.
} __attribute__((packed)) directoryentry_t;

// Functions with Fat16

void read_file(char* buffer, const char* path, uint8_t drive);

void fat16_init(int DRIVE);

int strlen(char* a);



#endif