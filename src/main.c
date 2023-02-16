#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <windows.h>

#include "Config.h"
#include "Chip7.h"

#include "Chip7_keyboard.h"

int millisecsPreviousFrame = 0;

const char keyboard_map[CHIP7_KEY_TOTAL] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f};

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Must provide a file to load.");
        return -1;
    }

    const char *filename = argv[1];
    printf("Now loading : %s\n", filename);

    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        printf("Failed to open file.");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char buffer[size];
    int res = fread(buffer, size, 1, f);
    if (res != 1)
    {
        printf("Failed to read file.");
        return -1;
    }   

    struct Chip7 ThisChip7;
    chip7_init(&ThisChip7);
    chip7_load(&ThisChip7, buffer, size);
    chip7_keyboard_set_map(&ThisChip7.Keyboard, keyboard_map);

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CHIP7_SCREEN_WIDTH * CHIP7_RENDER_SCALE,
        CHIP7_SCREEN_HEIGHT * CHIP7_RENDER_SCALE,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_TEXTUREACCESS_TARGET);

    bool quit = false;

    while (!quit)
    {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
            {
                char key = event.key.keysym.sym;
                int vkey = chip_key_map(&ThisChip7.Keyboard, key);
                if (vkey != -1)
                {
                    chip7_key_down(&ThisChip7.Keyboard, vkey);
                }
            }
            break;

            case SDL_KEYUP:
            {
                char key = event.key.keysym.sym;
                int vkey = chip_key_map(&ThisChip7.Keyboard, key);
                if (vkey != -1)
                {
                    chip7_key_up(&ThisChip7.Keyboard, vkey);
                }
            }
            break;

            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x13, 0x13, 0x11, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFC, 0xFF, 0);

        for (int x = 0; x < CHIP7_SCREEN_WIDTH; x++)
        {
            for (int y = 0; y < CHIP7_SCREEN_HEIGHT; y++)
            {
                if (chip7_screen_get(&ThisChip7.Screen, x, y))
                {
                    SDL_Rect r;
                    r.x = x * CHIP7_RENDER_SCALE;
                    r.y = y * CHIP7_RENDER_SCALE;
                    r.w = CHIP7_RENDER_SCALE;
                    r.h = CHIP7_RENDER_SCALE;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (ThisChip7.Register.DT > 0)
        {
            Sleep(10);
            ThisChip7.Register.DT -= 1;
        }
        if (ThisChip7.Register.ST > 0)
        {
            Beep(1000, 10 * ThisChip7.Register.ST);
            ThisChip7.Register.ST = 0;
        }

        unsigned short opcode = chip7_mem_get_short(&ThisChip7.Memory, ThisChip7.Register.PC);  
        ThisChip7.Register.PC += 2;
        chip7_exec(&ThisChip7, opcode);
    }

    SDL_DestroyWindow(window);
    return 0;
}