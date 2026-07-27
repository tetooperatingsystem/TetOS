#ifndef PSF_C
#define PSF_C

#include "../fat16/fat16.h"
#include "../../kernel/drive_io.h"
#include "../vesa/vesa.h"
#include "../../mem.h"
#include "psf.h"

#include <stddef.h>

uint8_t FileBuff[12345];

// 1 - PSF1, 2 - PSF2 

int PSF_VERS = 0;
PSF_Font Info;
PSF1_Header Info1;

void PSF_Init(const char* filename) {
    DirectoryEntry file = find_file((char*)filename);
    if (file.FileAttributes & 0x10) {
        // /Sys/Fonts/Font1.PSF - Basic font
        read_file((char*) FileBuff, "Sys/Fonts/Font1.PSF", ATA_SLAVE);
    }
    else read_file((char*) FileBuff, filename, ATA_SLAVE);

    PSF_Font inf = *(PSF_Font*) FileBuff;
    PSF1_Header inf1 = *(PSF1_Header*) FileBuff;

    if (inf1.magic == PSF1_FONT_MAGIC) PSF_VERS = 1;
    else if (inf.magic == PSF_FONT_MAGIC) PSF_VERS = 2;
    else return;

    if (PSF_VERS == 2) memcpy(&Info, &inf, sizeof(PSF_Font));
    else memcpy(&Info1, &FileBuff[0], sizeof(PSF1_Header));
}

int GetPSFVers() { return PSF_VERS; }

void DrawGlyph(int x, int y, uint8_t c, uint8_t col) { 
    
    SetPixel(320,240,0x4);

    int offset;
    if (PSF_VERS == 1) offset = sizeof(PSF1_Header) + Info1.characterSize * (unsigned char) c;
    else offset = Info.headersize + (Info.bytesperglyph * (unsigned char)c);

    unsigned char* glyph = (unsigned char*) (FileBuff + offset);
    int width;
    if (PSF_VERS==1) width=8;
    else width=Info.width;
    int height;
    if (PSF_VERS==1) height=Info1.characterSize;
    else height=Info.height;

    uint32_t bytesperglyphline = (width + 7) / 8;
    
    for (int y1 = 0; y1 < height; y1++) {
        for (int x1 = 0; x1 < width; x1++) {
            uint8_t byte = glyph[y1 * bytesperglyphline + x1/8];
            if (byte & (0x80 >> (x1 & 7))) {
                SetPixel(x+x1, y+y1, col);
            }
        }
    }
    
}

#endif