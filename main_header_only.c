#include "chess_bit.h"
#include <stdio.h>

int main(){
    cb_init();
    cb_board b;
    cb_rep_struct rep;
    cb_init_board(&b, &rep); //1

    long start = cb_get_time_ms();
    
    
    cb_perft_divide(&b, 6);
    long end = cb_get_time_ms();
    cb_print_board(&b);

    printf("time %ld\n", end - start);
    return 0;
}