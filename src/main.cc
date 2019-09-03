#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "chip8.h"
#include "graphics.h"


int main (int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Graphics graphics;
    Chip8 chip8("roms/test_opcode.ch8", graphics);

    chip8.event_loop();
}
