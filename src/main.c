#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "chip8_keyboard.h"
#include <unistd.h>
#include "utils.h"
#include <fcntl.h>

const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_1, SDLK_2, SDLK_3, SDLK_4,
    SDLK_q, SDLK_w, SDLK_e, SDLK_r,
    SDLK_a, SDLK_s, SDLK_d, SDLK_f,
    SDLK_z, SDLK_x, SDLK_c, SDLK_v};

int main(int argc, char **argv)
{
    if(argc < 2) {
        printf("Usage: ./program <rom_file> <debug_mode_on_start>\n\
        \n\nrom_file is a valid path for file to run \ndebug_mode_on_start is a bool variable, supply 1 to start in debug mode(optional)\n");
        return -1;
    }

    const char *filename = argv[1];
    printf("Loaded file: %s\n",filename);

    FILE *f = fopen(filename, "rb");
    if(!f) {
        printf("Gimme correct file\n");
        return -1;
    }
    
    
    fseek(f,0, SEEK_END);
    long size = ftell(f);
    fseek(f,0,SEEK_SET);
    char buf[size];
    fread(buf,size,1,f);

    struct chip8 chip8;
    chip8_init(&chip8);
    chip8_load(&chip8, buf, size);

    SDL_Init(SDL_INIT_EVERYTHING);


    SDL_Window *window = SDL_CreateWindow(EMULATOR_WINDOW_TITLE,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          CHIP8_WIDTH * WINDOW_MULTIPLIER, CHIP8_HEIGHT * WINDOW_MULTIPLIER,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    srand(time(NULL));
    char running = 1,stopped_text = 1;
    char debug_mode = 0,sub_menu = 0;
    void *debug_key_buff[2] = {0,0};
    char debug_key;
    if(argc == 3) {
        printf("DEBUG MODE!\n");
        printf("Enter 'i' for usage info.\n");
        fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
        running = 0;
        debug_mode = 1;
    }

    // Main logic loop
    while (1)
    {

        // Main debug loop
        if(debug_mode) {
            running = 0;
            int ret = 0;
            ret = read(0,debug_key_buff,1);
            debug_key = (char)debug_key_buff[0];
                
            switch (debug_key)
            {
            case 'i': 
                printf("p -> Resume Program\n");
                printf("n -> Run Next Instruction\n");
                printf("s -> Show Stack\n");
                printf("m -> Show Memory\n");
                printf("r -> Show Registers\n");
                printf("v -> Show Video Memory\n");
                break;
            case 'p':
                printf("p pressed\n");
                break;

            case 'n':
                running = 1;
                break;

            case 's':
                print_raw(&chip8.stack.stack,CHIP8_TOTAL_STACK_DEPTH);
                break;

            case 'm':
                print_raw(&chip8.memory.memory,CHIP8_MEMORY_SIZE);
                break;

            case 'r':
                printf("Current instruction: %X\n",chip8_memory_get_short(&chip8.memory, chip8.registers.program_counter));
                printf("Delay Timer: %X\n",chip8.registers.delay_timer);
                printf("I: %X\n",chip8.registers.I);
                printf("Program Counter: %X\n",chip8.registers.program_counter);
                printf("Sound Timer: %X\n",chip8.registers.sound_timer);
                printf("Stack Pointer: %X\n",chip8.registers.stack_pointer);
                for (int i = 0; i < CHIP8_TOTAL_DATA_REGISTERS; i++)
                {
                    printf("V-%X : %03X ",i,chip8.registers.V[i]);
                    if((i + 1) % 4 == 0) {
                        printf("\n");
                    }
                }
                break;

            case 'v':
                for (unsigned short i = 0; i < CHIP8_HEIGHT; i++)
                {
                    for (unsigned short j = 0; j < CHIP8_WIDTH; j++)
                    {
                        if(chip8_screen_is_set(&chip8.screen,j,i)) {
                            printf("1");
                        } else {
                            printf("0");
                        }
                    }
                    printf("\n");
                }
                
                break;

            default:
                break;
            }
            
        }
        // Main SDL loop
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {

            switch (e.type)
            {
            case SDL_QUIT:
                goto out;
                break;

            case SDL_KEYDOWN:
            {
                char key = e.key.keysym.sym;
                int v_key = chip8_keyboard_map(keyboard_map, key);
                if (v_key != -1)
                {
                    chip8_keyboard_down(&chip8.keyboard, v_key);
                } else if(key == 'n') {
                    running = !running;
                }
            }
            break;

            case SDL_KEYUP:
            {
                char key = e.key.keysym.sym;
                int v_key = chip8_keyboard_map(keyboard_map, key);
                if (v_key != -1)
                {
                    chip8_keyboard_up(&chip8.keyboard, v_key);
                }
            }
            break;

            default:
                break;
            }

        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        for (int x = 0; x < CHIP8_WIDTH; x++)
        {
            for (int y = 0; y < CHIP8_HEIGHT; y++)
            {
                if (chip8_screen_is_set(&chip8.screen, x, y))
                {
                    SDL_Rect r;
                    r.x = x * WINDOW_MULTIPLIER;
                    r.y = y * WINDOW_MULTIPLIER;
                    r.w = WINDOW_MULTIPLIER;
                    r.h = WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        

        SDL_RenderPresent(renderer);

        if(running) {
            
            if (chip8.registers.delay_timer > 0)
            {
                usleep(100000);
                chip8.registers.delay_timer -= 1;
            }
            if (chip8.registers.sound_timer > 0)
            {
                chip8.registers.sound_timer -= 1;
            }
            unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.program_counter);
            chip8.registers.program_counter += 2;
            chip8_exec(&chip8,opcode);
            stopped_text = 1;
        } else if(stopped_text) {
            printf("INFO: Program stopped. Press 'p' to continue\n");
            stopped_text = 0;
        }

    }
out:
    SDL_DestroyWindow(window);
    return 0;
}
