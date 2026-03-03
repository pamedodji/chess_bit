#include "../header.h"





void bishop_moves(const board *b, u32 sq_idx, list_move *l, uint32_t piece){
    bitboard occupied = b -> player_pieces[BLACK] | b -> player_pieces[WHITE];
    bitboard my_piece = b -> player_pieces[b -> turn];
    bitboard n_east;
    bitboard s_east;
    bitboard n_west;
    bitboard s_west;
    bitboard all_moves;
    int trailling_zeros;
    //up right
    n_east = brays[sq_idx].n_east;
    if (n_east & occupied)
        n_east = _bzhi_u64(n_east, __builtin_ctzll(n_east & occupied) + 1);
    //up left
    n_west = brays[sq_idx].n_west;
    if (n_west & occupied)
        n_west = _bzhi_u64(n_west, __builtin_ctzll(n_west & occupied) + 1);
    //down right
    s_east = brays[sq_idx].s_east;
    if (s_east & occupied)
        s_east = bzlo_u64(s_east, 63 -__builtin_clzll((s_east & occupied) ));
    //down left
    s_west = brays[sq_idx].s_west;
    if (s_west & occupied)
        s_west = bzlo_u64(s_west, 63 -__builtin_clzll((s_west & occupied) ));
    all_moves = n_west | n_east | s_east | s_west;
    all_moves &= ~my_piece;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, NO_PROM);
        (l -> index)++;
        all_moves &= (all_moves - 1);
    }
}
