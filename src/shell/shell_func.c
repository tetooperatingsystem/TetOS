
#ifndef SHELL_FUNC_C

#define SHELL_FUNC_C

#include "../terminal/terminal.h"
#include "../mem.h"
#include "../kernel/drive_io.h"
#include "../drivers/fat16/fat16.h"
#include "../drivers/vga/vga.h"
#include "../drivers/vesa/vesa.h"
#include "../kernel/ports.h"
#include "../drivers/cmos/cmos.h"
#include "../drivers/sound/sound.h"
#include "../drivers/fonts/psf.h"
#include "../drivers/wait.h"
#include "../rand.h"

#include "shell_func.h"

char filepath[256];

extern unsigned char* FAT_TABLE;
extern PSF1_Header Info1;
extern PSF_Font Info;

extern int term_fg, term_bg;

int drive = ATA_SLAVE;

#define SH_COLORS_SIZE 16

int stoi(char* str) {
    int len = strlen(str);
    int num = 0;

    for (int i = 0; str[i] != 0; i++) {
        char ch = str[i];
        if (ch < '0'  || ch > '9') continue;
        num = num * 10 + (ch - '0');
    }

    return num;
}

void sh_colors(char parsed[32][32]) {
    int i = 0;

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int px = 300 + x * SH_COLORS_SIZE;
            int py = 10 + y * SH_COLORS_SIZE;

            int a = 0;
            do {
                int dx,dy;
                dx=a%SH_COLORS_SIZE;
                dy=a/SH_COLORS_SIZE;
                SetPixel(px+dx,py+dy,i);
                // Reasoning: a++ crashes.
                (*(&a))++;
            } while (a < SH_COLORS_SIZE * SH_COLORS_SIZE);
            i++;
        }
    }
}
/*
typedef struct {
    int duration;
    int octave;
    int bpm;
} RTTTLDefaults;

typedef struct {
    int duration;
    char note;
    int sharp;
    int octave;
    int dotted;
} RTTTLNote;

int parse_number(const char **p) {
    int value = 0;

    while (**p >= '0' && **p <= '9') {
        value = value * 10 + (**p-'0');
        (*p)++;
    }
    return value;
}
*/

/*
void sh_playsong(char parsed[32][32]) {
    return;
    if (parsed[1][0]==0) {
        printf("USAGE: PlaySong [RTTL_FILE]",terminal_color);
        return;
    }
    char npath[256];
    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    DirectoryEntry file = find_file(npath);
    if (file.FileAttributes & 0x10) {
        printf("NO SUCH FILE", terminal_color);
        return;
    }
    char buff[4096];

    read_file(buff,npath,drive);

    RTTTLDefaults defaults;

    const char *p = buff;

    while (*p && *p != ':') p++;
    if (*p != ':') return;
    
    p++;

    int note_freq[][12] = {
        // C  C#   D     D#   E    F    F#   G   G#    A    A#   B
        {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494},   // O4
        {523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988},   // O5
        {1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976},  // 06
        {2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951}   // O7
    };

    while (*p && *p != ':') {
        char key = *p++;

        if (*p++ != '=') { return; }
        int value = parse_number(&p);

        switch (key) {
            case 'd': defaults.duration = value; break;
            case 'o': defaults.octave = value; break;
            case 'b': defaults.bpm = value; break;
            default: return;
        }

        if (*p==',') {
            p++;
        }
    }
    if (*p != ':') {
        return;
    }
    p++;

    //while (*p) {
        // Actually playing the sound.
    //}

}
*/

void sh_color(char parsed[32][32]) {
    int fg = stoi(parsed[1]);
    int bg = stoi(parsed[2]);

    // for other
    term_fg = fg;
    term_bg = bg;

    terminal_color = vga_entry_color(fg, bg);
}

void sh_rline(char parsed[32][32]) {
    if (parsed[1][0] == 0) {
        printf("USAGE: RLINE [FILE]", terminal_color);
        return;
    }
    char npath[256] = {0};

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    DirectoryEntry file = find_file(npath);
    if (file.FileAttributes & 0x10) {
        printf("NO SUCH FILE", terminal_color);
        return;
    }

    char buff[8192];

    read_file(buff, npath, drive);

    int lines = 0;
    int i = 0;
    do {
        if (buff[i]=='\n')lines++;
        i++;
    } while(buff[i]);

    int line = rand() % lines;

    int x = 0;
    int y = 0;

    do {
        if (buff[x]=='\n') {
            y++;
            if (y==line-1) {
                x++;
                break;
            }
        }
        x++;
    } while (buff[x]);

    // x - index to the line
    do {
        if (buff[x]=='\n') break;
        putchar(buff[x], terminal_color);
        x++;
    } while (buff[x]);
}

void sh_cat(char parsed[32][32]) {
    char buff[8192];
    char npath[256] = {0};

    int width = GetWidth();

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    read_file(buff, npath, drive);
    DirectoryEntry file = find_file(npath);
    if (file.FileAttributes & 0x10) {
        printf("NO SUCH FILE: ", terminal_color);
        printf(npath, terminal_color);
    }
    else {
        int i = 0;
        do {
            write(&buff[i], width, terminal_color);
            i += width;
        } while (buff[i] != 0);
    }
    memset(buff, 0, 8192);
}

void clear_line(int row) {
    for (int col = 0; col < 80; col++) {
        term_putentryat(' ', terminal_color, col, row);
    }
}

void sh_rand(char parsed[32][32]) {
    int max = stoi(parsed[1]);

    int res = rand();

    srand(res);

    if (max) res %= max;

    printf(itos(res), terminal_color);
}

void sh_read(char parsed[32][32]) {
    char buff[16384];
    char npath[256] = {0};

    int width = GetWidth();
    int height = GetHeight();

    if (parsed[1][0] == 0) {
        printf("USAGE: READ [PATH]", terminal_color);
        return;
    }

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    memset(buff, 0, 16384);
    read_file(buff, npath, drive);
    DirectoryEntry file = find_file(npath);
    if (file.FileAttributes & 0x10) {
        printf("NO FILE: ", terminal_color);
        printf(npath, terminal_color);
        goto sh_read_done;
    }

    size_t offset = 0;

    buff[16383] = 0;

    int page = 1;

    do {
        sh_cls(parsed);
    
        if (offset >= sizeof(buff) || buff[offset] == 0) break;

        size_t i = offset;
        size_t rows = 0;

        int last_row = terminal_row;
        int xss = 0;

        while (buff[i] && rows < height - 1) {
            putchar(buff[i], terminal_color);
            if (buff[i]=='\n') {
                rows++;
                xss=0;
            } else {
                xss++;
                if (xss>=width) {
                    xss=0;
                    rows++;
                }
            }

            i++;
        }
        terminal_column++;

        for (int i = terminal_row; i < height-1; i++) { putchar('\n',terminal_color); }

        putchar(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        printf("SPACE/D: NEXT, Q: EXIT  A: LAST   PAGE ", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        printf(itos_h(page), vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        putchar(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        write(npath, strlen(npath), vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));

        int x = width - terminal_column;

        for (int i = 0; i < x-1; i++) {
            putchar(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        }
        wait_for_input:

        uint8_t sc = ps2_kb_wfi();

        if (sc & 0x80) goto wait_for_input;

        char ch = scancode_to_ascii((scancode_t)sc);

        if (ch == 'q' || ch == 'Q') break;
        else if (ch == ' ' || ch == 'd' || ch == 'D') {
            page += 1;
            offset = i;
        }
        else if ((ch == 'a' || ch == 'A') && page > 1) {
            page -= 1;
            offset = 0;

            for (int p = 1; p < page; p++) {
                size_t tmp = offset;
                size_t rows = 0;
                int xss = 0;
                do {
                    if (buff[tmp]=='\n') {
                        rows++;
                        xss=0;
                    } else {
                        xss++;
                        if (xss>=width) {
                            xss=0;
                            rows++;
                        }
                    }
                    tmp++;

                } while (buff[tmp] && rows < height - 1);
                offset = tmp;
            }
        }
        else goto wait_for_input;
        
    } while(1);
    sh_read_done:
        memset(buff, 0, 16384);
}

void sh_exec(char parsed[32][32]) {
    char npath[256] = {0};

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    DirectoryEntry file = find_file(npath);

    if (file.size_bytes > 4096) {
        printf("COULD NOT EXECUTE; BINARY LARGER THAN 4096", terminal_color);
        return;
    }

   // find_file() stops at the last iteration it found, so if it cannot find a file it stops in a directory
    if (file.FileAttributes & 0x10) {
        printf("NO SUCH FILE", terminal_color);
        return;
    }

    if (memcmp(&file.Filename[8], "TTO", 3)) {
        char buff[16384];
        read_file(buff, npath, drive);
        size_t len = strlen(buff);

        char line[1055];
        int line_ind = 0;

        for (int i = 0; i < len; i++) {
            if (buff[i] == '\n') {
                line[line_ind] = 0;
                shell_exec(line);
                memset(line, 0, 1055);
                line_ind = 0;
            }
            else line[line_ind++] = buff[i];
        }
        if (*line) shell_exec(line);

        return;
    }

    read_file(NULL, npath, drive);

    void (*entry)()  = (void(*)()) NULL;
    
    entry();
}

void sh_dir(char parsed[32][32]) {
    char npath[256] = {0};
    strappend(npath, filepath);
    if (parsed[1])  {
        size_t len = strlen(npath);
        if (len > 0 && npath[len - 1] != '/')
            strappend(npath, "/");
        strappend(npath, parsed[1]);
    }

    list_entries_in_dir(npath);
}

void sh_setfont(char parsed[32][32]) {

    if (!IsVesaOn()) return;
    char npath[256] = {0};

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    DirectoryEntry file = find_file(npath);

    if (file.FileAttributes & 0x10) {
        printf("No such file.", terminal_color);
        return;
    }

    PSF_Init(npath);
}

void sh_setpixel(char parsed[32][32]) {
    if (parsed[3][0]==0) {
        printf("USAGE: SETPIXEL [COLOR] [X] [Y]",terminal_color);
        return;
    }
    uint8_t col = stoi(parsed[1]);
    int x = stoi(parsed[2]);
    int y = stoi(parsed[3]);

    SetPixel(x,y,col);
}

void sh_cd(char parsed[32][32]) {
    char npath[256] = {0};

    strappend(npath, filepath);
    strappend(npath, parsed[1]);

    char dirs[32][32] = {0};
    char new_path[32][32] = {0};

    int count = separatebyslashes(npath, dirs);

    int x = 0;

    for (int i = 1; i < count; i++) {
        char* str = dirs[i];

        if (strcmp(str, "..")) {
            if (x>0) {
                x--;
                memset(new_path[x], 0, 32);
            }
        }
        else if (strcmp(str, ".")) {continue; }
        else {
            memcpy(new_path[x], dirs[i], 32);
            x++;
        }
    }

    char strpath[1024] = {0};

    if (parsed[1][0] != '/') strappend(strpath, "/");

    for (int j = 0; j < x; j++) {
        strappend(strpath, new_path[j]);
        strappend(strpath, "/");
    }

    DirectoryEntry dir = find_file(strpath);

    if (!(dir.FileAttributes & 0x10) && !(dir.first_cluster_number == 0)) {
        printf("NO SUCH DIRECTORY.", terminal_color);
        return;
    }

    if (strcmp(strpath, "/")) {
        char path[256] = "/";
        memcpy(filepath, path, 256);
        return; // Epstein ate babies
    
    }

    else if (x-1<-1) {
        printf("COULD NOT OPEN DIRECTORY.", terminal_color);
        return;

    }
    char name[11] = {0};

    format_name(new_path[x-1], name);

    if (!memcmp(dir.Filename, name, 11)) {
        printf("NO SUCH DIRECTORY.", terminal_color);
        return;
    }

    memcpy(filepath, strpath, 256);
}

void sh_pwd(char parsed[32][32]) {
    printf(filepath, terminal_color);
}

void sh_shutdown(char parsed[32][32]) {
    outw(0x604, 0x2000);
}

void sh_echo(char parsed[32][32]) {
    char res[512];
    memset(res, 0, 512);

    int pos = 0;

    for (int i = 1; parsed[i][0] != 0; i++) {
        for (int j = 0; parsed[i][j]; j++)
            res[pos++] = parsed[i][j];
        res[pos++] = ' ';
    }

    printf(res, terminal_color);
    putchar('\n', terminal_color);
}

void sh_beep(char parsed[32][32]) { 
    if (parsed[1][0]==0) {
        printf("USAGE: Beep [FREQUENCY] [TIME]", terminal_color);
        return;
    }
    int frequency = stoi(parsed[1]);
    PlaySound(frequency);
    wait(stoi(parsed[2]));
    MuteSound();
}


void sh_help(char parsed[32][32]) {
    printf(
        "read README.txt",
        terminal_color
    );
}

void sh_cls(char parsed[32][32]) {
    int width = GetWidth();
    int height = GetHeight();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            term_putentryat(' ', terminal_color, x, y);
            terminal_column = 0;
            terminal_row = 0;
        }
    }
}

void sh_col(char parsed[32][32]) {
    uint8_t fg = stoi(parsed[1]);
    uint8_t bg = stoi(parsed[2]);

    printf("############\n", vga_entry_color(fg, bg));   
}

void sh_chgdrv(char parsed[32][32]) {
    if (parsed[2][0]==0) {
        printf("USE: CHGDRV [F/D] [NUMBER]\n",terminal_color);
        printf("F:FLOPPY; D:DISK",terminal_color);
        return;
    }
    char drs = parsed[1][0];

    if (drs=='F'||drs=='f') {
        printf("NO FLOPPY SUPPORT",terminal_color);
    }
    else if (drs=='D'||drs=='d') {
        int dr = stoi(parsed[2]);
        if (dr>3) {
            printf("ATA ONLY 0-3",terminal_color);
            return;
        }
        debug("DR:",dr,terminal_color);
        drive=dr;
        memset(filepath,0,256);
        filepath[0]='/';
        fat16_init(dr);
    }
}

void sh_text(char parsed[32][32]) {
    if (parsed[4][0]==0) {
        printf("USAGE: TEXT [COL] [X] [Y] [TEXT]", terminal_color);
        return;
    }
    uint8_t col = stoi(parsed[1]);
    int x,y;
    x=stoi(parsed[2]);
    y=stoi(parsed[3]);

    char buff[512];

    for (int i = 4; parsed[i][0]; i++) {
        strappend(buff, parsed[i]);
        strappend(buff, " ");
    }

    int old_x = x;

    int width,height;
    width=GetWidth();
    height=GetHeight();

    int psfv=GetPSFVers();

    for (int i = 0; i < strlen(buff); i++) {
        if (x >= 640) {
            x=old_x;
            if (psfv==1) {
                y+=Info1.characterSize;
            }
            else if (psfv==2) {
                y+=Info.height;
            }
        }
        DrawGlyph(x,y,buff[i],col);
        if (psfv==1) x+=8;
        else x+=Info.width;
    }

    memset(buff,0,512);
}

void sh_wait(char parsed[32][32]) {
    if (parsed[1][0]==0) {
        printf("USAGE:Wait[S]",terminal_color);
        return;
    }
    wait(stoi(parsed[1]));
}

void sh_square(char parsed[32][32]) {
    if (parsed[5][0]==0) {
        printf("USAGE: SQUARE [COL] [X] [Y] [WIDTH] [HEIGHT]", terminal_color);
        return;
    }
    uint8_t col = stoi(parsed[1]);
    int x,y,width,height;
    x=stoi(parsed[2]);
    y=stoi(parsed[3]);
    width=stoi(parsed[4]);
    height=stoi(parsed[5]);

    for (int xx = 0; xx < width; xx++) {
        for (int yy = 0; yy < height; yy++) {
            SetPixel(xx+x,yy+y,col);
        }
    }
}

char* itos_0h(int number) {
	if (number == 0) {
		return "0";
	}
	static char ret[10];
	char out[8];
	int i = 0;

	while (number > 0  && i < 8) {
		int b = number % 16;
		number /= 16;
		if (b <= 9) {
			out[i] = '0' + b;
		}
		else if (b >= 10 && b <= 15) {
			out[i] = 'A' + (b - 10);
		}
		i++;
	}

	// Reverse the outcome

	for (int j = 0; j < i; j++) {
		ret[j] = out[i - j - 1];
	}
	ret[i] = 0;

	return ret;
}


void sh_datetime(char parsed[32][32]) {
    CMOS_T cmos = read_rtc();

    char* weekday_s;

    switch (cmos.weekday) {
        case 1:
            weekday_s = "SUN";
            break;
        case 2:
            weekday_s = "MON";
            break;
        case 3:
            weekday_s = "TUE";
            break;
        case 4:
            weekday_s = "WED";
            break;
        case 5:
            weekday_s = "THU";
            break;
        case 6:
            weekday_s = "FRI";
            break;
        case 7:
            weekday_s = "SAT";
            break;
        default:
            weekday_s = "???";
            break;
    }

    char bbbb[512];

    strappend(bbbb, weekday_s);
    strappend(bbbb, "   ");
    if (cmos.day_of_month < 10) { strappend(bbbb, "0"); }
    strappend(bbbb, itos(cmos.day_of_month));
    strappend(bbbb, "/");
    if (cmos.month < 10) { strappend(bbbb, "0"); }
    strappend(bbbb, itos(cmos.month));
    strappend(bbbb, "/");
    if (cmos.year < 10) { strappend(bbbb, "0"); }
    strappend(bbbb, itos(cmos.year));
    strappend(bbbb, "   ");
    if (cmos.hours >= 24) { cmos.hours -= 24; }
    if (cmos.hours < 10) { strappend(bbbb, "0"); }
    strappend(bbbb, itos(cmos.hours));
    strappend(bbbb, ":");
    if (cmos.minutes < 10) { strappend(bbbb, "0"); }
    strappend(bbbb, itos(cmos.minutes));

    printf(bbbb, terminal_color);

    memset(bbbb, 0, 512);
}

void sh_timezone(char parsed[32][32]) {
    if (parsed[1][0] == 0 || parsed[2][0] == 0) {
        printf("USAGE: TIMEZONE [+/-] [OFFSET]", terminal_color);
        return;
    }

    if (strcmp(parsed[1], "+"))
        SetTimezone(stoi(parsed[2]));
    else if (strcmp(parsed[1], "-"))
        SetTimezone(-(stoi(parsed[2])));
}

int abs(int a) {
    if (a<0) return -a;
    return a;
}

void sh_circle(char parsed[32][32]) {
    if (parsed[4][0]==0) {
        printf("USAGE: CIRCLE [COL] [X] [Y] [R]", terminal_color);
        return;
    }

    uint8_t col = stoi(parsed[1]);
    int xc,yc,x,y,r,p;
    xc=stoi(parsed[2]);
    yc=stoi(parsed[3]);
    r=stoi(parsed[4]);

    // Midpoint Circle Algorithm

    x=r;
    y=0;
    p=1-r;

    int b[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};

    while (x>=y) {
        int i = 0;
        int z[2];
        do {
            z[0]=b[i][0];
            z[1]=b[i][1];

            SetPixel(xc+(x*z[0]),yc+(y*z[1]),col);
            SetPixel(xc+(y*z[1]),yc+(x*z[0]),col); 
            i++;
        } while(i<4);
        y++;

        if (p<0) p+=2*y+1;
        else {
            x--;
            p+=2*(y-x)+1;
        }
    }
}

void sh_line(char parsed[32][32]) {
    // Bresenham's line algorithm

    if (parsed[5][0]==0) {
        printf("USAGE: LINE [COL] [X1] [Y1] [X2] [Y2]", terminal_color);
        return;
    }

    uint8_t col=stoi(parsed[1]);
    int x1, x2, y1, y2;
    x1=stoi(parsed[2]);
    y1=stoi(parsed[3]);
    x2=stoi(parsed[4]);
    y2=stoi(parsed[5]);

    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -(abs(y2-y1));
    int sy = y1 < y2 ? 1 : -1;
    int error = dx + dy;

    while (1) {
        SetPixel(x1,y1,col);
        int e2 = 2 * error;
        if (e2 >= dy) {
            if (x1==x2) break;
            error = error + dy;
            x1 = x1 + sx;
        }
        if (e2 <= dx) {
            if (y1==y2) break;
            error = error + dx;
            y1 = y1 + sy;
        }
    }

}

#endif
