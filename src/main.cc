#include <string.h>
#include <iostream>
#include <fstream>

#include "opcodes.h"

#define Vx ((opcode & 0x0F00) >> 8)
#define Vy ((opcode & 0x00F0) >> 4)

uint16_t InitialPC = 0x200;
char memory[4096];

// Registers
int8_t V[16];
int16_t I;
uint8_t VF;
uint8_t delay_timer;
uint8_t sound_timer;

// Pseudo-Registers
uint16_t PC;
uint8_t SP;
uint16_t stack[16];

void init_registers();
void dump_registers();
void init_memory(const char *file_path);

int main (int argc, char **argv)
{
    // @Todo: check argc and argv for the path to the rom to load
    init_memory("roms/test_opcode.ch8");

    PC = InitialPC;
    uint16_t opcode = 0;

    for(;;)
    {
        opcode = (memory[PC++] << 8);
        opcode |= memory[PC++];

        switch(opcode & 0xF000)
        {
            case MASK_0:
                switch (opcode & 0xF0FF)
                {
                    case CLS:
                        std::cout << "CLS: " << opcode << std::endl;

                        // @Todo: Clear the graphics screen
                        break;

                    case RET:
                        std::cout << "RET: " << opcode << std::endl;
                        PC = stack[--SP];
                        dump_registers();
                        break;

                    default: // Assuming SYS, we're ignoring this
                        break;
                }
                break;

            case JP:
                std::cout << "JP: PC = " << (opcode & 0x0FFF) << std::endl;

                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case CALL:
                std::cout << "CALL: PC = " << (opcode & 0x0FFF) << std::endl;

                stack[SP++] = PC;
                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case SE_X:
                std::cout << "SE_X: " << (int)V[Vx] << " == " << (opcode & 0x00FF) << std::endl;

                if (V[Vx] == (opcode & 0x00FF))
                {
                    PC += 2;
                }
                dump_registers();
                break;

            case SNE_X:
                std::cout << "SNE_X: " << (int)V[Vx] << " != " << (opcode & 0x00FF) << std::endl;

                if (V[Vx] != (opcode & 0x00FF))
                {
                    PC += 2;
                }
                dump_registers();
                break;

            case SE_XY:
                std::cout << "SE_XY: " << (int)V[Vx] << " == " << (int)V[Vy] << std::endl;

                if (V[Vx] == V[Vy])
                {
                    PC += 2;
                }
                dump_registers();
                break;

            case LD_X:
                std::cout << "LD_X: V" << Vx << " = " << (opcode & 0x00FF) << std::endl;

                V[Vx] = (opcode & 0x00FF);
                dump_registers();
                break;

            case ADD_X:
                std::cout << "ADD_X: V" <<Vx << " = " << V[Vx] << " + " << (opcode & 0x00FF) << std::endl;

                V[Vx] += (opcode & 0x00FF);
                dump_registers();
                break;

            case MASK_8:
                switch (opcode & 0xF00F)
                {
                    case LD_XY:
                        std::cout << "LD_XY: " << opcode << std::endl;
                        break;

                    case OR_XY:
                        std::cout << "OR_XY: " << opcode << std::endl;
                        break;

                    case AND_XY:
                        std::cout << "AND_XY: " << opcode << std::endl;
                        break;

                    case XOR_XY:
                        std::cout << "XOR_XY: " << opcode << std::endl;
                        break;

                    case ADD_XY:
                        std::cout << "ADD_XY: " << opcode << std::endl;
                        break;

                    case SUB_XY:
                        std::cout << "SUB_XY: " << opcode << std::endl;
                        break;

                    case SHR_X:
                        std::cout << "SHR_X: " << opcode << std::endl;
                        break;

                    case SUBN_XY:
                        std::cout << "SUBN_XY: " << opcode << std::endl;
                        break;

                    case SHL_X:
                        std::cout << "SHL_X: " << opcode << std::endl;
                        break;
                }
                break;

            case SNE_XY:
                std::cout << "SNE_XY: " << opcode << std::endl;
                break;

            case LD_I:
                std::cout << "LD_I: " << opcode << std::endl;
                break;

            case JP_V0:
                std::cout << "JP_V0: " << opcode << std::endl;
                break;

            case RND_X:
                std::cout << "RND_X: " << opcode << std::endl;
                break;

            case DRW_XY:
                std::cout << "DRW_XY: " << opcode << std::endl;
                break;

            case MASK_E:
                switch (opcode & 0xF0FF)
                {
                    case SKP_X:
                        std::cout << "SKP_X: " << opcode << std::endl;
                        break;

                    case SKNP_X:
                        std::cout << "SKNP_X: " << opcode << std::endl;
                        break;
                }
                break;

            case MASK_F:
                switch (opcode & 0xF0FF)
                {
                    case LD_XDT:
                        std::cout << "LD_XDT: " << opcode << std::endl;
                        break;

                    case LD_XK:
                        std::cout << "LD_XK: " << opcode << std::endl;
                        break;

                    case LD_DTX:
                        std::cout << "LD_DTX: " << opcode << std::endl;
                        break;

                    case LD_STX:
                        std::cout << "LD_STX: " << opcode << std::endl;
                        break;

                    case ADD_IX:
                        std::cout << "ADD_IX: " << opcode << std::endl;
                        break;

                    case LD_FX:
                        std::cout << "LD_FX: " << opcode << std::endl;
                        break;

                    case LD_BX:
                        std::cout << "LD_BX: " << opcode << std::endl;
                        break;

                    case LD_IV0:
                        std::cout << "LD_IV0: " << opcode << std::endl;
                        break;

                    case LD_V0I:
                        std::cout << "LD_V0I: " << opcode << std::endl;
                        break;
                }
                break;

            default:
                std::cout << "We've encountered an opcode that we don't know about " << opcode << std::endl;
                break;
        }

        //dump_registers();
    }

    return 0;
}

void init_registers()
{
    memset (V, 0, 16 * sizeof(int8_t));
    I = 0;
    VF = 0;
    delay_timer = 0;
    sound_timer = 0;

    PC = InitialPC;
    SP = 0;
    memset (stack, 0, 16 * sizeof(uint16_t));
}

void dump_registers()
{
    std::cout << "  Vx:" << std::endl;
    for (int i=0; i<16; ++i)
    {
        std::cout << "    V" << (int)i << ": " << (int)V[i] << std::endl;
    }

    std::cout << "  I: " << (int)I << std::endl;
    std::cout << "  VF: " << (unsigned)VF << std::endl;
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
}

void init_memory(const char *file_path)
{
    memset (memory, 0, 4096 * sizeof(char));
    std::ifstream is (file_path, std::ifstream::binary);

    if (is)
    {
        is.seekg (0, is.end);
        int length = is.tellg();
        std::cout << "File Length: " << length << std::endl;
        is.seekg (0, is.beg);

        // Caps the file size to the size of the chip8 RAM
        length = length > (4096 - 0x200) ? (4096 - 0x200) : length;
        is.read(&(memory[0x200]), length);

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
