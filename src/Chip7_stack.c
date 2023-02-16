#include "Chip7_stack.h"
#include "Chip7.h"
#include <assert.h>

static void chip7_stack_bounds_check(struct Chip7 *chip7)
{
    assert(chip7->Register.SP < CHIP7_STACK_TOTAL);
}

void chip7_stack_push(struct Chip7 *chip7, unsigned short val)
{
    chip7->Register.SP += 1;
    chip7_stack_bounds_check(chip7);
    chip7->Stack.StackArray[chip7->Register.SP] = val;
}

unsigned short chip7_stack_pop(struct Chip7 *chip7)
{
    chip7_stack_bounds_check(chip7);
    unsigned short val = chip7->Stack.StackArray[chip7->Register.SP];
    chip7->Register.SP -= 1;
    return val;
}