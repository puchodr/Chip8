#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "chip8.h"

#include "opcodes.h"
#include "graphics.h"
#include "input.h"

#define Vx ((opcode & 0x0F00) >> 8)
#define Vy ((opcode & 0x00F0) >> 4)

Chip8::Chip8(Graphics& graphics) :
    graphics(graphics)
{
    srand (static_cast<unsigned int>(time(NULL)));
    init_memory("roms/test_opcode.ch8");
    init_registers();
}

Chip8::~Chip8()
{
    SDL_Quit();
}

void Chip8::event_loop()
{
    // @Todo: check argc and argv for the path to the rom to load
    uint16_t opcode = 0;

    for(;;)
    {
        process_input();

        // @Todo: Uncomment this whenever you implement the keyboard for LD_XK
        //if (!wait)
        //{
        opcode = (memory[PC++] << 8);
        opcode |= memory[PC++];
        //}

        switch(opcode & 0xF000)
        {
            case MASK_0:
                switch (opcode & 0xF0FF)
                {
                    case CLS:
                        std::cout << "CLS: " << std::hex << std::hex << opcode << std::dec << std::dec << std::endl;

                        // @Todo: Clear the graphics screen
                        dump_registers();
                        break;

                    case RET:
                        std::cout << "RET: " << std::hex << std::hex << opcode << std::dec << std::dec << std::endl;
                        PC = stack[--SP];
                        stack[SP] = 0;
                        dump_registers();
                        break;

                    default:
                        std::cout << "Assuming SYS, we're ignoring opcode: " << std::hex << opcode << std::dec << std::endl;
                        dump_registers();
                        break;
                }
                break;

            case JP:
                std::cout << "JP: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  PC = " << (opcode & 0x0FFF) << std::endl;

                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case CALL:
                std::cout << "CALL: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  PC = " << (opcode & 0x0FFF) << std::endl;

                stack[SP++] = PC;
                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case SE_X:
                std::cout << "SE_X: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  " << (unsigned)V[Vx] << " == " << (opcode & 0x00FF) << std::endl;

                PC += (V[Vx] == (opcode & 0x00FF)) ? 2 : 0;
                dump_registers();
                break;

            case SNE_X:
                std::cout << "SNE_X: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  " << (unsigned)V[Vx] << " != " << (opcode & 0x00FF) << std::endl;

                PC += (V[Vx] != (opcode & 0x00FF)) ? 2 : 0;
                dump_registers();
                break;

            case SE_XY:
                std::cout << "SE_XY: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  " << (unsigned)V[Vx] << " == " << (unsigned)V[Vy] << std::endl;

                PC += (V[Vx] == V[Vy]) ? 2 : 0;
                dump_registers();
                break;

            case LD_X:
                std::cout << "LD_X: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  V" << (unsigned) Vx << " = " << (opcode & 0x00FF) << std::endl;

                V[Vx] = (opcode & 0x00FF);
                dump_registers();
                break;

            case ADD_X:
                std::cout << "ADD_X: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  V" << (unsigned) Vx << " = " << (unsigned)V[Vx] << " + " << (opcode & 0x00FF) << std::endl;

                V[Vx] += (opcode & 0x00FF);
                dump_registers();
                break;

            case MASK_8:
                switch (opcode & 0xF00F)
                {
                    case LD_XY:
                        std::cout << "LD_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned)V[Vy] << std::endl;

                        V[Vx] = V[Vy];
                        dump_registers();
                        break;

                    case OR_XY:
                        std::cout << "OR_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " | " << (unsigned) V[Vy] << std::endl;

                        V[Vx] |= V[Vy];
                        dump_registers();
                        break;

                    case AND_XY:
                        std::cout << "AND_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " & " << (unsigned) V[Vy] << std::endl;

                        V[Vx] &= V[Vy];
                        dump_registers();
                        break;

                    case XOR_XY:
                        std::cout << "XOR_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " ^ " << (unsigned) V[Vy] << std::endl;

                        V[Vx] ^= V[Vy];
                        dump_registers();
                        break;

                    case ADD_XY:
                        std::cout << "ADD_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " + " << (unsigned) V[Vy] << std::endl;
                        V[0xF] = ((V[Vx] + V[Vy]) > 255) ? 1 : 0;

                        V[Vx] += V[Vy];
                        dump_registers();
                        break;

                    case SUB_XY:
                        std::cout << "SUB_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " - " << (unsigned) V[Vy] << std::endl;
                        V[0xF] = (V[Vx] > V[Vy]) ? 1 : 0;

                        V[Vx] -= V[Vy];
                        dump_registers();
                        break;

                    case SHR_X:
                        // @Todo: Figure out what Vy is used for
                        std::cout << "SHR_X: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " >> 1 " << std::endl;
                        //V[0xF] = V[Vx] % 2;
                        V[0xF] = (V[Vx] & 0x1);

                        V[Vx] >>= 1;
                        dump_registers();
                        break;

                    case SUBN_XY:
                        std::cout << "SUBN_XY: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vy] << " - " << (unsigned) V[Vx] << std::endl;
                        V[0xF] = (V[Vy] > V[Vx]) ? 1 : 0;

                        V[Vx] = V[Vy] - V[Vx];
                        dump_registers();
                        break;

                    case SHL_X:
                        // @Todo: Figure out what Vy is used for
                        std::cout << "SHL_X: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " << 1 " << std::endl;
                        V[0xF] = (V[Vx] & 0x80) >> 7;

                        V[Vx] <<= 1;
                        dump_registers();
                        break;

                    default:
                        std::cout << "Don't know how to decode MASK_8 opcode: " << std::hex << std::hex << opcode << std::dec << std::endl;
                        dump_registers();
                        break;
                }
                break;

            case SNE_XY:
                std::cout << "SNE_XY: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  " << (unsigned)V[Vx] << " != " << (unsigned)V[Vy] << std::endl;

                PC += (V[Vx] != V[Vy]) ? 2 : 0;
                dump_registers();
                break;

            case LD_I:
                std::cout << "LD_I: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  I" << " = " << (opcode & 0x0FFF) << std::endl;

                I = (opcode & 0x0FFF);
                dump_registers();
                break;

            case JP_V0:
                std::cout << "JP_V0: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  PC = " << (unsigned) V[0] << " + " << (opcode & 0x0FFF) << std::endl;

                PC = V[0] + opcode & 0x0FFF;
                dump_registers();
                break;

            case RND_X:
                V[Vx] = rand() & 0xFF;
                std::cout << "RND_X: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  V" << (unsigned) Vx << " = " << (unsigned) V[Vx] << " & " << (opcode & 0x00FF) << std::endl;

                V[Vx] &= (opcode & 0x00FF);
                dump_registers();
                break;

            case DRW_XY:
                std::cout << "DRW_XY: " << std::hex << opcode << std::dec << std::endl;
                std::cout << "  " << std::hex << opcode << std::dec << std::endl;

                // @Todo: Figure out drawing to the screen.
                dump_registers();
                break;

            case MASK_E:
                switch (opcode & 0xF0FF)
                {
                    case SKP_X:
                        std::cout << "SKP_X: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  " << (int)key[Vx] << " == 1" << std::endl;

                        PC += key[Vx] ? 2 : 0;
                        dump_registers();
                        break;

                    case SKNP_X:
                        std::cout << "SKNP_X: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  " << (int)key[Vx] << " == 0" << std::endl;

                        PC += key[Vx] ? 0 : 2;
                        dump_registers();
                        break;

                    default:
                        std::cout << "Don't know how to decode MASK_E opcode: " << std::hex << std::hex << opcode << std::dec << std::endl;
                        dump_registers();
                        break;
                }
                break;

            case MASK_F:
                switch (opcode & 0xF0FF)
                {
                    case LD_XDT:
                        std::cout << "LD_XDT: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned)DT << std::endl;

                        V[Vx] = DT;
                        dump_registers();
                        break;

                    case LD_XK:
                        std::cout << "LD_XK: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  " << std::hex << opcode << std::dec << std::endl;

                        // @Todo: Pause execution until a key is pressed
                        wait = true;
                        dump_registers();
                        break;

                    case LD_DTX:
                        std::cout << "LD_DTX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  DT = " << (unsigned)V[Vx] << std::endl;

                        DT = V[Vx];
                        delay_time = SDL_GetTicks();
                        dump_registers();
                        break;

                    case LD_STX:
                        std::cout << "LD_STX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  ST = " << (unsigned)V[Vx] << std::endl;

                        ST = V[Vx];
                        sound_time = SDL_GetTicks();
                        dump_registers();
                        break;

                    case ADD_IX:
                        std::cout << "ADD_IX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  I" << " = " << (unsigned) I << " + " << V[Vx] << std::endl;

                        I += V[Vx];
                        dump_registers();
                        break;

                    case LD_FX:
                        std::cout << "LD_FX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  " << std::hex << opcode << std::dec << std::endl;
                        // @Todo: Come back to this when you figure out the graphics
                        dump_registers();
                        break;

                    case LD_BX:
                        std::cout << "LD_BX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  memory at I = " << (unsigned) V[Vx] << std::endl;
                        std::cout << "  memory[I] = " << (unsigned) (V[Vx] / 100) << std::endl;
                        std::cout << "  memory[I+1] = " << (unsigned) ((V[Vx] % 100) / 10) << std::endl;
                        std::cout << "  memory[I+2] = " << (unsigned) (V[Vx] % 10) << std::endl;

                        memory[I] = V[Vx] / 100;
                        memory[I+1] = (V[Vx] % 100) / 10;
                        memory[I+2] = V[Vx] % 10;
                        dump_registers();
                        break;

                    case LD_IV:
                        std::cout << "LD_IV: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  Storing V0-V" << (unsigned) Vx << " in memory starting at I" << std::endl;

                        for (int i=0; i<Vx; ++i)
                        {
                            memory[I+i] = V[i];
                        }
                        dump_registers();
                        break;

                    case LD_VI:
                        std::cout << "LD_VI: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  Storing I-I+" << (unsigned) Vx << " into V0-V" << (unsigned) Vx << std::endl;

                        for (int i=0; i<Vx; ++i)
                        {
                            V[i] = memory[I+i];
                        }
                        dump_registers();
                        break;

                    default:
                        std::cout << "Don't know how to decode MASK_F opcode: " << std::hex << std::hex << opcode << std::dec << std::endl;
                        dump_registers();
                        break;
                }
                break;

            default:
                std::cout << "Don't know how to decode opcode: " << std::hex << std::hex << opcode << std::dec << std::endl;
                dump_registers();
                break;
        }

        // @Todo: Test out if we need to really limit it to 60Hz
        if (DT)
        {
            uint32_t elapsed_time = SDL_GetTicks() - delay_time;
            if (elapsed_time >= (1000/60))
            {
                DT--;
                delay_time = SDL_GetTicks();
            }
        }

        // @Todo: Test out if we need to really limit it to 60Hz
        if (ST)
        {
            uint32_t elapsed_time = SDL_GetTicks() - sound_time;
            if (elapsed_time >= (1000/60))
            {
                ST--;
                sound_time = SDL_GetTicks();
            }
        }
    }
}

void Chip8::init_registers()
{
    memset (V, 0, 16 * sizeof(uint8_t));
    I = 0;
    DT = 0;
    ST = 0;

    PC = INITIAL_PC;
    SP = 0;
    memset (stack, 0, 16 * sizeof(uint16_t));
    memset (key, false, 16 * sizeof(bool));

    wait = false;
}

void Chip8::dump_registers() const
{
    printf("Internal Data:\n");
    printf("============================\n");
    printf("PC = %X\tSP = %u\n", PC, SP);
    printf("DT = %u\t\tST = %u\n", DT, ST);
    printf("I  = %u\n", I);

    for (int i=0; i<16; ++i)
    {
        printf("V[%X] = %u\tS[%X] = %u\n", i, V[i], i, stack[i]);
    }

    printf("============================\n\n");
}

void Chip8::init_memory(const char *file_path)
{
    unsigned char fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    memset (memory, 0, MEMORY_SIZE * sizeof(unsigned char));
    for (int i=0; i<80; ++i)
    {
        memory[SPRITE_OFFSET + i] = fontset[i];
    }

    std::ifstream is (file_path, std::ifstream::binary);

    if (is)
    {
        is.seekg (0, is.end);
        int length = is.tellg();
        std::cout << "File Length: " << length << std::endl;
        is.seekg (0, is.beg);

        if (length > (MAX_ROM_SIZE))
        {
            std::cout << "Error: this rom is too big for Chip8 ROM." << std::endl;
            exit(EXIT_FAILURE);
        }

        is.read((char*)(&memory[INITIAL_PC]), length);

        if (is)
        {
            std::cout << "File was read successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: only " << is.gcount() << " bytes could be read." << std::endl;
            exit(EXIT_FAILURE);
        }

        is.close();
    }
}

void Chip8::process_input()
{
    input.beginNewFrame();

    // This gets all the SDL_Events and checks for key presses, then stores
    // those key presses in a map to be checked later.
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                input.keyDownEvent(event);
                break;
            case SDL_KEYUP:
                input.keyUpEvent(event);
                break;
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    /*
     * Keypad       Keyboard
     * +-+-+-+-+    +-+-+-+-+
     * |1|2|3|C|    |1|2|3|4|
     * +-+-+-+-+    +-+-+-+-+
     * |4|5|6|D|    |Q|W|E|R|
     * +-+-+-+-+ => +-+-+-+-+
     * |7|8|9|E|    |A|S|D|F|
     * +-+-+-+-+    +-+-+-+-+
     * |A|0|B|F|    |Z|X|C|V|
     * +-+-+-+-+    +-+-+-+-+
     */

    key[0x1] = input.isKeyHeld(SDL_SCANCODE_1);
    key[0x2] = input.isKeyHeld(SDL_SCANCODE_2);
    key[0x3] = input.isKeyHeld(SDL_SCANCODE_3);
    key[0xC] = input.isKeyHeld(SDL_SCANCODE_4);

    key[0x4] = input.isKeyHeld(SDL_SCANCODE_Q);
    key[0x5] = input.isKeyHeld(SDL_SCANCODE_W);
    key[0x6] = input.isKeyHeld(SDL_SCANCODE_E);
    key[0xD] = input.isKeyHeld(SDL_SCANCODE_R);

    key[0x7] = input.isKeyHeld(SDL_SCANCODE_A);
    key[0x8] = input.isKeyHeld(SDL_SCANCODE_S);
    key[0x9] = input.isKeyHeld(SDL_SCANCODE_D);
    key[0xE] = input.isKeyHeld(SDL_SCANCODE_F);

    key[0xA] = input.isKeyHeld(SDL_SCANCODE_Z);
    key[0x0] = input.isKeyHeld(SDL_SCANCODE_X);
    key[0xB] = input.isKeyHeld(SDL_SCANCODE_C);
    key[0xF] = input.isKeyHeld(SDL_SCANCODE_V);

    if (input.isKeyHeld(SDL_SCANCODE_BACKSPACE))
        exit(EXIT_SUCCESS);
}
