#include "../header.h"

void pawn_all_moves(const board *b, list_move *l){
    bitboard not_occupied = ~(b -> player_pieces[0] | b -> player_pieces[1]);
    bitboard no_prom_pawns = (b -> pieces[PAWN + 6 * (b -> turn ^ 1)]) & ~ROWS[6 - 5 * (b -> turn ^ 1)]; //can't have pawns on last row
    bitboard prom_pawns = (b -> pieces[PAWN + 6 *(b -> turn ^ 1)]) & ROWS[6 - 5 * (b -> turn ^ 1)];
    bitboard opp = b -> player_pieces[b -> turn ^ 1];
    bitboard all_moves = 0;
    int trailling_zeros;

    if (b -> turn == WHITE){
        //forward
        all_moves = ((no_prom_pawns) << 8) & not_occupied; 
        bitboard double_forward = ((all_moves & ROWS[2]) << 8) & not_occupied;
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros - 8, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //Double forward
        while (double_forward){
            trailling_zeros = pop_inplace(&double_forward);
            l -> m[l -> index] = create_move(trailling_zeros - 16, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //Captures 
        all_moves = ((no_prom_pawns & ~COLUMNS[7]) << 9) & opp; //right
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros - 9, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        all_moves = ((no_prom_pawns & ~COLUMNS[0]) << 7) & opp; //left
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros - 7, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //promotion
        if (prom_pawns){ //rare so we can skip this instructions very often
            all_moves = (prom_pawns << 8) & not_occupied;
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros - 8, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 8, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 8, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 8, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
            all_moves = ((prom_pawns & ~COLUMNS[7]) << 9) & opp; //right captures
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros - 9, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 9, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 9, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 9, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
            all_moves = ((prom_pawns & ~COLUMNS[0]) << 7) & opp; //left captures
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros - 7, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 7, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 7, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros - 7, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
        }
        //en_passant
        if (b -> b_en_passant_flag != -1){
            
            int square_idx = 32 + b -> b_en_passant_flag; 
            bitboard square = 1ULL << square_idx;
            if (no_prom_pawns & ROWS[4] & (square << 1)){
                l -> m[l -> index] = create_move(square_idx + 1 , square_idx + 8, PAWN, EN_PASSANT);
                (l -> index)++;
            }
            if (no_prom_pawns & ROWS[4] & (square >> 1)){
                l -> m[l -> index] = create_move(square_idx - 1 , square_idx + 8, PAWN, EN_PASSANT);
                (l -> index)++;
            }
        }
    }
    else{
        //forward
        all_moves = (no_prom_pawns >> 8) & not_occupied; 
        bitboard double_forward = ((all_moves & ROWS[5]) >> 8) & not_occupied;
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros + 8, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //Double forward
        while (double_forward){
            trailling_zeros = pop_inplace(&double_forward);
            l -> m[l -> index] = create_move(trailling_zeros + 16, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //Captures 
        all_moves = ((no_prom_pawns & ~COLUMNS[0]) >> 9) & opp; //right
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros + 9, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        all_moves = ((no_prom_pawns & ~COLUMNS[7]) >> 7) & opp; //left
        while (all_moves){
            trailling_zeros = pop_inplace(&all_moves);
            l -> m[l -> index] = create_move(trailling_zeros + 7, trailling_zeros, PAWN, NO_PROM);
            (l -> index)++;
        }
        //promotion
        if (prom_pawns){ //rare so we can skip this instructions very often
            all_moves = (prom_pawns >> 8) & not_occupied;
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros + 8, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 8, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 8, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 8, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
            all_moves = ((prom_pawns & ~COLUMNS[0]) >> 9) & opp; //right captures
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros + 9, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 9, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 9, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 9, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
            all_moves = ((prom_pawns & ~COLUMNS[7]) >> 7) & opp; //left captures
            while (all_moves){
                trailling_zeros = pop_inplace(&all_moves);
                l -> m[l -> index] = create_move(trailling_zeros + 7, trailling_zeros, PAWN, QUEEN);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 7, trailling_zeros, PAWN, ROOK);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 7, trailling_zeros, PAWN, BISHOP);
                (l -> index)++;
                l -> m[l -> index] = create_move(trailling_zeros + 7, trailling_zeros, PAWN, KNIGHT);
                (l -> index)++;
            }
        }
        //en_passant
        if (b -> w_en_passant_flag != -1){
            int square_idx = 24 + b -> w_en_passant_flag; 
            bitboard square = 1ULL << square_idx;
            if (no_prom_pawns & ROWS[3] & (square << 1)){
                l -> m[l -> index] = create_move(square_idx + 1 , square_idx - 8, PAWN, EN_PASSANT);
                (l -> index)++;
            }
            if (no_prom_pawns & ROWS[3] & (square >> 1)){
                l -> m[l -> index] = create_move(square_idx - 1 , square_idx - 8, PAWN, EN_PASSANT);
                (l -> index)++;
            }
        }
    }
}