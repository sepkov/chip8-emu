#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

#include "config.h"
#include <stdbool.h>
#include<memory.h>

struct chip8_screen
{
    bool pixel[CHIP8_HEIGHT][CHIP8_WIDTH];
};

void chip8_screen_clear(struct chip8_screen *screen);
void chip8_screen_set(struct chip8_screen *screen, int x, int y);
bool chip8_screen_is_set(struct chip8_screen *screen, int x, int y);
bool chip8_screen_draw_sprite(struct chip8_screen *screen, int x, int y, unsigned char *sprite, int num);

#endif