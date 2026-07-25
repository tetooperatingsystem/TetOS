

// Types

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

typedef struct {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;             // Bytes Per Line
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;                // Bits Per Pixel
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mark;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;       // Physical address of the framebuffer
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed)) VBEModeInfoStruct;

VBEModeInfoStruct VesaInf;

void* memcpy(void* dest, const void* src, int count) {
    uint8_t* ptr = (uint8_t*) dest;
    uint8_t* ptr2 = (uint8_t*) src;

    for (int x = 0; x < count; x++) {
        ptr[x] = ptr2[x];
    }

    return dest;
}

void vesa_init() {
    // Pointer
    // From the bootloader, we get the Mode Info at 0x8000:0
    volatile uint8_t* p = (volatile uint8_t*) 0x80000;

    memcpy(&VesaInf, (void*) p, sizeof(VBEModeInfoStruct));
}

void SetPixel(int x,int y,uint8_t color) {
    if (x<0||y<0||x>=VesaInf.width||y>=VesaInf.height) return;
    volatile uint8_t* fr = (volatile uint8_t*) VesaInf.framebuffer;
    int offset = (VesaInf.pitch*y)+x;

    fr[offset]=color;
}

int main(void) {
    vesa_init();

    int width = 15;

    int image[] = {
        01, 01, 01, 01, 01, 01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01, 01, 01, 01, 01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01, 01, 01, 01, 01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 01,
        01, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 01,
        01, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 01,
        01, 01, 01, 01, 01, 01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01 ,01 ,01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01, 01, 01, 01, 01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        01, 01 ,01, 01, 01 ,01, 14, 14, 14, 01, 01, 01, 01, 01, 01,
        500
    };

    int x = 400;
    int startx=x;
    int y = 10;

    for (int i = 0; image[i]!=500; i++) {
        SetPixel(x,y,image[i]);

        x++;
        if ((i+1) % width == 0) { y++; x=startx; }
    }


    return 0;
}