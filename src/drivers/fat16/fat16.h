#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
Ripped comments from one of my old projects: 

	   Fat16 Data structure

	__________________________
	| 	Boot Sector	         |
	|________________________|
	|	BPB & EBPB	         |
	|________________________|
	|	 FAT #0		         |
	|________________________|
	|	 FAT #1		         |
	|________________________|
	|     Root Directory	 |
	|________________________|
	|        Data Area	     |
	|________________________|

 Note : FAT #0 is usually the original table,
 whilst FAT #1 is a backup

 A FAT is a Table that contains a value for each cluster to see if its allocated or not.
	Values:
		0x0000	-	Not Allocated
		0xFFF7	-	Bad Cluster
	0xFFF8 and greater -	End of FAT
		Rest	-	Allocated sector


BPB:
	Offset	Size	Meaning
	0x00	3	JMP SHORT 3C (needed for Windows and OSX)
	0x03	8	OEM identifier
	0x0B	2	the number of bytes per sector (little-endian)
	0x0D	1	sectors per cluster
	0x0E	2	number of reserved sectors (boot sectors included)
	0x10	1	number of file allocation tables (oftenly, 2)	
	0x11	2	number of root directory entries (must be set so that the root directory occupies entire sectors)
	0x13	2	the total sectors in the logical volume (if the value is 0, it means there are more than 2^16 sectors in the volume, and the actual count is in the LSC entry at 0x20)
	0x15	1	media descriptor type
	0x16	2	number of sectors per FAT
	0x18	2	number of sectors per track
	0x1A	2	number of heads or sides on the storage media
	0x1C	4	number of hidden sectors
	0x20	4	large sector count.
EBPB:
	0x24	1	drive number
	0x25	1	flags in windows NT. reserved otherwise
	0x26	1	signature (0x28 OR 0x29)
	0x27	4	volumeid 'serial' number. (non necessary)
	0x2B	11	volume label string. padded with spaces.
	0x36	8	system identifier string. string representation of the FAT file system type. it is padded with spaces. the spec says to never trust teh contents of this string for any use
	0x3E	448	boot code.
	0x1FE	2	bootable partition signature  0xAA55
Directory:
	0x00	11	Short File Name
	0x0B	1	File attributes (See table below)
	0x0C	1	Reserved by Windows NT
	0x0D	1	Creation time(hundreds of a second)
	0x0E	2	Creation time(hour, minute, second/2)
	0x10	2	Creation date(year, month, date)
	0x12	2	Last accessdate(year, month, day)
	0x14	2	High word of the first  cluster index (on FAT12 and FAT16 - 0)
	0x16	2	Modification time(hour, minute, second)
	0x18	2	Modification date(year, month, day)
	0x1A	2	First cluster number for this entry
	0x16	4	Size in bytes
Filename Entry:
	0x0	1	Entry type = 0xC1
	0x1	1	Flags
	0x2	30	File name characters (15 UTF16 code units).


 A sector is usually 512 bytes, whilst a directory entry is 32 bytes.
 So unless the sector is 4096 bytes, a sector should contain 16 directories.


*/

#define CLUSTER_CHAIN_END 0xfff8
#define CLUSTER_BAD 0xfff7

typedef struct {
	uint8_t byt[3];
} uint24_t;

typedef struct {
	uint24_t JMP;
	char OEM[8];
	uint16_t BytesPerSector;
	uint8_t SectorsPerCluster;
	uint16_t ReservedSectors; // (Boot record values included)
	uint8_t FATAmount;
	uint16_t RootDirectoriesAmount;
	uint16_t SectorsInLogicalVolume16;	// if 0, use 32
	uint8_t MediaDescriptorType;
	uint16_t SectorsPerFAT;
	uint16_t SectorsPerTrack;
	uint16_t HeadAmount;
	uint32_t HiddenSectorAmount;
	uint32_t SectorsInLogicalVolume32;
} __attribute__((packed)) BPB;

typedef struct {
	uint8_t DriveNumber; // (BIOS drive number)
	uint8_t FlagsWindows;
	uint8_t Signature; // (0x28 or 0x29)
	uint32_t VolumeIDSerialNumber; 
	char VolumeLabelString[11];	// Padded with spaces
	char SystemIdentifierString[8];
	uint8_t BootCode[448];
	uint16_t BootablePartitionSignature; // 0xAA55
} EBPB;

struct F16_DATA {
	uint8_t FirstRootSector;		// = (bpb.reserved_sectors;bpb.sectors_per_cluster) + bpb.fat_count * bpb.sectors_per_fat
	unsigned int RootDirSectors; // = (bpb.rootdir_count * 32) / 512
	unsigned int FirstDataSector; // = first_root_sector + root_dir_sectors
	unsigned int ClusterCount;  // = (bpb.sectors_per_volume - first_data_sector) / bpb.sectors_per_cluster
};

typedef struct {
	char Filename[11];
	uint8_t FileAttributes;
	uint8_t WindowsNT_Reserve;

	uint8_t creation_time_hundreds; // HUNDREDS OF A SECOND
	uint16_t creation_time_hms2;	// Hour, minute, second /2
	uint16_t creation_date;			// YEAR, MONTH, DAY
	uint16_t last_access_date;		// LAST ACCESSED DATE (YMD)

	uint16_t high_word_cluster_index;// ON FAT16 - 0

	uint16_t modification_time;		 // HOUR, MINUTE, SECOND
	uint16_t modification_date;		 // YEAR, MONTH, DAY

	int16_t first_cluster_number;	 // FIRST CLUSTER NUMBER FOR THE ENTRY
	uint32_t size_bytes;			// SIZE IN BYTES

} __attribute__((packed)) DirectoryEntry;

// Long filename entries are always placed right before short filenames (8.3)
// 32 bytes long

typedef struct {
	uint8_t entry_order;			// The order of the entry in the sequence of long filename entries. Helps you to know where in the file's name the characters from this entry should be placed
	uint16_t first_characters[5];    // First 5 characters, 2 byte per character
	uint8_t attribute;				// Always 0xF
	uint8_t long_entry_type;		// Long entry type. Zero for name entries.
	uint8_t checksum;				// Checksum generated of the short file name when the file was created
	uint16_t mid_characters[6];		// The next 6 characters, 2 bytes per character
	uint16_t zero;					// Nobody knows what it does. Always 0
	uint16_t last_characters[4];	// Last 2 characters, 2 bytes per character
} __attribute__((packed)) LongFileName;

void read_file(char* buffer, const char* path, uint8_t drive);
void write_file(char* buffer, uint32_t file_size, char* filename);

// uint8_t check_fat(unsigned int cluster);

void fat16_init(uint8_t drive);

void list_entries_in_dir(const char* path);

void format_name(const char* input, char out[11]);

int separatebyslashes(char* str, char dir[32][32]);

DirectoryEntry find_file(char* filepath);