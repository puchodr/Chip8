#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "chip8.h"
#include "graphics.h"


int main (int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Palette> <ROM>\nOnly include the name of the ROM, we assume all of the ROMs are in ./roms\n";
        std::exit(EXIT_FAILURE);
    }

    int scale = atoi(argv[1]);
    int palette = atoi(argv[2]);
    std::string rom = argv[3];
    rom = "./roms/" + rom;

    // @Todo: check argc and argv for the path to the rom to load
    SDL_Init(SDL_INIT_EVERYTHING);
    Graphics graphics("Chip-8",
            scale * Chip8::SCREEN_WIDTH,
            scale * Chip8::SCREEN_HEIGHT,
            Chip8::SCREEN_WIDTH,
            Chip8::SCREEN_HEIGHT);
    Chip8 chip8(rom, palette, graphics);

    chip8.event_loop();

    return 0;
}
