#ifndef CHIP7_H
#define CHIP7_H

#include "Config.h"
#include "Chip7_memory.h"
#include "Chip7_register.h"
#include "Chip7_stack.h"
#include "Chip7_keyboard.h"
#include "Chip7_screen.h"

#include <stddef.h>

struct Chip7
{
    struct Chip7_mem Memory;
    struct Chip7_reg Register;
    struct Chip7_stk Stack;
    struct Chip7_key Keyboard;
    struct Chip7_scr Screen;
    
};

void chip7_init(struct Chip7* chip7);
void chip7_load(struct Chip7* chip7, const char* buffer, size_t size);
void chip7_exec(struct Chip7* chip7, unsigned short opcode);
    
#endif