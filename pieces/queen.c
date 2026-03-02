#include "../header.h"

void queen_moves(const board *b, u32 sq_idx, list_move *l){
    rook_moves(b, sq_idx, l, QUEEN);
    bishop_moves(b, sq_idx, l, QUEEN);
}




