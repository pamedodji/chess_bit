
#include "header.h"

bitboard pos_table[12][64];
bitboard k, q, K, Q; //k for white king 
bitboard en_passant_table[8];
bitboard turn_table;

void init_zobrist_tables(){
    bitboard s = Zobrist_SEED;
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 64; j++)
            pos_table[i][j] = splitmix64(&s);
    }
    k = splitmix64(&s);
    q = splitmix64(&s);
    K = splitmix64(&s);
    Q = splitmix64(&s);
    for (int i = 0; i < 8; i++)
        en_passant_table[i] = splitmix64(&s);
    turn_table = splitmix64(&s);
}

bitboard zobrist_key(const board *b){
    bitboard hash = 0;
    int trailling_zeros;
    bitboard copy_board;
    for (int piece = 0; piece < 12; piece++){
        copy_board = b -> pieces[piece];
        while (copy_board > 0){
            trailling_zeros = __builtin_ctzll(copy_board);
            hash ^= pos_table[piece][trailling_zeros];
            
            copy_board &= ~(1ULL << trailling_zeros);
        }
            
    }
    if (b -> castles & 1)
        hash ^= k;
    if (b -> castles & 2) 
        hash ^= q;
    if (b -> castles & 4)
        hash ^= K;
    if (b -> castles & 8)
        hash ^= Q;
    if (b -> w_en_passant_flag >= 0)
        hash ^= en_passant_table[b -> w_en_passant_flag];
    if (b -> b_en_passant_flag >= 0)
        hash ^= en_passant_table[b -> b_en_passant_flag];
    if (b -> turn == WHITE)
        hash ^= turn_table;

    return hash;
}
