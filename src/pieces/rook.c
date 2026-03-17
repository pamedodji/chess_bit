#include "../header.h"



void rook_moves(const board *b, u32 sq_idx, list_move *l, uint32_t piece){
    bitboard occupied = b -> player_pieces[BLACK] | b -> player_pieces[WHITE];
    bitboard opp_piece = b -> player_pieces[b -> turn ^ 1];
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
    bitboard cpy_all_moves = all_moves & ~occupied;
    while (cpy_all_moves){
        trailling_zeros = __builtin_ctzll(cpy_all_moves);
        l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, NO_PROM);
        (l -> index)++;
        cpy_all_moves &= (cpy_all_moves - 1);
    }
    cpy_all_moves = all_moves & opp_piece;
    int ind;
    while (cpy_all_moves){
        trailling_zeros = __builtin_ctzll(cpy_all_moves);
        ind = 6 * (b -> turn == BLACK);

        while ((b -> pieces[ind] & (1ULL << trailling_zeros)) == 0)
            ind++;
        ind -= 6 * (b -> turn == BLACK);
        switch (ind){
            case ROOK:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, CAPTURES_ROOK);
                break;
            case BISHOP:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, CAPTURES_BISHOP);
                break;
            case QUEEN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, CAPTURES_QUEEN);
                break;
            case PAWN:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, CAPTURES_PAWN);
                break;
            case KNIGHT:
                l -> m[l -> index] = create_move(sq_idx, trailling_zeros, piece, CAPTURES_KNIGHT);
                break;
        }
        (l -> index)++;
        cpy_all_moves &= (cpy_all_moves - 1);
    }

}

