#include "header.h"


int main(){
    init();
    board b;
    rep_struct rep;
    unmake_info info;
    init_board(&b, &rep); //1

    long start = get_time_ms();

    
    perft_divide(&b, 6);
    verify_logics(5);
    long end = get_time_ms();
    print_board(&b);

    printf("time %ld\n", end - start);

    return 0;
}

