#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "chip8.h"

#include "graphics.h"
#include "input.h"

#define Vx ((opcode & 0x0F00) >> 8)
#define Vy ((opcode & 0x00F0) >> 4)

const unsigned int FPS = 250;

Chip8::Chip8(
        std::string ROM,
        int palette,
        Graphics& graphics) :
    ROM(ROM),
    wait(false),
    draw_flag(false),
    quit(false),
    graphics(graphics)
{
    switch (palette)
    {
        case BLACK_WHITE:
            PIXEL_ON = 0xFFFFFFFF;
            PIXEL_OFF = 0x00000000;
            break;
        case WHITE_BLACK:
            PIXEL_ON = 0x00000000;
            PIXEL_OFF = 0xFFFFFFFF;
            break;
        case YELLOW_BLUE:
            PIXEL_ON  = 0xF4B41AFF;
            PIXEL_OFF = 0x143D59FF;
            break;
        case BLUE_YELLOW:
            PIXEL_ON = 0x143D59FF;
            PIXEL_OFF = 0xF4B41AFF;
            break;
        case NAVY_TEAL:
            PIXEL_ON = 0x210070FF;
            PIXEL_OFF = 0x60CDCDFF;
            break;
        case TEAL_NAVY:
            PIXEL_ON = 0x60CDCDFF;
            PIXEL_OFF = 0x210070FF;
            break;
        case BLACK_ORANGE:
            PIXEL_ON = 0x111111FF;
            PIXEL_OFF = 0xFF6500FF;
            break;
        case ORANGE_BLACK:
            PIXEL_ON = 0xFF6500FF;
            PIXEL_OFF = 0x111111FF;
            break;
        case MAROON_PEACH:
            PIXEL_ON = 0x5B0E2DFF;
            PIXEL_OFF = 0xFFA781FF;
            break;
        case PEACH_MAROON:
            PIXEL_ON = 0xFFA781FF;
            PIXEL_OFF = 0x5B0E2DFF;
            break;
        case PURPLE_BLUE:
            PIXEL_ON = 0x5E001FFF;
            PIXEL_OFF = 0x00E1D9FF;
            break;
        case BLUE_PURPLE:
            PIXEL_ON = 0x00E1D9FF;
            PIXEL_OFF = 0x5E001FFF;
            break;
        case NAVY_ORANGE:
            PIXEL_ON = 0x210070FF;
            PIXEL_OFF = 0xF49F1CFF;
            break;
        case ORANGE_NAVY:
            PIXEL_ON = 0xF49F1CFF;
            PIXEL_OFF = 0x210070FF;
            break;
    }

    clear_gfx();

    srand (static_cast<unsigned int>(time(NULL)));
    init_memory(ROM.c_str());
    init_registers();
}

Chip8::~Chip8()
{
    SDL_Quit();
}

void Chip8::event_loop()
{
    uint16_t opcode = 0;

    uint32_t draw_time = 0;

    for(;;)
    {
        const unsigned int start_time = SDL_GetTicks();
        process_input();

        if (quit)
        {
            break;
        }

        if (!wait)
        {
            opcode = (memory[PC++] << 8);
            opcode |= memory[PC++];
        }

        switch(opcode & 0xF000)
        {
            case MASK_0:
                switch (opcode & 0xF0FF)
                {
                    case CLS:
                        dump_opcode(CLS, opcode);

                        clear_gfx();
                        draw_flag = true;
                        dump_registers();
                        break;

                    case RET:
                        dump_opcode(RET, opcode);
                        PC = stack[--SP];
                        stack[SP] = 0;
                        dump_registers();
                        break;

                    case EXIT:
                        dump_opcode(EXIT, opcode);
                        quit = true;
                        dump_registers();
                        break;

                    default:
                        dump_opcode(SYS, opcode);
                        dump_registers();
                        break;
                }
                break;

            case JP:
                dump_opcode(JP, opcode);

                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case CALL:
                dump_opcode(CALL, opcode);

                stack[SP++] = PC;
                PC = opcode & 0x0FFF;
                dump_registers();
                break;

            case SE_X:
                dump_opcode(SE_X, opcode);

                PC += (V[Vx] == (opcode & 0x00FF)) ? 2 : 0;
                dump_registers();
                break;

            case SNE_X:
                dump_opcode(SNE_X, opcode);

                PC += (V[Vx] != (opcode & 0x00FF)) ? 2 : 0;
                dump_registers();
                break;

            case SE_XY:
                dump_opcode(SE_XY, opcode);

                PC += (V[Vx] == V[Vy]) ? 2 : 0;
                dump_registers();
                break;

            case LD_X:
                dump_opcode(LD_X, opcode);

                V[Vx] = (opcode & 0x00FF);
                dump_registers();
                break;

            case ADD_X:
                dump_opcode(ADD_X, opcode);

                V[Vx] += (opcode & 0x00FF);
                dump_registers();
                break;

            case MASK_8:
                switch (opcode & 0xF00F)
                {
                    case LD_XY:
                        dump_opcode(LD_XY, opcode);

                        V[Vx] = V[Vy];
                        dump_registers();
                        break;

                    case OR_XY:
                        dump_opcode(OR_XY, opcode);

                        V[Vx] |= V[Vy];
                        dump_registers();
                        break;

                    case AND_XY:
                        dump_opcode(AND_XY, opcode);

                        V[Vx] &= V[Vy];
                        dump_registers();
                        break;

                    case XOR_XY:
                        dump_opcode(XOR_XY, opcode);

                        V[Vx] ^= V[Vy];
                        dump_registers();
                        break;

                    case ADD_XY:
                        dump_opcode(ADD_XY, opcode);
                        V[0xF] = ((V[Vx] + V[Vy]) > 255) ? 1 : 0;

                        V[Vx] += V[Vy];
                        dump_registers();
                        break;

                    case SUB_XY:
                        dump_opcode(SUB_XY, opcode);
                        V[0xF] = (V[Vx] > V[Vy]) ? 1 : 0;

                        V[Vx] -= V[Vy];
                        dump_registers();
                        break;

                    case SHR_X:
                        dump_opcode(SHR_X, opcode);
                        V[0xF] = (V[Vx] & 0x1);

                        V[Vx] >>= 1;
                        dump_registers();
                        break;

                    case SUBN_XY:
                        dump_opcode(SUBN_XY, opcode);
                        V[0xF] = (V[Vy] > V[Vx]) ? 1 : 0;

                        V[Vx] = V[Vy] - V[Vx];
                        dump_registers();
                        break;

                    case SHL_X:
                        dump_opcode(SHL_X, opcode);
                        V[0xF] = (V[Vx] & 0x80) >> 7;

                        V[Vx] <<= 1;
                        dump_registers();
                        break;

                    default:
                        dump_opcode(MASK_8, opcode);
                        dump_registers();
                        break;
                }
                break;

            case SNE_XY:
                dump_opcode(SNE_XY, opcode);

                PC += (V[Vx] != V[Vy]) ? 2 : 0;
                dump_registers();
                break;

            case LD_I:
                dump_opcode(LD_I, opcode);

                I = (opcode & 0x0FFF);
                dump_registers();
                break;

            case JP_V0:
                dump_opcode(JP_V0, opcode);

                PC = V[0] + opcode & 0x0FFF;
                dump_registers();
                break;

            case RND_X:
                V[Vx] = rand() & 0xFF;
                dump_opcode(RND_X, opcode);

                V[Vx] &= (opcode & 0x00FF);
                dump_registers();
                break;

            case DRW_XY:
                dump_opcode(DRW_XY, opcode);

                V[0xF] = 0;
                for (int i = 0; i < (opcode & 0x000F); ++i)
                {
                    uint8_t sprite_row = memory[I+i];
                    for (int j = 0; j < 8; ++j)
                    {
                        uint8_t sprite_pixel = sprite_row & (0x80 >> j);
                        int  gfx_x = (V[Vx] + j) % SCREEN_WIDTH;
                        int  gfx_y = ((V[Vy] + i) * SCREEN_WIDTH) % (SCREEN_WIDTH * SCREEN_HEIGHT);
                        uint32_t *gfx_pixel = &gfx_buffer[gfx_y + gfx_x];

                        if (sprite_pixel)
                        {
                            if (*gfx_pixel == PIXEL_ON)
                            {
                                *gfx_pixel = PIXEL_OFF;
                                V[0xF] = 1;
                            }
                            else
                            {
                                *gfx_pixel = PIXEL_ON;
                            }
                        }
                    }
                }

                draw_flag = true;
                dump_registers();
                break;

            case MASK_E:
                switch (opcode & 0xF0FF)
                {
                    case SKP_X:
                        dump_opcode(SKP_X, opcode);

                        PC += key[V[Vx]] ? 2 : 0;
                        dump_registers();
                        break;

                    case SKNP_X:
                        dump_opcode(SKNP_X, opcode);

                        PC += key[V[Vx]] ? 0 : 2;
                        dump_registers();
                        break;

                    default:
                        dump_opcode(MASK_E, opcode);
                        dump_registers();
                        break;
                }
                break;

            case MASK_F:
                switch (opcode & 0xF0FF)
                {
                    case LD_XDT:
                        dump_opcode(LD_XDT, opcode);

                        V[Vx] = DT;
                        dump_registers();
                        break;

                    case LD_XK:
                        dump_opcode(LD_XK, opcode);

                        wait = true;
                        for (uint8_t i=0; i<16; ++i)
                        {
                            if (key[i])
                            {
                                wait = false;
                                V[Vx] = i;
                                break;
                            }
                        }

                        dump_registers();
                        break;

                    case LD_DTX:
                        dump_opcode(LD_DTX, opcode);

                        DT = V[Vx];
                        delay_time = SDL_GetTicks();
                        dump_registers();
                        break;

                    case LD_STX:
                        dump_opcode(LD_STX, opcode);

                        ST = V[Vx];
                        sound_time = SDL_GetTicks();
                        dump_registers();
                        break;

                    case ADD_IX:
                        dump_opcode(ADD_IX, opcode);

                        I += V[Vx];
                        dump_registers();
                        break;

                    case LD_FX:
                        dump_opcode(LD_FX, opcode);

                        // @Info: Each character takesup 5 bytes.
                        I = SPRITE_OFFSET + (V[Vx] * 5);
                        dump_registers();
                        break;

                    case LD_BX:
                        dump_opcode(LD_BX, opcode);

                        memory[I] = V[Vx] / 100;
                        memory[I+1] = (V[Vx] % 100) / 10;
                        memory[I+2] = V[Vx] % 10;
                        dump_registers();
                        break;

                    case LD_IV:
                        dump_opcode(LD_IV, opcode);

                        for (int i=0; i<=Vx; ++i)
                        {
                            memory[I+i] = V[i];
                        }
                        dump_registers();
                        break;

                    case LD_VI:
                        dump_opcode(LD_VI, opcode);

                        for (int i=0; i<=Vx; ++i)
                        {
                            V[i] = memory[I+i];
                        }
                        dump_registers();
                        break;

                    default:
                        dump_opcode(MASK_F, opcode);
                        dump_registers();
                        break;
                }
                break;

            default:
                // @Info: Passing MASK_0 just cuz. This should output the opcode so we could figure out if there's an issue with
                //        the rom or an issue with the op decoding.
                dump_opcode(MASK_0, opcode);
                dump_registers();
                break;
        }

        uint32_t elapsed_time = SDL_GetTicks() - draw_time;
        if (elapsed_time > (1000/60))
        {
            draw_time = SDL_GetTicks();
            graphics.update_window(gfx_buffer, sizeof(gfx_buffer[0]) * SCREEN_WIDTH);
            draw_flag = false;
        }

        if (DT)
        {
            elapsed_time = SDL_GetTicks() - delay_time;
            if (elapsed_time >= (1000/60))
            {
                DT--;
                delay_time = SDL_GetTicks();
            }
        }

        if (ST)
        {
            elapsed_time = SDL_GetTicks() - sound_time;
            if (elapsed_time >= (1000/60))
            {
                ST--;
                sound_time = SDL_GetTicks();
            }
        }

        if (FPS != 0)
        {
            const unsigned int ms_per_frame = 1000 / FPS;
            const unsigned int elapsed_time = SDL_GetTicks() - start_time;

            // This delays the game to maintain 60 FPS.
            if (elapsed_time < ms_per_frame) {
                SDL_Delay(ms_per_frame - elapsed_time);
            }
        }
    }
}

void Chip8::clear_gfx()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
        gfx_buffer[i] = PIXEL_OFF;
    }
}

void Chip8::init_registers()
{
    memset (V, 0, 16 * sizeof(V[0]));
    I = 0;
    DT = 0;
    ST = 0;

    PC = INITIAL_PC;
    SP = 0;
    memset (stack, 0, 16 * sizeof(stack[0]));
    memset (key, false, 16 * sizeof(key[0]));

    wait = false;
}

void Chip8::dump_opcode(OpcodeType type, uint16_t opcode) const
{
    printf("%0*X - ", 4, PC-2);
    switch(type)
    {
        case SYS:
            printf("SYS\tIGNORED\n");
            break;
        case CLS:
            printf("CLS\n");
            break;
        case RET:
            printf("RET\n");
            break;
        case EXIT:
            printf("EXIT\n");
            break;
        case JP:
            printf("JP %0*X\n", 4, (opcode & 0x0FFF));
            break;
        case CALL:
            printf("CALL %0*X\n", 4, (opcode & 0x0FFF));
            break;
        case SE_X:
            printf("SE V%u, %0*X - SE_X\n", Vx, 4, (opcode & 0x00FF));
            break;
        case SNE_X:
            printf("SNE V%u, %0*X - SNE_X\n", Vx, 4, (opcode & 0x00FF));
            break;
        case SE_XY:
            printf("SE V%u, V%u - SE_XY\n", Vx, Vy);
            break;
        case LD_X:
            printf("LD V%u, %0*X - LD_X\n", Vx, 4, (opcode & 0x00FF));
            break;
        case ADD_X:
            printf("ADD V%u, %0*X - ADD_X\n", Vx, 4, (opcode & 0x00FF));
            break;
        case LD_XY:
            printf("LD V%u, V%u - LD_XY\n", Vx, Vy);
            break;
        case OR_XY:
            printf("OR V%u, V%u - OR_XY\n", Vx, Vy);
            break;
        case AND_XY:
            printf("AND V%u, V%u - AND_XY\n", Vx, Vy);
            break;
        case XOR_XY:
            printf("XOR V%u, V%u - XOR_XY\n", Vx, Vy);
            break;
        case ADD_XY:
            printf("ADD V%u, V%u - ADD_XY\n", Vx, Vy);
            break;
        case SUB_XY:
            printf("SUB V%u, V%u - ADD_XY\n", Vx, Vy);
            break;
        case SHR_X:
            printf("SHR V%u, 1 - SHR_X\n", Vx);
            break;
        case SUBN_XY:
            printf("SUBN V%u, V%u - SUBN_XY\n", Vx, Vy);
            break;
        case SHL_X:
            printf("SHL V%u, 1 - SHL_X\n", Vx);
            break;
        case SNE_XY:
            printf("SNE V%u, V%u - SNE_XY\n", Vx, Vy);
            break;
        case LD_I:
            printf("LD I, %0*X - LD_I\n", 4, (opcode & 0x0FFF));
            break;
        case JP_V0:
            printf("JP V0, %0*X - JP_V0\n", 4, (opcode & 0x0FFF));
            break;
        case RND_X:
            printf("RND V%u, %0*X - RND_X\n", Vx, 4, (opcode & 0x00FF));
            break;
        case DRW_XY:
            printf("DRW V%u, V%u, %u - DRW_XY\n", Vx, Vy, (opcode & 0x000F));
            break;
        case SKP_X:
            printf("SKP V%u - SKP_X\n", Vx);
            break;
        case SKNP_X:
            printf("SKNP V%u - SKNP_X\n", Vx);
            break;
        case LD_XDT:
            printf("LD V%u, DT - LD_XDT\n", Vx);
            break;
        case LD_XK:
            printf("LD V%u, K - LD_XK\n", Vx);
            break;
        case LD_DTX:
            printf("LD DT, V%u - LD_DTX\n", Vx);
            break;
        case LD_STX:
            printf("LD ST, V%u - LD_STX\n", Vx);
            break;
        case ADD_IX:
            printf("ADD I, V%u - ADD_IX\n", Vx);
            break;
        case LD_FX:
            printf("LD F, V%u - LD_FX\n", Vx);
            break;
        case LD_BX:
            printf("LD B, V%u - LD_BX\n", Vx);
            break;
        case LD_IV:
            printf("LD [I], V%u - LD_IV\n", Vx);
            break;
        case LD_VI:
            printf("LD V%u, [I] - LD_VI\n", Vx);
            break;
        default:
            printf("Failed to decode Opcode: %0*X\n", 4, opcode);
            break;
    }
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
        printf("V[%X] = %u\tS[%X] = %u\tKey[%X] = %u\n", i, V[i], i, stack[i], i, key[i]);
    }
    printf("\n");
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
            quit = true;
        }

        is.read((char*)(&memory[INITIAL_PC]), length);

        if (is)
        {
            std::cout << "File was read successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: only " << is.gcount() << " bytes could be read." << std::endl;
            quit = true;
        }

        is.close();
    }
    else
    {
        std::cerr << "Can't find file " << file_path << std::endl;
        std::exit(EXIT_FAILURE);
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
                quit = true;
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

    if (input.wasKeyPressed(SDL_SCANCODE_BACKSPACE))
    {
        quit = true;
    }

    if (input.wasKeyPressed(SDL_SCANCODE_RETURN))
    {
        init_memory(ROM.c_str());
        init_registers();
        clear_gfx();
    }
}
