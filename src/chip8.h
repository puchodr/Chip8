#ifndef CHIP8_H_
#define CHIP8_H_

#include "input.h"

struct Graphics;

struct Chip8
{
    Chip8(std::string ROM, Graphics& graphics);
    ~Chip8();

    void event_loop();

    static const uint16_t MEMORY_SIZE = 4096;
    static const uint16_t INITIAL_PC = 0x200;
    static const uint16_t MAX_ROM_SIZE = MEMORY_SIZE - INITIAL_PC;
    static const uint16_t SPRITE_OFFSET = 0x50;

    static const uint8_t SCREEN_WIDTH = 64;
    static const uint8_t SCREEN_HEIGHT = 32;

    enum OpcodeType
    {
        MASK_0  = 0x0000,   // Used to check for additional opcodes
        MASK_8  = 0x8000,   // Used to check for additional opcodes
        MASK_E  = 0xE000,   // Used to check for additional opcodes
        MASK_F  = 0xF000,   // Used to check for additional opcodes

        // Chip-8 Instructions
        SYS     = 0x0000,   // Ignored
        CLS     = 0x00E0,   // Clear the Display
        RET     = 0x00EE,   // Return from subroutine
        EXIT    = 0x00FD,   // Exit
        JP      = 0x1000,   // Jump to location addr
        CALL    = 0x2000,   // Call subroutine at addr
        SE_X    = 0x3000,   // Skip next instruction if Vx = kk
        SNE_X   = 0x4000,   // Skip next instruction if Vx != kk
        SE_XY   = 0x5000,   // Skip next instruction if Vx = Vy
        LD_X    = 0x6000,   // Set Vx = kk
        ADD_X   = 0x7000,   // Set Vx = Vx + kk
        LD_XY   = 0x8000,   // Set Vx = Vy
        OR_XY   = 0x8001,   // Set Vx = Vx OR Vy
        AND_XY  = 0x8002,   // Set Vx = Vx AND Vy
        XOR_XY  = 0x8003,   // Set Vx = Vx XOR Vy
        ADD_XY  = 0x8004,   // Set Vx = Vx + Vy, set VF = carry
        SUB_XY  = 0x8005,   // Set Vx = Vx - Vy, set VF = NOT borrow
        SHR_X   = 0x8006,   // Set Vx = VX >> 1, VF = 1 if LSB == 1
        SUBN_XY = 0x8007,   // Set Vx = Vy - Vx, set VF = NOT borrow
        SHL_X   = 0x800E,   // Set Vx = Vy << 1, VF = 1 if MSB == 1
        SNE_XY  = 0x9000,   // Skip next instruction if Vx != Vy
        LD_I    = 0xA000,   // Set I = addr
        JP_V0   = 0xB000,   // Jump to location V0 + addr
        RND_X   = 0xC000,   // Set Vx = rand() & kk
        DRW_XY  = 0xD000,   // Draws sprite at coordinate Vx, Vy
        SKP_X   = 0xE09E,   // Skip next instruction if key with value Vx is pressed
        SKNP_X  = 0xE0A1,   // Skip next instruction if key with value Vx is not pressed
        LD_XDT  = 0xF007,   // Set Vx = delay timer value
        LD_XK   = 0xF00A,   // Wait for a key press, store value of key in Vx
        LD_DTX  = 0xF015,   // Set delay timer = Vx
        LD_STX  = 0xF018,   // Set sound timer = Vx
        ADD_IX  = 0xF01E,   // Set I = I + Vx
        LD_FX   = 0xF029,   // Set I = location of sprite for digit Vx
        LD_BX   = 0xF033,   // Store BCD represenstation of Vx in I, I+1, and I+2
        LD_IV   = 0xF055,   // Store registers V0-Vx in memory starting at I
        LD_VI   = 0xF065,   // Read V0-Vx from memory starting at I
    };

    enum ColorPalette
    {
        BLACK_WHITE = 0,
        WHITE_BLACK,
        YELLOW_BLUE,
        BLUE_YELLOW,
        NAVY_TEAL,
        TEAL_NAVY,
        BLACK_ORANGE,
        ORANGE_BLACK,
        MAROON_PEACH,
        PEACH_MAROON,
        PURPLE_BLUE,
        BLUE_PURPLE,
        NAVY_ORANGE,
        ORANGE_NAVY,
    };

    private:
    void dump_opcode(OpcodeType type, uint16_t opcode) const;
    void init_registers();
    void dump_registers() const;
    void init_memory(const char *file_path);
    void process_input();

    uint8_t memory[MEMORY_SIZE];

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
    uint32_t gfx_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

    std::string ROM;
    bool wait;
    bool draw_flag;
    bool quit;

    uint32_t PIXEL_ON;
    uint32_t PIXEL_OFF;

    Input input;
    SDL_Event event;
    Graphics& graphics;
};

#endif // CHIP8_H_
