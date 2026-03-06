#include "header.h"


int main(){
    init();
    board b;
    rep_struct rep;
    unmake_info info;
    init_board(&b, &rep); //1
    printf("nb_threads %d\n", omp_get_max_threads());

    long start = get_real_time_ms();
    perft_divide(&b, 5, 1);
 
    
    long end = get_real_time_ms();
    print_board(&b);

    printf("time %ld\n", end - start);

    return 0;
}

