#include "Chip7_memory.h"
#include <assert.h>

static void chip7_mem_bounds_check(int index)
{
    assert(index >= 0 && index < CHIP7_MEMORY_SIZE);
}


void chip7_mem_set(struct Chip7_mem* memory, int index, unsigned char val)
{
    chip7_mem_bounds_check(index);
    memory->MemoryByte[index] = val;
}


unsigned char chip7_mem_get(struct Chip7_mem* memory, int index)
{
    chip7_mem_bounds_check(index);
    return memory->MemoryByte[index];
}


unsigned short chip7_mem_get_short(struct Chip7_mem* memory, int index)
{
    unsigned char byte1 = chip7_mem_get(memory, index);
    unsigned char byte2 = chip7_mem_get(memory, index + 1);
    return byte1 << 8 | byte2;
}