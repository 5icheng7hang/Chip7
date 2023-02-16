#include "Chip7_keyboard.h"
#include <assert.h>

static void chip7_key_bounds_check(int key)
{
    assert(key >= 0 && key < CHIP7_KEY_TOTAL);
}


void chip7_keyboard_set_map(struct Chip7_key* keyboard, const char* map)
{
    keyboard->Keyboard_Map = map;
}

int chip_key_map(struct Chip7_key* keyboard, char key)
{
    for (int i = 0; i < CHIP7_KEY_TOTAL; i++)
    {
        if (keyboard->Keyboard_Map[i] == key)
        {
            return i;
        }
    }
    return -1;
}


void chip7_key_down(struct Chip7_key* keyboard, int key)
{
    keyboard->KeyboardArray[key] = true;
}


void chip7_key_up(struct Chip7_key* keyboard, int key)
{
    keyboard->KeyboardArray[key] = false;
}


bool chip7_is_key_down(struct Chip7_key* keyboard, int key)
{
    return keyboard->KeyboardArray[key];
}