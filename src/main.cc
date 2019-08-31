#include <string.h>
#include <iostream>
#include <fstream>

uint16_t InitialPC = 0x200;

int main (int argc, char **argv)
{
    //Counter = InterruptPeriod;

    // @Todo: check argc and argv for the path to the rom to load

    char memory[4096];
    memset (memory, 0, 4096);
    const char* file_path = "roms/test_opcode.ch8";
    std::ifstream is (file_path, std::ifstream::binary);
    int length = 0;

    if (is)
    {
        is.seekg (0, is.end);
        int length = is.tellg();
        //std::cout << "File Length: " << length << std::endl;
        is.seekg (0, is.beg);

        // Caps the file size to the size of the chip8 RAM
        length = length > (4096 - 0x200) ? (4096 - 0x200) : length;
        is.read(&(memory[0x200]), length);

        if (is)
        {
            //std::cout << "File was read successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: only " << is.gcount() << " bytes could be read." << std::endl;
        }

        is.close();
    }

    uint16_t PC = InitialPC;

    uint16_t opcode = 0;

    for(;;)
    {
        opcode = memory[PC++];
        //Counter -= Cycles[OpCode];

        switch(opcode)
        {
            case 0x0000:
            default:
                break;
        }

        //if(Counter<=0)
        //{
            //[> Check for interrupts and do other <]
            //[> cyclic tasks here                 <]
            //...
            ////Counter+=InterruptPeriod;
            //if(ExitRequired)
            //{
                //break;
            //}
        //}
    }

    return 0;
}
