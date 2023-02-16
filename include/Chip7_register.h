#ifndef CHIP7_REGISTER_H
#define CHIP7_REGISTER_H

#include "Config.h"

struct Chip7_reg
{
    unsigned char V[CHIP7_REGISTER_TOTAL];

    unsigned short I;

    // Delay Timer
    unsigned char DT;
    
    // Sound Timer
    unsigned char ST;

    // Programmer Counter
    unsigned short PC;

    // Stack Pointer
    unsigned char SP;
};


#endif