#include "../header.h"

void queen_moves(const board *b, bitboard square, list_move *l){
    rook_moves(b, square, l, QUEEN);
    bishop_moves(b, square, l, QUEEN);
}




