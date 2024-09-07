#include "utils.h"

void print_raw(unsigned char *location,unsigned short size) {
    for (unsigned short i = 0; i < size; i++)
    {
        
        if(i % 16 == 0) {
            printf("%04X ",i);
        }
        if(i % 4 == 0) {
            printf(" ");
        }
        printf("%02X",location[i] & 0xFF);
        if((i + 1) % 16 == 0){
            printf("\n");
        }
    }
}