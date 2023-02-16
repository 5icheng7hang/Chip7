#ifndef CHIP7_KEYBOARD_H
#define CHIP7_KEYBOARD_H

#include "Config.h"

#include <stdbool.h>

struct Chip7_key
{
    bool KeyboardArray[CHIP7_KEY_TOTAL];
    const char* Keyboard_Map;
};

void chip7_keyboard_set_map(struct Chip7_key* keyboard, const char* map);
int chip_key_map(struct Chip7_key* keyboard, char key);
void chip7_key_down(struct Chip7_key* keyboard, int key);
void chip7_key_up(struct Chip7_key* keyboard, int key);
bool chip7_is_key_down(struct Chip7_key* keyboard, int key);

#endif