#include <string.h>
#include <iostream>
#include <fstream>

#include "opcodes.h"

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

            default:
                break;
        }
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
