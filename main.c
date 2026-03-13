#include "header.h"


int main(){
    init();
    board b;
    rep_struct rep;
    unmake_info info;
    
    init_board(&b, &rep);
    long start = get_real_time_ms();
    perft_divide(&b, 6, 1);
    long end = get_real_time_ms();
    printf("TIME : %ld ms\n", end - start);
    return 0;
}

