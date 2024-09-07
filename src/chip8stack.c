#include "chip8_stack.h"
#include "chip8.h"
#include <assert.h>
#include <stdio.h>

static void chip8_stack_in_bounds(struct chip8 *chip8) {
    assert(chip8->registers.stack_pointer < CHIP8_TOTAL_STACK_DEPTH);
}

void chip8_stack_push(struct chip8 *chip8, unsigned short value) {
    chip8_stack_in_bounds(chip8);
    chip8->stack.stack[chip8->registers.stack_pointer] = value;
    chip8->registers.stack_pointer++;

}

unsigned short chip8_stack_pop(struct chip8 *chip8) {
    chip8_stack_in_bounds(chip8);

    unsigned short res = chip8->stack.stack[--chip8->registers.stack_pointer];
    return res;
}