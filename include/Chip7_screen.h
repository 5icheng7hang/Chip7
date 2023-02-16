#ifndef CHIP7_SCREEN_H
#define CHIP7_SCREEN_H

#include <stdbool.h>
#include "Config.h"

struct Chip7_scr
{
    bool PixelBit[CHIP7_SCREEN_HEIGHT][CHIP7_SCREEN_WIDTH];
};

void chip7_screen_clear(struct Chip7_scr* screen);
void chip7_screen_set(struct Chip7_scr* screen, int x, int y);
bool chip7_screen_get(struct Chip7_scr* screen, int x, int y);

bool chip7_screen_draw_sprite(struct Chip7_scr* screen, int x, int y, const char* sprite, int num);

#endif