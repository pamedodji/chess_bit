#include "../header.h"

void king_moves(const board *b, list_move *l){
    bitboard square_idx = __builtin_ctzll(b -> pieces[KING + (b -> turn ^ 1) * 6]);
    bitboard to_add = kings_table[square_idx] & ~b -> player_pieces[b -> turn];
    int trailling_zeros;
    while (to_add){
        trailling_zeros = __builtin_ctzll(to_add);
        l -> m[l -> index] = create_move(square_idx, trailling_zeros, KING, NO_PROM); 
        (l -> index)++;
        to_add &= (to_add - 1);
    }
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    if (b -> turn == WHITE && (b -> castles & 1)){ //white can castle
        if ((~occupied & 32) && (~occupied & 64)){
            l -> m[l -> index] = create_move(square_idx, 6, KING, CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == WHITE && (b -> castles & 2)){ //white can long castle
        if ((~occupied & 8) && (~occupied & 4) && (~occupied & 2)){
            l -> m[l -> index] = create_move(square_idx, 2, KING, LONG_CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 4)){ //black can castle
        if ((~occupied & (1ULL << 61)) && (~occupied & (1ULL << 62))){
            l -> m[l -> index] = create_move(square_idx, 62, KING, CASTLE); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 8)){ //black can long castle
        if ((~occupied & (1ULL << 59)) && (~occupied & (1ULL << 58)) && (~occupied & (1ULL << 57))){
            l -> m[l -> index] = create_move(square_idx, 58, KING, LONG_CASTLE); 
            (l -> index)++;
        }
    }
}