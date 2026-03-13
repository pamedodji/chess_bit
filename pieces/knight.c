#include "../header.h"

void knight_moves(const board *b, u32 sq_idx, list_move *l){
    u64 opp_piece = b -> player_pieces[b -> turn ^ 1];
    u64 occupied = b -> player_pieces[WHITE] | b -> player_pieces[BLACK];
    bitboard all_moves = knight_table[sq_idx] & ~occupied;
    u64 cpy_all_moves = knight_table[sq_idx] & opp_piece;
    int trailling_zeros;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = create_move(sq_idx,trailling_zeros, KNIGHT, NO_PROM); 
        (l -> index)++;
        all_moves &= (all_moves -1);
    }
    all_moves = cpy_all_moves;
    int ind;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        ind = 6 * (b -> turn == BLACK);

        while ((b -> pieces[ind] & (1ULL << trailling_zeros)) == 0)
            ind++;
        ind -= 6 * (b -> turn == BLACK);
        switch (ind){
            case ROOK:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KNIGHT, CAPTURES_ROOK);
                break;
            case BISHOP:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KNIGHT, CAPTURES_BISHOP);
                break;
            case QUEEN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KNIGHT, CAPTURES_QUEEN);
                break;
            case PAWN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KNIGHT, CAPTURES_PAWN);
                break;
            case KNIGHT:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KNIGHT, CAPTURES_KNIGHT);
                break;
        }
        (l -> index)++;
        all_moves &= (all_moves -1);
    }
}

