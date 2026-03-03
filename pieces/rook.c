#include "../header.h"



void rook_moves(const board *b, u32 sq_idx, list_move *l, uint32_t piece){
    bitboard occupied = b -> player_pieces[BLACK] | b -> player_pieces[WHITE];
    bitboard my_piece = b -> player_pieces[b -> turn];
    bitboard north;
    bitboard south;
    bitboard east;
    bitboard west;
    bitboard all_moves;
    int trailling_zeros;
    //up
    north = rrays[sq_idx].north;
    if (north & occupied)
        north = _bzhi_u64(north, __builtin_ctzll(north & occupied) + 1);
    //right
    east = rrays[sq_idx].east;
    if (east & occupied)
        east = _bzhi_u64(east, __builtin_ctzll(east & occupied) + 1);
    //down
    south = rrays[sq_idx].south;
    if (south & occupied)
        south = bzlo_u64(south, 63 -__builtin_clzll((south & occupied) ));
    //west
    west = rrays[sq_idx].west;
    if (west & occupied)
        west = bzlo_u64(west, 63 - __builtin_clzll((west & occupied) ));
    all_moves = north | east | south | west;
    all_moves &= ~my_piece;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, NO_PROM);
        (l -> index)++;
        all_moves &= (all_moves - 1);
    }
}

