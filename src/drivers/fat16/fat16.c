#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "fat16.h"
#include "../../kernel/drive_io.h"
#include "../../mem.h"
#include "../../terminal/terminal.h"

BPB bpb;
EBPB ebpb;

struct F16_DATA f16_data;

#define MAX_FAT_SIZE (512 * 128)

extern uint16_t terminal_column;
extern uint16_t terminal_row;

uint8_t F16_DRIVE;

static char f16_sector[512];

unsigned char FAT_TABLE[MAX_FAT_SIZE];

uint16_t get_fat_entry(uint16_t cluster) {
    return *(uint16_t*)&FAT_TABLE[cluster * 2] & 0xFFFF;
}

void fat16_init(uint8_t drive) {
    char buff[512];

    F16_DRIVE = drive;

    ata_read_sectors(0, buff, 1, drive);
    
    // Read BPB and EBPB
    memcpy((void*)&bpb, buff, sizeof(BPB));
    memcpy((void*)&ebpb, &buff[sizeof(BPB)], sizeof(EBPB));

    // Read FAT

    int index = bpb.ReservedSectors;

    ata_read_sectors(index, (char*) FAT_TABLE, bpb.SectorsPerFAT, F16_DRIVE);

    f16_data.FirstRootSector = bpb.ReservedSectors + (bpb.SectorsPerFAT * bpb.FATAmount);
    f16_data.RootDirSectors = ((uint32_t) bpb.RootDirectoriesAmount * 32u + (uint32_t) bpb.BytesPerSector - 1u) / (uint32_t)  bpb.BytesPerSector;
    f16_data.FirstDataSector = bpb.ReservedSectors + (bpb.FATAmount * bpb.SectorsPerFAT) + f16_data.RootDirSectors;
}

 int separatebyslashes(char* str, char dir[32][32]) {
    int pos = 0;

    memset(dir, 0, 1024);

    int j = 0;
    int len = strlen(str);
    // Iterate until the last character
    for (int i = 0; str[i] != '\0'; i++) {
        // If the character is / or \, separate
        if (i == len) break; // just to make sure cuz i hate C
        if (str[i] == '/' || str[i] == '\\') {
            dir[pos][j] = 0;
            pos++;
            j = 0;
        }
        // Else, add the character
        else {
            dir[pos][j] = str[i];
            j++;
        }
    }
    
    dir[pos][j] = 0;
    return pos + 1;
}


void format_name(const char* input, char out[11]) {
    for (int i = 0; i < 11; i++) out[i] = ' ';

    int i = 0, j = 0;

    while (input[i] && input[i] != '.' && j < 8) {
        char c = input[i];

        // To uppercase
        if (c >= 'a' && c <= 'z') c -= 32;

        out[j++] = c;
        i++;
    }
    if (input[i] == '.') i++;

    j = 8;

    while (input[i] && j < 11) {
        char c = input[i];

        if (c >= 'a' && c <= 'z') c -= 32;
        out[j++] = c;
        i++;
    }
}

DirectoryEntry find_file(char* filepath) {
    char dirs[32][32];
    int count = separatebyslashes(filepath, dirs);

    int max_per_sector = bpb.BytesPerSector / 0x20;

    DirectoryEntry cur;
    memset(&cur, 0, 0x20);

    cur.first_cluster_number = 0;
    cur.size_bytes = f16_data.RootDirSectors * bpb.BytesPerSector;

    char sector[512];

    for (int i = 0; i < count; i++) {
        char* dirname = dirs[i];
        char padded[11];
        format_name(dirname, padded);

        int found = 0;

        uint32_t start_sector;
        // Is root
        if (cur.first_cluster_number == 0) start_sector = f16_data.FirstRootSector;
        else start_sector = f16_data.FirstDataSector + (cur.first_cluster_number - 2) * bpb.SectorsPerCluster;

        int sectors_to_scan = (cur.first_cluster_number == 0) ? f16_data.RootDirSectors : bpb.SectorsPerCluster;
        
        for (int s = 0; s < sectors_to_scan && !found; s++) {
            ata_read_sectors(start_sector + s, sector, 1, F16_DRIVE);
            DirectoryEntry* entries = (DirectoryEntry*) sector;
            for (int j = 0; j < max_per_sector; j++) { 
                DirectoryEntry entry = entries[j];

                if ((unsigned char) entry.Filename[0] == 0xE5) continue;
                if (entry.FileAttributes & 0x2) continue;

                // LFN
                if (entry.FileAttributes == 0xF) {
                    continue;
                    char name[40];
                    // LFNtoName(*((LongFileName*) &entry), name);
                    if (memcmp(name, dirs[i], 14)) {
                        cur = entries[j+1];
                        found = 1;
                        break;
                    }
                }

                if (memcmp(entry.Filename, padded, 11)) {
                    cur = entry;
                    found = 1;
                    break;
                }
            }
        }
    }

    return cur;
}

void read_file(char* buffer, const char* path, uint8_t drive) {
    uint16_t* fat = (uint16_t*) FAT_TABLE;
    
    DirectoryEntry file = find_file((char*) path);

    uint16_t cluster = file.first_cluster_number;

    if (cluster < 2) {
        debug("NO CLUSTER: ", cluster, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        return;
    }

    while (cluster < 0xFFF8) {
        int f16_sector = f16_data.FirstDataSector + (cluster - 2) * bpb.SectorsPerCluster;

        ata_read_sectors(f16_sector, buffer, bpb.SectorsPerCluster, drive);

        buffer += bpb.SectorsPerCluster * bpb.BytesPerSector;
        cluster = get_fat_entry(cluster);
    }
    
}

void list_entries_in_dir(const char* path) {
    bool is_root = (strcmp(path, "/"));

    DirectoryEntry dir;

    uint16_t cluster = 0;

    if (!is_root) {
        dir = find_file((char*) path);
        if (!(dir.FileAttributes & 0x10)) {
            printf("NOT DIRECTORY", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
            return;
        }

        cluster = dir.first_cluster_number;
    }

    char sector[512];

    int i = -1;
    do {
        putchar(ebpb.VolumeLabelString[i],terminal_color);
        i++;
    } while (ebpb.VolumeLabelString[i]!=' '&&i<11);

    printf(path,terminal_color);
    putchar('\n',terminal_color);

    while (1) {
        uint32_t first_sector;
        uint32_t sectors_to_read;

        if (is_root) {
            first_sector = f16_data.FirstRootSector;
            sectors_to_read = f16_data.RootDirSectors;
        }
        else {
            if (cluster < 2 || cluster >= 0xFFF8)
                break;

            first_sector = ((cluster - 2) * bpb.SectorsPerCluster) + f16_data.FirstDataSector;
            sectors_to_read = bpb.SectorsPerCluster;
        }

        for (uint32_t s = 0; s < sectors_to_read; s++) {
            ata_read_sectors(first_sector + s, sector, 1, F16_DRIVE);

            DirectoryEntry* entries = (DirectoryEntry*) sector;

            for (int j = 0; j < 16; j++) {
                DirectoryEntry e = entries[j];

                char first_ch  = e.Filename[0];

                char displayed[26] = {0};

                if (first_ch == 0x0) return;
                if ((unsigned char) first_ch == 0xE5) continue;
                if (e.FileAttributes==0x8) {//Volume Label
                    continue;
                }
                if (e.FileAttributes==0xF) {
                    // LFN
                    continue;
                }
                else {
                    memcpy(displayed, e.Filename, 11);
                }

                write(e.Filename, 8, terminal_color);

                if (e.FileAttributes & 0x10) {
                        printf(" <DIR>", terminal_color);
                }
                else {
                    putchar('.', terminal_color);
                    write(&e.Filename[8], 3, terminal_color);
                }
                putchar('\n', terminal_color);
            }
        }
    }
}

void write_file(char* buffer, uint32_t file_size, char* filename) {
    /*
    // Find free cluster

    uint16_t free_cluster = 0;
    uint32_t total_sectors = bpb.SectorsInLogicalVolume16;
    if (!total_sectors) total_sectors = bpb.SectorsInLogicalVolume32;
    for (free_cluster = 2; free_cluster < total_sectors; free_cluster++) {
        if (get_fat_entry(free_cluster) == 0x0) goto free_cluster_found;
    }

    return;

    free_cluster_found:

    uint32_t cluster_size = bpb.SectorsPerCluster * bpb.BytesPerSector;
    uint32_t clusters_needed = (file_size + cluster_size - 1) / cluster_size;

    // supporting max 64 clusters - (512*64*4) around 131072 bytes
    uint16_t clusters[64];
    int count = 0;

    for (uint16_t c = 2; c < f16_data.ClusterCount; c++) {
        if (get_fat_entry(c)==0) {
            clusters[count++]=c;
            if(count==clusters_needed) break;
        }
    }

    if (count != clusters_needed) {
        printf("COULD NOT WRITE TO DISK; DISK FULL.", terminal_color);
        return;
    }

    for (int i = 0; i < clusters_needed - 1; i++) {
        uint32_t fat_offset = clusters[i] * sizeof(uint16_t);
        
        uint32_t sector_offset = fat_offset / bpb.BytesPerSector;
        uint32_t entry_offset = fat_offset % bpb.BytesPerSector;

        uint8_t buffer[512];

        for (uint8_t fat = 0; fat < bpb.FATAmount; fat++) {
            uint32_t fat_sector = bpb.ReservedSectors + fat * bpb.SectorsPerFAT + sector_offset;

            ata_read_sectors(fat_sector, buffer, 1, F16_DRIVE);

            *(uint16_t*)(buffer + entry_offset) = clusters[i+1];
            FAT_TABLE[entry_offset] = clusters[i+1];

            ata_write_sectors(fat_sector, buffer, 1, F16_DRIVE);
        }
    }

    char dir_buff[512]={0};
    */
}
