#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "chip8.h"
#include "graphics.h"


int main (int argc, char **argv)
{
    // @Todo: check argc and argv for the path to the rom to load
    SDL_Init(SDL_INIT_EVERYTHING);
    Graphics graphics("Chip-8",
            10 * Chip8::SCREEN_WIDTH,
            10 * Chip8::SCREEN_HEIGHT,
            Chip8::SCREEN_WIDTH,
            Chip8::SCREEN_HEIGHT);
    Chip8 chip8("roms/test_opcode.ch8", graphics);

    chip8.event_loop();
}
