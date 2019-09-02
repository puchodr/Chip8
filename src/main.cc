#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include "opcodes.h"

#define Vx ((opcode & 0x0F00) >> 8)
#define Vy ((opcode & 0x00F0) >> 4)

uint16_t InitialPC = 0x200;
unsigned char memory[4096];

// Registers
uint8_t V[16];
uint16_t I;
uint8_t delay_timer;
uint8_t sound_timer;

// Pseudo-Registers
uint16_t PC;
uint8_t SP;
uint16_t stack[16];
bool key[16]; // Pressed = true
bool wait;

void init_registers();
void dump_registers();
void init_memory(const char *file_path);

int main (int argc, char **argv)
{
    srand (time(0));
    // @Todo: check argc and argv for the path to the rom to load
    init_memory("roms/test_opcode.ch8");

    PC = InitialPC;
    uint16_t opcode = 0;

    for(;;)
    {
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
                        std::cout << "  V" << (unsigned) Vx << " = " << (unsigned)delay_timer << std::endl;

                        V[Vx] = delay_timer;
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

                        delay_timer = V[Vx];
                        dump_registers();
                        break;

                    case LD_STX:
                        std::cout << "LD_STX: " << std::hex << opcode << std::dec << std::endl;
                        std::cout << "  ST = " << (unsigned)V[Vx] << std::endl;

                        sound_timer = V[Vx];
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

        //dump_registers();
    }

    return 0;
}

void init_registers()
{
    memset (V, 0, 16 * sizeof(uint8_t));
    I = 0;
    delay_timer = 0;
    sound_timer = 0;

    PC = InitialPC;
    SP = 0;
    memset (stack, 0, 16 * sizeof(uint16_t));
    memset (key, false, 16 * sizeof(bool));

    wait = false;
}

void dump_registers()
{
    std::cout << std::dec;
    std::cout << "  Vx:" << std::endl;
    for (int i=0; i<16; ++i)
    {
        std::cout << "    V" << (int)i << ": " << (unsigned)V[i] << std::endl;
    }

    std::cout << "  I: " << (unsigned)I << std::endl;
    std::cout << "  Delay Timer: " << (unsigned)delay_timer << std::endl;
    std::cout << "  Sound Timer: " << (unsigned)sound_timer << std::endl;

    // Pseudo-Registers
    std::cout << "  PC: " << (unsigned)PC << std::endl;
    std::cout << "  SP: " << (unsigned)SP << std::endl;

    std::cout << "  Stack:" << std::endl;
    for (int i=0; i<16; ++i)
    {
        std::cout << "    S" << (int)i << ": " << (unsigned)stack[i] << std::endl;
    }

    std::cout << std::endl;
}

void init_memory(const char *file_path)
{
    memset (memory, 0, 4096 * sizeof(unsigned char));
    std::ifstream is (file_path, std::ifstream::binary);

    if (is)
    {
        is.seekg (0, is.end);
        int length = is.tellg();
        std::cout << "File Length: " << length << std::endl;
        is.seekg (0, is.beg);

        // Caps the file size to the size of the chip8 RAM
        length = length > (4096 - 0x200) ? (4096 - 0x200) : length;
        is.read((char*)(&memory[0x200]), length);

        if (is)
        {
            std::cout << "File was read successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: only " << is.gcount() << " bytes could be read." << std::endl;
        }

        is.close();
    }
}
