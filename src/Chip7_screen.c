#include "Chip7_screen.h"
#include "Config.h"
#include "memory.h"

#include <assert.h>

static void chip7_screen_bounds_check(int x, int y)
{
    assert(x >= 0 && x < CHIP7_SCREEN_WIDTH && y >= 0 && y <= CHIP7_SCREEN_HEIGHT);
}

void chip7_screen_clear(struct Chip7_scr* screen)
{
    memset(screen->PixelBit, 0, sizeof(screen->PixelBit));
}

void chip7_screen_set(struct Chip7_scr *screen, int x, int y)
{
    chip7_screen_bounds_check(x, y);
    screen->PixelBit[y][x] = true;
}

bool chip7_screen_get(struct Chip7_scr *screen, int x, int y)
{
    chip7_screen_bounds_check(x, y);
    return screen->PixelBit[y][x];
}

bool chip7_screen_draw_sprite(struct Chip7_scr *screen, int x, int y, const char *sprite, int num)
{
    bool IsPixelCollide = false;

    for (int ly = 0; ly < num; ly++)
    {
        char c = sprite[ly];
        for (int lx = 0; lx < 8; lx++)
        {
            if ((c & (0b10000000 >> lx)) == 0)
            {
                continue;
            }
            if (screen->PixelBit[(ly + y) % CHIP7_SCREEN_HEIGHT][(lx + x) % CHIP7_SCREEN_WIDTH])
            {
                IsPixelCollide = true;
            }

            screen->PixelBit[(ly + y) % CHIP7_SCREEN_HEIGHT][(lx + x) % CHIP7_SCREEN_WIDTH] ^= true;
        }
    }

    return IsPixelCollide;
}