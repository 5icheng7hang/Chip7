#include "Chip7.h"
#include "Config.h"
#include "Chip7_stack.h"
#include "SDL2/SDL.h"

#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

const char chip7_default_character_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80};

void chip7_init(struct Chip7 *chip7)
{
    memset(chip7, 0, sizeof(struct Chip7));
    memcpy(chip7->Memory.MemoryByte, chip7_default_character_set, sizeof(chip7_default_character_set));
}

void chip7_load(struct Chip7 *chip7, const char *buffer, size_t size)
{
    assert(size + CHIP7_PROG_LOAD_ADDR < CHIP7_MEMORY_SIZE);
    memcpy(&chip7->Memory.MemoryByte[CHIP7_PROG_LOAD_ADDR], buffer, size);
    chip7->Register.PC = CHIP7_PROG_LOAD_ADDR;
}

static void chip7_exec_extended_8(struct Chip7 *chip7, unsigned short opcode)
{
    unsigned char x = (opcode >> 8) & 0x000F;
    unsigned char y = (opcode >> 4) & 0x000F;
    unsigned char final_four_bits = opcode & 0x000F;
    unsigned short tmp = 0;

    switch (final_four_bits)
    {

    // 8xy0 - LD Vx, Vy : Set Vx = Vy.
    case 0x00:
        chip7->Register.V[x] = chip7->Register.V[y];
        break;

    // 8xy1 - OR Vx, Vy : Set Vx = Vx OR Vy.
    case 0x01:
        chip7->Register.V[x] = chip7->Register.V[x] | chip7->Register.V[y];
        break;

    // 8xy2 - AND Vx, Vy : Set Vx = Vx AND Vy.
    case 0x02:
        chip7->Register.V[x] = chip7->Register.V[x] & chip7->Register.V[y];
        break;

    // 8xy3 - XOR Vx, Vy : Set Vx = Vx XOR Vy.
    case 0x03:
        chip7->Register.V[x] = chip7->Register.V[x] ^ chip7->Register.V[y];
        break;

    // 8xy4 - ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry.
    case 0x04:
        tmp = chip7->Register.V[x] + chip7->Register.V[y];
        chip7->Register.V[0x0F] = false;
        if (tmp > 0xFF)
        {
            chip7->Register.V[0x0F] = true;
        }
        chip7->Register.V[x] = tmp;
        break;

    // 8xy5 - SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
    case 0x05:
        if (chip7->Register.V[x] > chip7->Register.V[y])
        {
            chip7->Register.V[0x0F] = true;
        }
        chip7->Register.V[x] = chip7->Register.V[x] - chip7->Register.V[y];
        break;

    // 8xy6 - SHR Vx {, Vy} : Set Vx = Vx SHR 1.
    case 0x06:
        chip7->Register.V[0x0F] = chip7->Register.V[x] & 0x01;
        chip7->Register.V[x] /= 2;
        break;

    // 8xy7 - SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
    case 0x07:
        chip7->Register.V[0x0F] = chip7->Register.V[y] > chip7->Register.V[x];
        chip7->Register.V[x] = chip7->Register.V[y] - chip7->Register.V[x];
        break;

    // 8xyE - SHL Vx {, Vy} : Set Vx = Vx SHL 1.
    case 0x0E:
        chip7->Register.V[0x0F] = 0;
        if (chip7->Register.V[x] & 0b10000000)
        {
            chip7->Register.V[0x0F] = 1;
        }
        chip7->Register.V[x] = chip7->Register.V[x] * 2;
        break;

    default:
        break;
    }
}

static char chip7_wait_for_key_press(struct Chip7 *chip7)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type != SDL_KEYDOWN)
        {
            continue;
        }
        char c = event.key.keysym.sym;

        char chip7_key = chip_key_map(&chip7->Keyboard, c);
        if (chip7_key != -1)
        {
            return chip7_key;
        }
    }
    return -1;
}

static void chip7_exec_extended_F(struct Chip7 *chip7, unsigned short opcode)
{
    unsigned char x = (opcode >> 8) & 0x000F;
    switch (opcode & 0x00FF)
    {
        // Fx07 - LD Vx, DT : Set Vx = delay timer value.
    case 0x07:
        chip7->Register.V[x] = chip7->Register.DT;
        break;

    // Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx.
    case 0x0A:
    {
        char pressed_key = chip7_wait_for_key_press(chip7);
        chip7->Register.V[x] = pressed_key;
    }
    break;

    // Fx15 - LD DT, Vx : Set delay timer = Vx.
    case 0x15:
        chip7->Register.DT = chip7->Register.V[x];
        break;

    // Fx18 - LD ST, Vx : Set sound timer = Vx.
    case 0x18:
        chip7->Register.ST = chip7->Register.V[x];
        break;

    // Fx1E - ADD I, Vx : Set I = I + Vx.
    case 0x1E:
        chip7->Register.I += chip7->Register.V[x];
        break;

    // Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
    case 0x29:
        chip7->Register.I = chip7->Register.V[x] * CHIP7_DEFAULT_SPRITE_HEIGHT;
        break;

    // Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2.
    case 0x33:
    {
        unsigned char hundreds = chip7->Register.V[x] / 100;
        unsigned char tens = chip7->Register.V[x] / 10 % 10;
        unsigned char units = chip7->Register.V[x] % 10;
        chip7_mem_set(&chip7->Memory, chip7->Register.I, hundreds);
        chip7_mem_set(&chip7->Memory, chip7->Register.I + 1, tens);
        chip7_mem_set(&chip7->Memory, chip7->Register.I + 2, units);
    }
    break;

    // Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I.
    case 0x55:
        for (int i = 0; i <= x; i++)
        {
            chip7_mem_set(&chip7->Memory, chip7->Register.I + i, chip7->Register.V[i]);
        }
        break;

    // Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I.
    case 0x65:
        for (int i = 0; i < x; i++)
        {
            chip7->Register.V[i] = chip7_mem_get(&chip7->Memory, chip7->Register.I + i);
        }
        break;

    default:
        break;
    }
}

static void chip7_exec_extended(struct Chip7 *chip7, unsigned short opcode)
{
    unsigned short nnn = opcode & 0x0FFF;
    unsigned char x = (opcode >> 8) & 0x000F;
    unsigned char y = (opcode >> 4) & 0x000F;
    unsigned char kk = opcode & 0x00FF;
    unsigned char n = opcode & 0x000F;

    switch (opcode & 0xF000)
    {
    // JP addr : 1nnn Jump to location nnn.
    case 0x1000:
        chip7->Register.PC = nnn;
        break;

    //  CALL addr : Call subroutine at nnn.
    case 0x2000:
        chip7_stack_push(chip7, chip7->Register.PC);
        chip7->Register.PC = nnn;
        break;

    // SE Vx, byte : Skip next instruction if Vx = kk.
    case 0x3000:
        if (chip7->Register.V[x] == kk)
        {
            chip7->Register.PC += 2;
        }
        break;

    // SNE Vx, byte : Skip next instruction if Vx != kk.
    case 0x4000:
        if (chip7->Register.V[x] != kk)
        {
            chip7->Register.PC += 2;
        }
        break;

    // SE Vx, Vy : Skip next instruction if Vx = Vy.
    case 0x5000:
        if (chip7->Register.V[x] == chip7->Register.V[y])
        {
            chip7->Register.PC += 2;
        }
        break;

    // LD Vx, byte : Set Vx = kk.
    case 0x6000:
        chip7->Register.V[x] = kk;
        break;

    // ADD Vx, byte : Set Vx = Vx + kk.
    case 0x7000:
        chip7->Register.V[x] += kk;
        break;

    case 0x8000:
        chip7_exec_extended_8(chip7, opcode);
        break;

    // 9xy0 - SNE Vx, Vy : Skip next instruction if Vx != Vy.
    case 0x9000:
        if (chip7->Register.V[x] != chip7->Register.V[y])
        {
            chip7->Register.PC += 2;
        }
        break;

    // Annn - LD I, addr : Set I = nnn.
    case 0xA000:
        chip7->Register.I = nnn;
        break;

    // Bnnn - JP V0, addr : Jump to location nnn + V0.
    case 0xB000:
        chip7->Register.PC == nnn + chip7->Register.V[0x00];
        break;

    // Cxkk - RND Vx, byte : Set Vx = random byte AND kk.
    case 0xC000:
        srand(clock());
        chip7->Register.V[x] = (rand() % 255) & kk;
        break;

    //
    case 0xD000:
    {
        const char *sprite = (const char *)&chip7->Memory.MemoryByte[chip7->Register.I];
        chip7->Register.V[0x0F] = chip7_screen_draw_sprite(&chip7->Screen, chip7->Register.V[x], chip7->Register.V[y], sprite, n);
    }
    break;

        // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed.

    case 0xE000:
    {
        switch (opcode & 0x00FF)
        {
            // Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed.
        case 0x9E:
            if (chip7_is_key_down(&chip7->Keyboard, chip7->Register.V[x]))
            {
                chip7->Register.PC += 2;
            }
            break;

            // ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
        case 0xA1:
            if (!chip7_is_key_down(&chip7->Keyboard, chip7->Register.V[x]))
            {
                chip7->Register.PC += 2;
            }
            break;

        default:
            break;
        }
    }
    break;

    case 0xF000:
        chip7_exec_extended_F(chip7, opcode);
        break;

    default:
        break;
    }
}

void chip7_exec(struct Chip7 *chip7, unsigned short opcode)
{
    switch (opcode)
    {

    // CLS : Clear the display.
    case 0x00E0:
        chip7_screen_clear(&chip7->Screen);
        break;

    // RET : Return from a subroutine.
    case 0x00EE:
        chip7->Register.PC = chip7_stack_pop(chip7);
        break;

    default:
        chip7_exec_extended(chip7, opcode);
        break;
    }
}