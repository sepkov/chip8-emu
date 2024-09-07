#include "chip8_screen.h"
#include <assert.h>
#include <stdio.h>

static void chip8_screen_check_bounds(int x, int y){
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);    
}

void chip8_screen_set(struct chip8_screen *screen, int x, int y) {
    chip8_screen_check_bounds(x,y);
    screen->pixel[y][x] = true;
}
bool chip8_screen_is_set(struct chip8_screen *screen, int x, int y) {
    chip8_screen_check_bounds(x,y);
    return screen->pixel[y][x];
}

bool chip8_screen_draw_sprite(struct chip8_screen *screen, int x, int y, unsigned char *sprite, int num) {
    bool pixel_collision = false;
    unsigned char c;
    for (int row = 0; row < num; row++) 
    {
        c = sprite[row];
        for (int col = 0; col < 8; col++)
        {
            if((c & (0b10000000 >> col)) == 0) {
                continue;
            }

            if(screen->pixel[(row+y) % CHIP8_HEIGHT][(col+x) % CHIP8_WIDTH]) {
                pixel_collision = true;
            }
            screen->pixel[(row+y) % CHIP8_HEIGHT][(col+x) % CHIP8_WIDTH] ^= true;
        }
        
    }
    

    return pixel_collision;
}

void chip8_screen_clear(struct chip8_screen *screen) {
    memset(screen->pixel, 0, sizeof(screen->pixel));
}