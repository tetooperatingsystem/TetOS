#ifndef PSF_H
#define PSF_H

#include <stddef.h>

#define PSF1_FONT_MAGIC 0x0436
#define PSF_FONT_MAGIC  0x864AB572

// PSF1 Glyphs are always 8-bit wide, and characterSize tall

// PSF1
typedef struct {
    uint16_t magic;
    uint8_t fontMode;      // PSF Font Mode
    uint8_t characterSize; // PSF Character Size
} __attribute__ ((packed)) PSF1_Header;

// PSF2
typedef struct {
    uint32_t magic;        // Identification
    uint32_t version;      // Zero
    uint32_t headersize;   // Offset of bitmaps in file, 32
    uint32_t flags;         // 0 if no Unicode
    uint32_t numglyph;     // number of glyphs
    uint32_t bytesperglyph;// Size of each glyph
    uint32_t height;       // Height in pixels
    uint32_t width;        // Width in pixels
} __attribute__ ((packed)) PSF_Font;

void PSF_Init(const char* filename);

int GetPSFVers();

void DrawGlyph(int x, int y, uint8_t c, uint8_t col);


#endif