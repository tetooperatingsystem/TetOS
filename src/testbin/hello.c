
#define WIDTH 80
#define HEIGHT 25

#define FG 0xF
#define BG 0x0

int main(void) {
    volatile unsigned short* VGA_MEM = (volatile unsigned short*) 0xb8000;

    // terminal column is at 0x1000
    unsigned short posx = *(unsigned short*) 0x1000;
    // terminal row is at 0x1002
    unsigned short posy = *(unsigned short*) 0x1002;

    unsigned long offset = (posy * WIDTH) + posx;

    unsigned char color = FG | BG << 4;

    char* msg = "Hello, World!";

    int i = 0;
    do {
        VGA_MEM[offset + i] = (unsigned short) msg[i] | (unsigned short) color << 8; 
        i++;
    } while (msg[i]);

    return 0;
}