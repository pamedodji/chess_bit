#include "../header.h"

void king_moves(const board *b, list_move *l){
    bitboard sq_idx = __builtin_ctzll(b -> pieces[KING + b -> turn * 6]);
    u64 occupied = b -> player_pieces[WHITE] | b -> player_pieces[BLACK];
    u64 opp_piece = b -> player_pieces[b -> turn ^ 1];
    bitboard to_add = kings_table[sq_idx] & ~occupied;
    int trailling_zeros;
    while (to_add){
        trailling_zeros = __builtin_ctzll(to_add);
        l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, NO_PROM); 
        (l -> index)++;
        to_add &= (to_add - 1);
    }
    to_add = kings_table[sq_idx] & opp_piece;
    int ind;
    while (to_add){
        trailling_zeros = __builtin_ctzll(to_add);
        ind = 6 * (b -> turn == BLACK);

        while ((b -> pieces[ind] & (1ULL << trailling_zeros)) == 0)
            ind++;
        ind -= 6 * (b -> turn == BLACK);
        switch (ind){
            case ROOK:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, CAPTURES_ROOK);
                break;
            case BISHOP:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, CAPTURES_BISHOP);
                break;
            case QUEEN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, CAPTURES_QUEEN);
                break;
            case PAWN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, CAPTURES_PAWN);
                break;
            case KNIGHT:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, KING, CAPTURES_KNIGHT);
                break;
        }
        (l -> index)++;
        to_add &= (to_add - 1);
    }
    if (b -> turn == WHITE && (b -> castles & 1)){ //white can castle
        if ((~occupied & 32) && (~occupied & 64)){
            l -> m[l -> index] = create_move(sq_idx, 6, KING, CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == WHITE && (b -> castles & 2)){ //white can long castle
        if ((~occupied & 8) && (~occupied & 4) && (~occupied & 2)){
            l -> m[l -> index] = create_move(sq_idx, 2, KING, LONG_CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 4)){ //black can castle
        if ((~occupied & (1ULL << 61)) && (~occupied & (1ULL << 62))){
            l -> m[l -> index] = create_move(sq_idx, 62, KING, CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 8)){ //black can long castle
        if ((~occupied & (1ULL << 59)) && (~occupied & (1ULL << 58)) && (~occupied & (1ULL << 57))){
            l -> m[l -> index] = create_move(sq_idx, 58, KING, LONG_CASTLE); 
            (l -> index)++;
        }
    }
}