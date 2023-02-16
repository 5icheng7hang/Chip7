#ifndef CHIP7_STACK_H
#define CHIP7_STACK_H

#include "Config.h"

struct Chip7;

struct Chip7_stk
{
    unsigned short StackArray[CHIP7_STACK_TOTAL];
};

void chip7_stack_push(struct Chip7* chip7, unsigned short val);
unsigned short chip7_stack_pop(struct Chip7* chip7);

#endif