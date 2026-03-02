#include "../header.h"

void knight_moves(const board *b, u32 sq_idx, list_move *l){
    bitboard all_moves = knight_table[sq_idx] & ~(b -> player_pieces[b -> turn]);
    int trailling_zeros;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = create_move(sq_idx,trailling_zeros, KNIGHT, NO_PROM); 
        (l -> index)++;
        all_moves &= (all_moves -1);
    }
}

