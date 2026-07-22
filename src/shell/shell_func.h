
#ifndef SHELL_FUNC_H

#define SHELL_FUNC_H

#include "shell.h"

void sh_color(char parsed[32][32]);

void sh_cat(char parsed[32][32]);

void sh_wait(char parsed[32][32]);

void sh_read(char parsed[32][32]);

void sh_colors(char parsed[32][32]);

void sh_exec(char parsed[32][32]);

void sh_datetime(char parsed[32][32]);

void sh_timezone(char parsed[32][32]);

void sh_rand(char parsed[32][32]);

void sh_beep(char parsed[32][32]);

void sh_dir(char parsed[32][32]);

void sh_cd(char parsed[32][32]);

void sh_pwd(char parsed[32][32]);

void sh_rline(char parsed[32][32]);

void sh_shutdown(char parsed[32][32]);

void sh_echo(char parsed[32][32]);

void sh_help(char parsed[32][32]);

void sh_cls(char parsed[32][32]);

void sh_playsong(char parsed[32][32]);

void sh_col(char parsed[32][32]);

void sh_setfont(char parsed[32][32]);

// GRAPHICS

void sh_setpixel(char parsed[32][32]);
void sh_line(char parsed[32][32]);
void sh_square(char parsed[32][32]);
void sh_text(char parsed[32][32]);
void sh_circle(char parsed[32][32]);

#endif