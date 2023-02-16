#ifndef CHIP7_MEMORY_H
#define CHIP7_MEMORY_H

#include "Config.h"

struct Chip7_mem
{
    unsigned char MemoryByte[CHIP7_MEMORY_SIZE];
};

void chip7_mem_set(struct Chip7_mem* memory, int index, unsigned char val);
unsigned char chip7_mem_get(struct Chip7_mem* memory, int index);
unsigned short chip7_mem_get_short(struct Chip7_mem* memory, int index); 

#endif