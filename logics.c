#include "header.h"

bitboard knight_table[64];
bitboard kings_table[64];
rooksray rrays[64];
bishopray brays[64];

void init(){
    init_zobrist_tables();
    memset(rrays, 0, 64 * sizeof(rooksray));
    bitboard square;
    int count;
    for (int i = 0; i < 64; i++){
        square = 1ULL << i;
        count = 1;
        int row = i/8;
        int col = i % 8;
        rrays[i].east = 0;
        rrays[i].west = 0;
        rrays[i].north = 0;
        rrays[i].south = 0;
        
        while (count < 8 && ((square << count) & ROWS[row]) != 0 ){
            rrays[i].east |= square << count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> count) & ROWS[row]) != 0 ){
            rrays[i].west |= square >> count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square << 8 * count) & COLUMNS[col]) != 0 ){
            rrays[i].north |= square << 8 * count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> 8 * count) & COLUMNS[col]) != 0 ){
            rrays[i].south |= square >> 8 * count;
            count++;
        }
    }


    memset(knight_table, 0, 64 * sizeof(bitboard));
    int row;
    int col;
    int row_mov;
    int col_mov;
    for (int i = 0; i < 64; i++){
        square = 1ULL << i; 
        row = i / 8;
        col = i % 8;
        for (int j = 0; j < 8; j++){
            row_mov = n_mov[j].r;
            col_mov = n_mov[j].c;
            if (!on_board(row + row_mov, col + col_mov)) 
                continue;
            if (row_mov < 0)
                knight_table[i] |= square >> (-row_mov * 8 - col_mov);
            else
                knight_table[i] |= square << (row_mov * 8 + col_mov);
            
        }
    }
    memset(brays, 0, 64 * sizeof(bishopray));
    int horizontal;
    int vertical ;
    for (int i = 0; i < 64; i++){
        row = i / 8;
        col = i % 8;
        square = 1ULL << i;
        //UP RIGHT
        horizontal = 1;
        vertical = 1;
        while ((row + vertical < 8) && (col + horizontal < 8)){
            brays[i].n_east |= square << (8 * vertical + horizontal);
            horizontal++;
            vertical++;
            
        }
        //UP LEFT
        horizontal = 1;
        vertical = 1;
        while ((row + vertical) < 8 && (col - horizontal >= 0)){
            brays[i].n_west |= square << (8 * vertical - horizontal);
            horizontal++;
            vertical++;
            
        }
        //DOWN RIGHT
        horizontal = 1;
        vertical = 1;
        while ((row - vertical >= 0) && col + horizontal < 8){
            brays[i].s_east |= square >> (8 * vertical - horizontal);
            horizontal++;
            vertical++;
            
        }
        //DOWN LEFT
        horizontal = 1;
        vertical = 1;
        while ((row - vertical >= 0) && (col - horizontal >= 0)){
            brays[i].s_west |= square >> (8 * vertical + horizontal);
            horizontal++;
            vertical++;
            
        } 
      
    }

    memset(kings_table, 0, 64 * sizeof(bitboard));
    for (int i = 0; i < 64; i++){
        row = i / 8;
        col = i % 8;
        square = 1ULL << i;
        if (row + 1 < 8){
            if (col + 1 < 8)
                kings_table[i] |= square << 9;
            if (col - 1 >= 0)
                kings_table[i] |= square << 7;
            kings_table[i] |= square << 8;
        }
        if (col + 1 < 8)
            kings_table[i] |= square << 1;
        if (col - 1 >= 0)
            kings_table[i] |= square >> 1;
        if (row - 1 >= 0){
            if (col + 1 < 8)
                kings_table[i] |= square >> 7;
            if (col - 1 >= 0)
                kings_table[i] |= square >> 9;
            kings_table[i] |= square >> 8;
        }

    }

    }


int is_attacked_row(const board *b, bitboard square){
    int piece = ROOK + 6 * (b -> turn ^ 1);
    bitboard Q_R_MASK = b -> pieces[piece] | b -> pieces[3 + piece];
    bitboard index;
    bitboard occupied = b -> player_pieces[BLACK] | b -> player_pieces[WHITE];
    u32 sq_idx = __builtin_ctzll(square);
    bitboard blockers = occupied & rrays[sq_idx].east;
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    blockers = occupied & rrays[sq_idx].west;
    if (blockers){
        index = 63 - __builtin_clzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    return 0;
}


int is_attacked_column(const board *b, bitboard square){
    int piece = ROOK + 6 * (b -> turn ^ 1);
    bitboard Q_R_MASK = b -> pieces[piece] | b -> pieces[3 + piece];
    bitboard index;
    bitboard occupied =  b -> player_pieces[0] | b -> player_pieces[1];
    u32 sq_idx = __builtin_ctzll(square);
    bitboard blockers = occupied & rrays[sq_idx].south;
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    blockers = occupied & rrays[sq_idx].north;
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    return 0;
}


int is_attacked_diagonal(const board *b, bitboard square){
    int piece = BISHOP + 6 * (b -> turn ^ 1);
    bitboard Q_B_MASK = b -> pieces[piece] | b -> pieces[ 1 + piece];
    bitboard index;
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    u32 sq_idx = __builtin_ctzll(square);
    bitboard blockers = occupied & brays[sq_idx].n_east;
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & brays[sq_idx].n_west;
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & brays[sq_idx].s_east;
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & brays[sq_idx].s_west;
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    return 0;
}

int is_attacked_by_knight(const board *b, bitboard square){
    bitboard opp = b -> pieces[KNIGHT + 6 * (b -> turn ^ 1)];
    return (opp & knight_table[__builtin_ctzll(square)]) > 0;
}

int is_attacked_pawn(const board *b, bitboard square){
    int row = get_row(square);
    int column = get_column(square);
    if (b -> turn == WHITE)
        return ((on_board(row + 1 ,column + 1) &&  (((square << 9) & b -> pieces[PAWN]) > 0))) || ((on_board(row +1 ,column - 1) &&  (((square << 7) & b -> pieces[PAWN]) > 0)));
    return ((on_board(row - 1 ,column + 1) &&  (((square >> 7) & b -> pieces[PAWN + 6]) > 0))) || ((on_board(row - 1 ,column - 1) &&  (((square >> 9) & b -> pieces[PAWN + 6]) > 0)));
}     

int is_attacked_king(const board *b, bitboard square){
    bitboard opp_king = b -> pieces[KING + 6 * (b -> turn ^ 1)];
    return (opp_king & kings_table[__builtin_ctzll(square)]) > 0;
}

int is_attacked(const board *b, bitboard square){
    return is_attacked_column(b, square) || is_attacked_row(b, square) ||is_attacked_diagonal(b, square) || 
         is_attacked_by_knight(b, square) || is_attacked_pawn(b, square) || is_attacked_king(b, square);
}



int is_check(const board *b){
    bitboard square = b -> pieces[KING + 6 * b -> turn];
    return is_attacked(b,square);
}

void rook_all_moves(const board *b, list_move *l){
    bitboard square;
    int i = 0;
    if (b -> turn == WHITE)
        i += 6; 
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        square = copy_mask & -copy_mask;  
        copy_mask ^= square;                     
        rook_moves(b, __builtin_ctzll(square), l, ROOK);
    } 
}


void bishop_all_moves(const board *b, list_move *l){
    int i = 2;
    if (b -> turn == WHITE)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        bishop_moves(b, trailing_zeros, l, BISHOP);
        copy_mask &= (copy_mask - 1);
    }     
}

void knight_all_moves(const board *b, list_move *l){
    int i = 1;
    if (b -> turn == WHITE)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        knight_moves(b, trailing_zeros, l);
        copy_mask &= (copy_mask - 1);
    }       
}

void queen_all_moves(const board *b, list_move *l){
    int i = 3;
    if (b -> turn == WHITE)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        queen_moves(b, trailing_zeros, l);
        copy_mask &= (copy_mask - 1);
    }     
}

void pseudo_legal_moves(const board *b, list_move *l){
    l -> index = 0;
    rook_all_moves(b, l);
    bishop_all_moves(b, l);
    knight_all_moves(b, l);
    pawn_all_moves(b, l);
    queen_all_moves(b, l);
    king_moves(b, l);    
    l -> size = l -> index;  
    l -> index = 0;
}

void legal_moves(board *b, list_move *l){
    long start;
    pseudo_legal_moves(b, l);
    
    if (l -> size == 0)
        return;
    int new_ind = 0;
    unmake_info info;
    bitboard pinned = get_pinned(b);
    int is_check_value = is_check(b);
    bitboard sq_src;
    bitboard sq_dst;
    u32 piece;
    u32 flag;
    for (int index = 0 ; index < l -> size; index++){
        sq_src = get_m_bitboard(l -> m[index], 0);
        sq_dst = get_m_bitboard(l -> m[index], 1);
        piece = get_m_int(l -> m[index], 2);
        flag = get_m_int(l -> m[index], 3);
        if ((sq_src & pinned) == 0 && is_check_value == 0 && piece != KING && 
            flag != EN_PASSANT){
            l -> m[new_ind] = l -> m[index];
            new_ind++;
        }
        else{
            if (piece == KING){
                if(flag >= CASTLE){
                    if (is_check(b))
                        continue;
                    if ((sq_src << 2) == sq_dst){
                        if (is_attacked(b, sq_src << 1) || is_attacked(b, sq_src << 2))
                            continue;
                        l -> m[new_ind] = l -> m[index];
                        new_ind++;
                        continue;
                    }
                    else {
                        if (is_attacked(b, sq_src >> 1) || is_attacked(b, sq_src >> 2))
                            continue;
                        l -> m[new_ind] = l -> m[index];
                        new_ind++;
                        continue;
                    }
                }
            }
            make_move(b, l -> m[index], 0, &info);
    
            b -> turn ^= 1;

            if (!is_check(b)){
                l -> m[new_ind] = l -> m[index];
                new_ind++;
            }
            b -> turn ^=1;
            unmake(b, &info);
        }
    }
    l -> size = new_ind;
}

int is_legal_move(const board *b, move m){
    board temp = *b;
    bitboard src;
    bitboard dst1;
    bitboard dst2;
    if (get_m_int(m, 3) == KING && (((src = get_m_bitboard(m, 0)) == (dst1 = get_m_bitboard(m, 1) >> 2)) || (src == (dst2 = get_m_bitboard(m, 1) << 2)))){
        if (is_check(b))
            return 0;
        if ((src << 2) == dst1){
            if (is_attacked(b, (b -> pieces[KING + 6 * b -> turn]) << 1) || is_attacked(b, (b -> pieces[KING + 6 * b -> turn]) << 2))
                return 0;
            return 1;
        }
        else {
            if (is_attacked(b, (b -> pieces[KING + 6 * b -> turn]) >> 1) || is_attacked(b, (b -> pieces[KING + 6 * b -> turn]) >> 2))
                return 0;
            return 1;
        }
    }
    make_move(&temp, m, 0, NULL);
    temp.turn ^= 1;
    return !is_check(&temp);
}



int is_aligned(const bitboard king_square, const bitboard piece_square){
    int piece_index = __builtin_ctzll(piece_square);
    return ((brays[piece_index].s_east | brays[piece_index].s_west | brays[piece_index].n_east | brays[piece_index].n_west |
        rrays[piece_index].east | rrays[piece_index].west | rrays[piece_index].north | rrays[piece_index].south) & king_square) > 0;
    
}

bitboard get_pinned(const board *b){
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    bitboard my_piece = b -> player_pieces[b -> turn];
    bitboard opp_piece;
    bitboard king_square = b -> pieces[KING + 6 * b -> turn];
    int index = __builtin_ctzll(king_square);
    bitboard pinned = 0;
    bitboard temp;
    bitboard temp2;
    bitboard blockers;
    // First rows
    if ((blockers = occupied & rrays[index].west)){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
                }
            }
        
            
    }
    if ((blockers = occupied & rrays[index].east)){
        temp = 1ULL << __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL <<  __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
            
    }
    //Seconde columns
    if ((blockers = occupied & rrays[index].north)){
        temp = 1ULL << __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL <<  __builtin_ctzll(blockers );
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
    }
    if ((blockers = occupied & rrays[index].south)){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL <<  (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
    }
    //Now diagonals
    //right up
    if ((blockers = occupied & brays[index].n_east)){
        temp = 1ULL <<  __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }
        }
    }
    //left up
    if ((blockers = occupied & brays[index].n_west)){
        temp = 1ULL <<  __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
    }
    //right down
    if ((blockers = occupied & brays[index].s_east)){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
            }
        }
    }
    //left down
    if ((blockers = occupied & brays[index].s_west)){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * (b -> turn ^ 1)] | b -> pieces[QUEEN + 6 * (b -> turn ^ 1)];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
            }
        }
    }
    return pinned;
}

int is_repetition(const board *b){
    //Saying if the last move caused draw
    int last = (b -> rep->idx == 0) ? 149 : (b -> rep -> idx - 1);
    bitboard last_move = (b -> rep) -> rep_table[last];
    int i = (b -> rep) -> idx_start_looking;
    int count = 1;
    do{
        if (i == 150)
            i = 0;
        if((b -> rep) -> rep_table[i] == last_move)
            count++;
        i++;
    }while(i != (last) && count < 3);
    return count == 3;
}


int insufficient_material(const board *b){
    int w = __builtin_popcountll(b -> player_pieces[0]);
    int bl = __builtin_popcountll(b -> player_pieces[1]);

    // K vs K
    if (w == 1 && bl == 1) 
        return 1;

    // K+(bishop or knight) vs K
    if (w == 2 && bl == 1 && (b -> pieces[BISHOP + 6] || b -> pieces[KNIGHT + 6])) 
        return 1;
    if (bl == 2 && w == 1 && (b -> pieces[BISHOP] || b -> pieces[KNIGHT]))
         return 1;

    // K+(bishop or knight) vs K+(bishop or knight)
    if (w == 2 && bl == 2 &&
        (b->pieces[BISHOP] || b->pieces[KNIGHT]) &&
        (b->pieces[BISHOP + 6] || b->pieces[KNIGHT + 6])) 
        return 1;
    return 0;
}

int is_checkmate(board *b){
    list_move l;
    legal_moves(b , &l);
    if (l.size == 0){
        if (is_check(b))
            return 1;
    }
    return 0;
}

int  is_draw(board *b){
    if (b->fifty_moves >= 150)
        return 1;
    if (is_repetition(b))
        return 1;
    if (insufficient_material(b))
        return 1;
    list_move l;
    legal_moves(b , &l);
    if (l.size == 0){
        if (is_check(b))
            return 0;
        return 1;
    }
    return 0;
}

int game_state(board *b){
    /*0 for draw and 1 for win, 2 for nothing */
    if (b->fifty_moves >= 150)
        return 0;
    list_move l;
    legal_moves(b, &l);
    if (l.size == 0){
        if (is_check(b))
            return 1;
        return 0;
    }
    if (is_repetition(b))
        return 0;
    if (insufficient_material(b))
        return 0;
    return 2;
}