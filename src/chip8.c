#include "chip8.h"
#include "memory.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

const char chip8_default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80};

void chip8_init(struct chip8 *chip8)
{
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
}

void chip8_load(struct chip8 *chip8, const char *buf, size_t size)
{
    assert(size + CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);

    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registers.program_counter = CHIP8_PROGRAM_LOAD_ADDRESS;
}


void chip8_exec(struct chip8 *chip8, unsigned short opcode)
{
    switch (opcode)
    {

    //CLS
    case 0x00E0:
            chip8_screen_clear(&chip8->screen);
        break;

    //RET
    case 0x00EE:
        chip8->registers.program_counter = chip8_stack_pop(chip8);
        break;

    
    default:

        chip8_exec_extended(chip8,opcode);

        break;
    }
}


static void chip8_exec_extended_eight(struct chip8 *chip8, unsigned short opcode) {

    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char N = opcode & 0x000f;
    switch (N)
    {
        // 8xy0 - LD Vx, Vy
        case 0x00:
            chip8->registers.V[x] = chip8->registers.V[y]; 
            break;

        // 8xy1 - OR Vx, Vy
        case 0x01:
            chip8->registers.V[x] = chip8->registers.V[x] | chip8->registers.V[y];
            break;

        // 8xy2 - AND Vx, Vy
        case 0x02:
            chip8->registers.V[x] = chip8->registers.V[x] & chip8->registers.V[y];
            break;

        // 8xy3 - XOR Vx, Vy
        case 0x03:
            chip8->registers.V[x] = chip8->registers.V[x] ^ chip8->registers.V[y];
            break;

        // 8xy4 - ADD Vx, Vy
        case 0x04:
            if((chip8->registers.V[x] + chip8->registers.V[y]) > 255) {
                chip8->registers.V[0xF] = 1;
            }
            chip8->registers.V[x] = (chip8->registers.V[x] + chip8->registers.V[y]) % 0x100;
            break;

        // 8xy5 - SUB Vx, Vy
        case 0x05:
            chip8->registers.V[0xF] = 0;
            if(chip8->registers.V[x] > chip8->registers.V[y]) {
                chip8->registers.V[0xF] = 1;
            }
            chip8->registers.V[x] = chip8->registers.V[x] - chip8->registers.V[y];
            break;

        //8xy6 - SHR Vx {, Vy}
        case 0x06:
            chip8->registers.V[0xF] = 0;
            if(chip8->registers.V[x] & 0x01) {
                chip8->registers.V[0xF] = 1;
            }
            chip8->registers.V[x] = chip8->registers.V[x] >> 1;
            break;
    
        // 8xy7 - SUBN Vx, Vy
        case 0x07:
            chip8->registers.V[0xF] = 0;
            if(chip8->registers.V[y] > chip8->registers.V[x]) {
                chip8->registers.V[0xF] = 1;
            }
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
            break;

        // 8xyE - SHL Vx {, Vy}
        case 0x0E:
            chip8->registers.V[0xF] = 0;
            if(chip8->registers.V[x] & 0x01) {
                chip8->registers.V[0xF] = 1;
            }
            chip8->registers.V[x] = chip8->registers.V[x] << 1;
            break;

        default:
            break;
    }

}

static void chip8_exec_extended_e(struct chip8 *chip8, unsigned short opcode) {

    unsigned char sub_opcode = opcode & 0x00FF;
    unsigned char x = (opcode >> 8) & 0x000f;
    switch (sub_opcode)
    {
    // Ex9E - SKP Vx
    case 0x9E:
        if(chip8_keyboard_is_down(&chip8->keyboard,x)) {
            chip8->registers.program_counter += 2;
        }
    break;

    // ExA1 - SKNP Vx
    case 0xA1:
        if(!chip8_keyboard_is_down(&chip8->keyboard,x)) {
            chip8->registers.program_counter += 2;
        }
    break;
    
    default:
        break;
    }

}

static void chip8_exec_extended_f(struct chip8 *chip8, unsigned short opcode) {

    unsigned char sub_opcode = opcode & 0x00FF;
    unsigned char x = (opcode >> 8) & 0x000f;

    switch (sub_opcode)
    {
    // Fx07 - LD Vx, DT
    case 0x07:
        chip8->registers.V[x] = chip8->registers.delay_timer;
    break;

    // Fx0A - LD Vx, K
    case 0x0A:
        for (char i = CHIP8_TOTAL_KEYS; i < CHIP8_TOTAL_KEYS ; i++)
        {
            if(chip8_keyboard_is_down(&chip8->keyboard,i)) {
                chip8->registers.V[x] = keyboard_map[i];
                break;
            }

            if(i == CHIP8_TOTAL_KEYS - 1) {
                i = 0;
            }
        }
    break;

    // Fx15 - LD DT, Vx
    case 0x15:
        chip8->registers.delay_timer = chip8->registers.V[x];
    break;

    // Fx18 - LD ST, Vx
    case 0x18:
        chip8->registers.sound_timer = chip8->registers.V[x];
    break;

    // Fx1E - ADD I, Vx
    case 0x1E:
        chip8->registers.I += chip8->registers.V[x];
    break;

    // Fx29 - LD F, Vx
    case 0x29:
        chip8->registers.I = (chip8->registers.V[x]) * 5;
    break;

    // Fx33 - LD B, Vx
    case 0x33:
    chip8_memory_set(&chip8->memory, chip8->registers.I, chip8->registers.V[x] / 100);
    chip8_memory_set(&chip8->memory, chip8->registers.I + 1, (chip8->registers.V[x] / 10) % 10);
    chip8_memory_set(&chip8->memory, chip8->registers.I + 2, chip8->registers.V[x] % 10);
    break;

    // Fx55 - LD B, Vx
    case 0x55:
        for (char i = 0; i <= x; i++)
        {
            chip8_memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
        }
    break;

    // Fx65 - LD Vx, [I]
    case 0x65:
        for (char i = 0; i <= x; i++)
        {
            chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
        }
    break;
    
    default:
        break;
    }
}

static void chip8_exec_extended(struct chip8 *chip8, unsigned short opcode) {

    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned char rnd;

    switch (opcode & 0xF000)
    {

    // JP Addr: Jump to nnn
    case 0x1000:
        chip8->registers.program_counter = nnn;
        break;

    // CALL Addr: Call subroutine at nnn
    case 0x2000:
        chip8_stack_push(chip8, chip8->registers.program_counter);
        chip8->registers.program_counter = nnn;
        break;

    // SE Vx, byte - 3xkk Skip next instruction if Vx == kk
    case 0x3000:
        if(chip8->registers.V[x] == kk) {
            chip8->registers.program_counter += 2;
        }
        break;

    // SNE Vx, byte - Skip next instruction if Vx != kk.
    case 0x4000:
        if(chip8->registers.V[x] != kk) {
            chip8->registers.program_counter += 2;
        }
        break;

    // SE Vx, Vy - Skip next instruction if Vx == Vy.
    case 0x5000:
        if(chip8->registers.V[x] == chip8->registers.V[y]) {
            chip8->registers.program_counter += 2;
        }
        break;

    // LD Vx, byte - Set Vx = kk.
    case 0x6000:
        chip8->registers.V[x] = kk;
        break;

    // ADD Vx, byte - Set Vx = Vx + kk.
    case 0x7000:
        chip8->registers.V[x] += kk;
        break;
        
    // 8xyN operations
    case 0x8000:
    chip8_exec_extended_eight(chip8, opcode);
    break;

    // 9xyN - SNE Vx, Vy
    case 0x9000:
        if(chip8->registers.V[x] != chip8->registers.V[y]) {
            chip8->registers.program_counter += 2;
        }
    break;

    // Annn - LD I, addr
    case 0xA000:
            chip8->registers.I = nnn;
    break;

    // Bnnn - JP V0, addr
    case 0xB000:
            chip8->registers.program_counter += nnn + chip8->registers.V[0];
    break;

    // Cxkk - RND Vx, byte
    case 0xC000:
            rnd = rand() % 0xFF;
            chip8->registers.V[x] = kk & rnd;
    break;

    // Dxyn - DRW Vx, Vy, nibble
    case 0xD000:

            chip8->registers.V[0xF] = chip8_screen_draw_sprite(
                &chip8->screen,
                chip8->registers.V[x],
                chip8->registers.V[y],
                &chip8->memory.memory[chip8->registers.I],
                nnn & 0x000f);
    break;

    case 0xE000:
        chip8_exec_extended_e(chip8,opcode);
    break;
    
    case 0xF000:
        chip8_exec_extended_f(chip8,opcode);
    break;

    default:
        break;
    }
}