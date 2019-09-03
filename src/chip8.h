#ifndef CHIP8_H_
#define CHIP8_H_

#include <memory>

#include "input.h"

struct Graphics;

struct Chip8
{
    Chip8(Graphics& graphics);
    ~Chip8();

    void event_loop();

    static const uint16_t MEMORY_SIZE = 4096;
    static const uint16_t INITIAL_PC = 0x200;
    static const uint16_t MAX_ROM_SIZE = MEMORY_SIZE - INITIAL_PC;
    static const uint16_t SPRITE_OFFSET = 0x50;

    static const uint8_t SCREEN_WIDTH = 64;
    static const uint8_t SCREEN_HEIGHT = 32;

private:
    void init_registers();
    void dump_registers() const;
    void init_memory(const char *file_path);
    void process_input();

    unsigned char memory[MEMORY_SIZE];

    // Registers
    uint8_t V[16];
    uint16_t I;
    uint8_t DT;
    uint32_t delay_time;
    uint8_t ST;
    uint8_t sound_time;

    // Pseudo-Registers
    uint16_t PC;
    uint8_t SP;
    uint16_t stack[16];
    bool key[16]; // Pressed = true

    bool wait;
    bool draw_flag = false;

    Input input;
    SDL_Event event;
    Graphics& graphics;
};

#endif // CHIP8_H_
