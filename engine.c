#define CHESSBIT_IMPLEMENTATION
#include "chess_bit.h"

int main(){
    cb_init();
    cb_board b;
    cb_rep_struct rep;
	cb_init_board(&b, &rep);

    long start = cb_get_real_time_ms();
    cb_perft_divide(&b, 6,1);
    long end = cb_get_real_time_ms();

    printf("\nTIME : %ld ms\n", end - start);
    return 0;
}