
//                       =======
//                       =======
//                       ======= 
//               =======================
//               =======================
//               =======================
//                       =======
//                       =======
//                       =======
//                       =======
//                       =======
//                       =======
//                       =======
//



#define AREA_WIDTH 30
#define AREA_HEIGHT 10

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define PLAYER 1
#define NOTHING 0
#define APPLE 2

#define FG 0xF
#define BG 0x1

#define SC_W 0x11
#define SC_A 0x1E
#define SC_S 0x1F
#define SC_D 0x20

#define SC_Q 0x10

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_STATUS_PORT 0x64

#define VGA_TEXT 0xB8000

typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned char uint8_t;

int main(void);

void _start(void) {
    main();
    return;
}

void outb(uint16_t port, uint8_t val) { asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); };

void outw(uint16_t port, uint16_t val) {  asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port)); };

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}



typedef struct {
    uint8_t x;
    uint8_t y;
} Pos __attribute__ ((packed));

Pos Player;
Pos Apple;

char Area[AREA_HEIGHT][AREA_WIDTH];

int Running = 1;

void putentry(uint16_t x, uint16_t y, char ch) {
    uint32_t offset = (y*SCREEN_WIDTH)+x;

    volatile uint16_t* VGA = (volatile uint16_t*) VGA_TEXT;
    VGA[offset] = (uint16_t) ch | (uint16_t) (FG | BG << 4) << 8;
    return;
}

void wait_til_key() {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1)) {
        __asm__ volatile("pause");
    }

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    switch (scancode) {
        case SC_W:
            if (Player.y > 0) Player.y--;
            break;
        case SC_A:
            if (Player.x > 0) Player.x--;
            break;
        case SC_S:
            if (Player.y <= AREA_HEIGHT) Player.y++;
            break;
        case SC_D:
            if (Player.x <= AREA_WIDTH) Player.x++;
            break;
        case SC_Q:
            Running = 0;
            break;
        default:
            break;
    }

    return;
}

void clear(void) {
    int x=0;
    int y=0;
    do {
        do {
            putentry(x,y,' ');
            y++;
        } while (y<SCREEN_HEIGHT);
        y=0;
        x++;
    } while (x<SCREEN_WIDTH);
}

void render(void) {
    clear();

    for (int x=0;x<AREA_WIDTH;x++) {
        for (int y=0;y<AREA_HEIGHT;y++) {
            char c = Area[y][x];
            if (c==PLAYER)       putentry(x,y,'@');
            else if (c==NOTHING) putentry(x,y,'#');
            else if (c==APPLE)   putentry(x,y,'$');
            else                 putentry(x,y,'?');
        }
    }
}

int main(void) {
    Player.x = 0;
    Player.y = 0;

    for (int x = 0; x < AREA_WIDTH; x++) {
        for (int y = 0; y < AREA_HEIGHT; y++)
            Area[y][x] = NOTHING;
    }

    Area[Player.y][Player.x] = PLAYER;

    do {
        render();
        wait_til_key();
    } while (Running);

    return 0;
}