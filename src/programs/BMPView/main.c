

// Just a file that loads file to 0x90000

#include "include/fat16.h"

void strappend(char* a, char* b) {
    int len1,len2;
    len1=strlen(a);
    len2=strlen(b);
    for (int i = 0; i < len2; i++) {
        a[len1+i]=b[i];
    }
}

void read_(char parsed[32][32]) {
    char filepath[256];
    for (int i = 23; i < 32; i++) {
        strappend(filepath, parsed[i]);
    }

    strappend(filepath, parsed[1]);

    fat16_init(ATA_SLAVE);

    unsigned char* buff = (volatile unsigned char*) 0x90000;

    read_file(buff, filepath, ATA_SLAVE);

    return;
}