#include "header.h"


int main(){
    init();
    board b;
    rep_struct rep;
    init_board(&b, &rep); //1
    int r;
    long start = get_time_ms();
    perft_divide(&b, 6);
    long end = get_time_ms();
    print_board(&b);

    printf("time %ld\n", end - start);
    return 0;
}

