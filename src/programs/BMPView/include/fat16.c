
#include "fat16.h"


// Port I/O
void outb(uint16_t port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}


 void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
 }

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Mem...

int memcmp(const char* a, const char* b, size_t size) {
    for (int i = 0; i < size; i++) {
        if (a[i] != b[i]) return 0;
    }

    return 1;
}

void* memcpy(void* dest, const void* src, size_t count) {
    unsigned char* ptr = (unsigned char*) dest;
    unsigned char* ptr2 = (unsigned char*) src;

    for (size_t x = 0; x < count; x++) {
        ptr[x] = ptr2[x];
    }

    return dest;
}

void* memset(void* dest, int val, size_t count) {
    unsigned char* ptr = (unsigned char*) dest;

    for (size_t i = 0; i < count; i++) {
        ptr[i] = (unsigned char) val;
    }

    return dest;
}

// Drive I/O

int ata_wait_drq() {
    uint8_t status;

    do {
        status=inb(0x1f7);
    
        if (!(status&0x80)) {
            if (status&0x01) return -1;     // ERR
            if (status&0x20) return -1;     // DF
            if (status&0x08) return 0;      // DRQ Ready
        }
    } while (1);

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
            return;
        }

        for (size_t i = 0; i < 256; i++) {
            ((uint16_t*)buffer)[s*256 + i] = inw(0x1F0);
        }
    }
}

// FAT16

int F16_DRIVE;

bpb_t bpb;
ebpb_t ebpb;
f16_t f16_d;

#define MAX_FAT_SIZE (512*128)

uint8_t FAT_TABLE[MAX_FAT_SIZE];

void read_fat_t() {
    int index = bpb.reserved_sectors;
    ata_read_sectors(index,FAT_TABLE,bpb.sectors_per_fat,F16_DRIVE);
}

uint16_t get_fat_entry(uint16_t cluster) {
    return *(uint16_t*)&FAT_TABLE[cluster*2]&0xFFFF;
}

void fat16_init(int DRIVE) {
    char buff[512];

    F16_DRIVE = DRIVE;

    ata_read_sectors(0,buff,1,DRIVE); // Read BPB and EBPB

    memcpy((void*)&bpb,buff,sizeof(bpb_t));
    memcpy((void*)&ebpb,&buff[sizeof(bpb_t)],sizeof(ebpb_t));

    read_fat_t();

    f16_d.first_root_sector = bpb.reserved_sectors + (bpb.sectors_per_fat * bpb.fat_count);
    f16_d.root_dir_sectors = ((uint32_t)bpb.count_root_dirs*32u+(uint32_t)bpb.bytes_per_sector-1u) / (uint32_t) bpb.bytes_per_sector;
    f16_d.first_data_sector = bpb.reserved_sectors + (bpb.fat_count*bpb.sectors_per_fat)+f16_d.root_dir_sectors;
}

int strlen(char *str) {
    int i=0;
    do {i++;} while(str[i]);
    return i;
}

int separatebyslashes(char* str, char dir[32][32]) {
    int pos=0;
    memset(dir,0,1024);

    int j = 0;
    int len = strlen(str);

    for (int i=0;str[i];i++) {
        if (i==len)break;
        if (str[i]=='/'||str[i]=='\\') {
            dir[pos][j]=0;
            pos++;
            j=0;
        }
        else {
            dir[pos][j]=str[i];
            j++;
        }
    }

    dir[pos][j]=0;
    return pos+1;
}

void format_name(const char* input, char out[11]) {
    for (int i=0;i<11;i++)out[i]=' ';
    int i=0,j=0;

    while (input[i]&&input[i]!='.'&&j<8) {
        char c = input[i];

        // To uppercase
        if (c>='a' && c<='z') c-=32;

        out[j++]=c;
        i++;
    }
    if (input[i]=='.') i++;
    j=8;
    while (input&&j<11) {
        char c=input[i];

        if (c>='a'&&c<='z') c-=32;
        out[j++]=c;
        i++;
    }
}

directoryentry_t find_file(char* filepath) {
    char dirs[32][32];
    int count=separatebyslashes(filepath,dirs);
    int max_per_sector=bpb.bytes_per_sector/0x20;

    directoryentry_t cur;
    memset(&cur,0,0x20);

    cur.first_cluster_number=0;
    cur.size_in_bytes=f16_d.root_dir_sectors*bpb.bytes_per_sector;

    char sector[512];

    for (int i=0;i<count;i++) {
        char* dirname=dirs[i];
        char padded[11];
        format_name(dirname,padded);
        int found=0;

        uint32_t start_sector;
        // Is it root?
        if (cur.first_cluster_number==0) start_sector=f16_d.first_root_sector;
        else start_sector=f16_d.first_data_sector+(cur.first_cluster_number-2)*bpb.sectors_per_cluster;

        int sectors_to_scan=(cur.first_cluster_number==0) ? f16_d.root_dir_sectors : bpb.sectors_per_cluster;

        for (int s=0;s<sectors_to_scan && !found;s++) {
            ata_read_sectors(start_sector+s,sector,1,F16_DRIVE);
            directoryentry_t* entries=(directoryentry_t*)sector;
            for (int j=0;j<max_per_sector;j++) {
                directoryentry_t entry=entries[j];
                
                if ((unsigned char)entry.filename[0]==0xE5) continue; // Deleted
                if (entry.file_attributes & 0x2) continue;

                // LFN
                if (entry.file_attributes == 0xF) continue;

                if (memcmp(entry.filename,padded,11)) {
                    cur=entry;
                    found=1;
                    break;
                }
            }
        }
    }

    return cur;
}

void read_file(char* buffer, const char* path, uint8_t drive) {
    uint16_t* fat = (uint16_t*) FAT_TABLE;

    directoryentry_t file = find_file((char*) path);

    if (file.file_attributes & 0x10) {return;}

    uint16_t cluster=file.first_cluster_number;

    if (cluster<2) { return; }

    while (cluster<0xFFF8) {
        int f16_sector = f16_d.first_data_sector+(cluster-2)*bpb.sectors_per_cluster;

        ata_read_sectors(f16_sector,buffer,bpb.sectors_per_cluster,drive);

        buffer += bpb.sectors_per_cluster * bpb.bytes_per_sector;
        cluster = get_fat_entry(cluster);
    }
}