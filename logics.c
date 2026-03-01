#include "header.h"

bitboard east_table[64];
bitboard west_table[64];
bitboard south_table[64];
bitboard north_table[64];
bitboard knight_table[64];
bitboard n_east_table[64];
bitboard n_west_table[64];
bitboard s_east_table[64];
bitboard s_west_table[64];
bitboard kings_table[64];


void init(){
    init_zobrist_tables();
    memset(east_table, 0, 64 * sizeof(bitboard));
    memset(west_table, 0, 64 * sizeof(bitboard));
    memset(south_table, 0, 64 * sizeof(bitboard));
    memset(north_table, 0, 64 * sizeof(bitboard));

    bitboard square;
    int count;
    for (int i = 0; i < 64; i++){
        square = 1ULL << i;
        count = 1;
        int row = i/8;
        int col = i % 8;
        while (count < 8 && ((square << count) & ROWS[row]) != 0 ){
            east_table[i] |= square << count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> count) & ROWS[row]) != 0 ){
            west_table[i] |= square >> count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square << 8 * count) & COLUMNS[col]) != 0 ){
            north_table[i] |= square << 8 * count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> 8 * count) & COLUMNS[col]) != 0 ){
            south_table[i] |= square >> 8 * count;
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
    memset(n_east_table, 0, 64 * sizeof(bitboard));
    memset(n_west_table, 0, 64 * sizeof(bitboard));
    memset(s_east_table, 0, 64 * sizeof(bitboard));
    memset(s_west_table, 0, 64 * sizeof(bitboard));
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
            n_east_table[i] |= square << (8 * vertical + horizontal);
            horizontal++;
            vertical++;
            
        }
        //UP LEFT
        horizontal = 1;
        vertical = 1;
        while ((row + vertical) < 8 && (col - horizontal >= 0)){
            n_west_table[i] |= square << (8 * vertical - horizontal);
            horizontal++;
            vertical++;
            
        }
        //DOWN RIGHT
        horizontal = 1;
        vertical = 1;
        while ((row - vertical >= 0) && col + horizontal < 8){
            s_east_table[i] |= square >> (8 * vertical - horizontal);
            horizontal++;
            vertical++;
            
        }
        //DOWN LEFT
        horizontal = 1;
        vertical = 1;
        while ((row - vertical >= 0) && (col - horizontal >= 0)){
            s_west_table[i] |= square >> (8 * vertical + horizontal);
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
    int j = ROOK + 6 * b -> turn;
    bitboard Q_R_MASK = b -> pieces[j] | b -> pieces[3 + j];
    bitboard index;
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    bitboard blockers = occupied & east_table[__builtin_ctzll(square)];
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    blockers = occupied & west_table[__builtin_ctzll(square)];
    if (blockers){
        index = 63 - __builtin_clzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    return 0;
}


int is_attacked_column(const board *b, bitboard square){

    int j = ROOK + 6 * b -> turn;
    bitboard Q_R_MASK = b -> pieces[j] | b -> pieces[3 + j];
    bitboard index;
    bitboard occupied =  b -> player_pieces[0] | b -> player_pieces[1];
    bitboard blockers = occupied & south_table[__builtin_ctzll(square)];
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    blockers = occupied & north_table[__builtin_ctzll(square)];
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_R_MASK)
            return 1;
    }
    return 0;
}


int is_attacked_diagonal(const board *b, bitboard square){
    int j = BISHOP + 6 * b -> turn;
    bitboard Q_B_MASK = b -> pieces[j] | b -> pieces[ 1 + j];
    bitboard index;
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    bitboard blockers = occupied & n_east_table[__builtin_ctzll(square)];
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & n_west_table[__builtin_ctzll(square)];
    if (blockers){
        index = __builtin_ctzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & s_east_table[__builtin_ctzll(square)];
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    blockers = occupied & s_west_table[__builtin_ctzll(square)];
    if (blockers){
        index = 63 -__builtin_clzll(blockers);
        if ((1ULL << index) & Q_B_MASK)
            return 1;
    }
    return 0;
}

int is_attacked_by_knight(const board *b, bitboard square){
    bitboard opp = b -> pieces[KNIGHT + 6 * b -> turn];
    return (opp & knight_table[__builtin_ctzll(square)]) > 0;
}

int is_attacked_pawn(const board *b, bitboard square){
    int row = get_row(square);
    int column = get_column(square);
    if (b -> turn == WHITE)
        return ((on_board(row + 1 ,column + 1) &&  (((square << 9) & b -> pieces[PAWN + 6]) > 0))) || ((on_board(row +1 ,column - 1) &&  (((square << 7) & b -> pieces[PAWN + 6]) > 0)));
    return ((on_board(row - 1 ,column + 1) &&  (((square >> 7) & b -> pieces[PAWN]) > 0))) || ((on_board(row - 1 ,column - 1) &&  (((square >> 9) & b -> pieces[PAWN]) > 0)));
}     

int is_attacked_king(const board *b, bitboard square){
    bitboard opp_king = b -> pieces[KING + 6 * b -> turn];
    return (opp_king & kings_table[__builtin_ctzll(square)]) > 0;
}

int is_attacked(const board *b, bitboard square){
    return is_attacked_column(b, square) || is_attacked_diagonal(b, square) || 
        is_attacked_row(b, square) || is_attacked_by_knight(b, square) || is_attacked_pawn(b, square) || is_attacked_king(b, square);
}

void make_move(board *b, move m, int rep, unmake_info *info){
    bitboard src = get_m_bitboard(m, 0);
    bitboard dst = get_m_bitboard(m, 1);
    uint32_t promotion = get_m_int(m, 3);
    //info for unmake function, info = NULL if you don't want to use unmake func
    if (info){
        info -> castles = b -> castles;
        info -> w_en_passant_flag = b -> w_en_passant_flag;
        info -> b_en_passant_flag = b -> b_en_passant_flag;
        info -> fifty_moves = b -> fifty_moves;
        info -> m = m;
        info -> piece_dst = 7; //Set to 7 while we don't know if there's a piece at src (ROOKs to pawns are between 0 and 5 include)
        info  -> flags_enP_prom = 0; 
        if (rep)
            info -> rep_idx = b -> rep -> idx_start_looking;
        else
            info -> rep_idx = 255;
    }
    b -> fifty_moves++;
    bitboard occupied =  b -> player_pieces[0] | b -> player_pieces[1];
    if (b -> turn)
        b -> w_en_passant_flag = -1;
    else
        b -> b_en_passant_flag = -1;
    uint32_t i = get_m_int(m, 2) + 6 * (b -> turn ^ 1);
    int opp_piece = 0;
    if (dst & occupied){
        if (rep)
            b -> rep -> idx_start_looking = b -> rep ->idx;
        opp_piece = 6 * b -> turn;
        while ((b -> pieces[opp_piece] & dst) == 0)
            opp_piece++;
            
        if (info)
            info -> piece_dst = opp_piece - 6 * b -> turn;
    }

    if (i == KING || i == PAWN ||i == KING + 6 || i == PAWN + 6){
        if (i == PAWN || i == PAWN + 6){
            if (rep)
                b -> rep -> idx_start_looking = b -> rep ->idx;
            if ((src & ROWS[(b -> turn ^ 1) * 5 + 1]) > 0 && dst & ROWS[(b -> turn ^ 1) * 1  + 3]){ // If we move from 2nd row to 4th or 7th to 5th (en_passant)
                if (b -> turn)
                    b -> w_en_passant_flag = get_column(src);
                else
                    b -> b_en_passant_flag = get_column(src);
            }
   
            b -> pieces[PAWN + (b -> turn ^ 1) * 6] &= ~src;
            b -> player_pieces[b -> turn] &= ~src;
            b -> pieces[PAWN + (b -> turn ^ 1) * 6] |= dst;
            b -> player_pieces[b -> turn] |= dst;
            if (dst & occupied){
                b -> player_pieces[b -> turn ^ 1] &= ~dst; 
                b -> pieces[opp_piece] &= ~dst;
            }
            
            if (b -> turn && ((dst & ((src << 9) | (src << 7))) > 0) && (dst & occupied) == 0){ //We're en passening with whites
                if (info)
                    info  -> flags_enP_prom |= 1; //info end here
                b -> pieces[PAWN + 6] &= ~(dst >> 8);
                b -> player_pieces[0] &= ~(dst >> 8);
            } 
            if (b -> turn == 0 && ((dst & ((src >> 9) | (src >> 7))) > 0) && (dst & occupied) == 0){ //We're en passening with black
                if (info)
                    info  -> flags_enP_prom |= 1; //info end here
                b -> pieces[PAWN ] &= ~(dst << 8);
                b -> player_pieces[1] &= ~(dst << 8);
            } 
            if (b -> turn && (src & ROWS[6]) > 0){ //white promotion part
                b -> pieces[PAWN + (b -> turn ^ 1) * 6] &= ~dst; // We delete the pawn that we added 10 lines before
                if (promotion == ROOK){
                    b -> pieces[ROOK] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 2;
                }
                else if (promotion == KNIGHT){
                    b -> pieces[KNIGHT] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 4;
                }
                else if (promotion == BISHOP){
                    b -> pieces[BISHOP] |= dst;
                    b -> player_pieces[1] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 8;
                }

                else{
                    b -> pieces[QUEEN] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 16;
                }
            }
            if (b -> turn == 0 && (src & ROWS[1]) > 0){ //black promotion part
                b -> pieces[PAWN + (b -> turn ^ 1) * 6] &= ~dst; // We delete the pawn that we added 10 lines before
                if (promotion == ROOK){
                    b -> pieces[ROOK + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 2;
                }
                else if (promotion == KNIGHT){
                    b -> pieces[KNIGHT + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 4;
                }
                else if (promotion == BISHOP){
                    b -> pieces[BISHOP + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 8;
                }
                else{
                    b -> pieces[QUEEN + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 16;
                }
            }
        }
        else{
            b -> castles &= ~(3 + (b -> turn ^ 1)* 9);
            b -> pieces[i] &= ~src;
            b -> player_pieces[b -> turn] &= ~src;
            b -> player_pieces[b -> turn] |= dst;
            b -> pieces[i] |= dst;
            if (dst & occupied){
                b -> player_pieces[b -> turn ^ 1] &= ~dst; 
                b -> pieces[opp_piece] &= ~dst;
            }
            if (dst == (src << 2)) {// If we're castling
                b -> pieces[i - 4] &= ~(dst << 1);
                b -> pieces[i - 4] |= src << 1;
                b -> player_pieces[b -> turn] |= src << 1;
                b -> player_pieces[b -> turn] &= ~(dst << 1);
            }
            if ((dst == (src >> 2))){
                b -> pieces[i - 4] &= ~(dst >> 2);
                b -> pieces[i - 4] |= dst << 1;
                b -> player_pieces[b -> turn] |= dst << 1;
                b -> player_pieces[b -> turn] &= ~(dst >> 2);
            }
        }

    }
    else{   
        b -> pieces[i] &= ~src;
        b -> player_pieces[b -> turn] &= ~src;
        b -> pieces[i] |= dst;
        b -> player_pieces[b -> turn] |= dst;
        if (dst & occupied){
            b -> player_pieces[b -> turn ^ 1] &= ~dst; 
            b -> pieces[opp_piece] &= ~dst;
        }

        
    }
    if (i == ROOK){ // We delete rook castle right
        if (src == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (src == (1ULL << 7))
            b -> castles &= ~1;
    }
    if (i == ROOK + 6){
        if (src == (1ULL << 56)) //We can't long castle with blacks
            b -> castles &= ~8;
        if (src == (1ULL << 63))
            b -> castles &= ~4;
    }
    if ((dst & occupied ) > 0 && opp_piece == ROOK){ // We delete a rook castle right
        if (dst == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (dst == (1ULL << 7))
            b -> castles &= ~1;
    }
    if ((dst & occupied ) > 0 && opp_piece == ROOK + 6){
        if (dst == (1ULL << 56)) //We can't long castle with blacks
            b -> castles &= ~8;
        if (dst == (1ULL << 63))
            b -> castles &= ~4;
    }
    if (rep){
        (b -> rep) -> rep_table[(b -> rep) -> idx] = zobrist_key(b);
        if(b -> rep -> idx >= 149)
            b -> rep -> idx = 0;
        else
            (b -> rep -> idx)++;
    }
    b -> turn ^= 1; 
}  

int is_check(const board *b){
    bitboard square = b -> pieces[KING + 6 * (b -> turn ^ 1)];
    return is_attacked(b,square);
}

void rook_moves(const board *b, bitboard square, list_move *l, uint32_t piece){
    bitboard my_piece = b -> player_pieces[1];
    bitboard opp_piece = b -> player_pieces[0];
    if (b -> turn == BLACK){
        my_piece = b -> player_pieces[0];
        opp_piece = b -> player_pieces[1];
    }
    int row = get_row(square);
    int column = get_column(square );
    bitboard mask_row = ROWS[row];
    bitboard mask_column = COLUMNS[column];

    int i = 1;
    while(((square << i) & mask_row) > 0){ //checking right rows 
        if ((my_piece & square << i) > 0)
            break;
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square << i), piece, NO_PROM); 
        (l -> index)++;
        if ((opp_piece & square << i) > 0)
            break;
        i++;
    }

    i = 1;
    while(((square >> i) & mask_row) > 0){ //checking left rows 
        if ((my_piece & square >> i) > 0)
            break;

        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square >> i), piece, NO_PROM); 
        (l -> index)++;

        if ((opp_piece & square >> i) > 0)
            break;
        i++;
    }

    i = 1;
    while(((square << 8*i) & mask_column ) > 0){ //checking up 
        if ((my_piece & square << 8* i) > 0)
            break;
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i)), piece, NO_PROM); 

        (l -> index)++;
        if ((opp_piece & square << 8 * i) > 0)
            break;
        i++;
    }
    
    i = 1;
    while(((square >> 8*i) & mask_column) > 0){ //checking  down
        if ((my_piece & square >> 8* i) > 0)
            break;
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i)), piece, NO_PROM); 
        (l -> index)++;
        if ((opp_piece & square >> 8 * i) > 0)
            break;
        i++;
    }
}


void bishop_moves(const board *b, bitboard square, list_move *l, uint32_t piece){
    bitboard my_piece = b -> player_pieces[1];
    bitboard opp_piece = b -> player_pieces[0];
    if (b -> turn == BLACK){
        my_piece = b -> player_pieces[0];
        opp_piece = b -> player_pieces[1];
    }
    
    int row = get_row(square);
    int column = get_column(square);

    int i = 1;
    int j = 1;
   
    while((square << (8*i + j)  > 0) && (row + i < 8 ) && (column + j < 8)){ //checking right_up diag
        if ((my_piece & square << (8*i + j)) > 0)
            break;
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i + j)), piece, NO_PROM); 
        (l -> index)++;
        if ((opp_piece & square << (8 * i + j)) > 0)
            break;
        i++;
        j++;

    }
    
    i = 1;
    j = 1;
    while(((square >> (8*i + j)) > 0) && (row - i >= 0) && (column - j >= 0)){ //checking right_downs rows
        if ((my_piece & square >> (8*i + j)) > 0)
            break;
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i + j)), piece, NO_PROM); 
        (l -> index)++;
        
        if ((opp_piece & square >> (8 * i + j)) > 0)
            break;
        i++;
        j++;
        
    }
    
    i = 1;
    j = 1;
    while(((square << (8*i - j))  > 0) && (row + i < 8) && (column - j >= 0)){ //checking up 
        if ((my_piece & square << (8*i - j)) > 0)
            break;
 
        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i - j)), piece, NO_PROM); 
        (l -> index)++;
      
        if ((opp_piece & square << (8 * i - j)) > 0)
            break;
        i++;
        j++;
    }
   
    i = 1;
    j = 1;
    while((((square >> (8*i - j))) > 0) && (row - i >= 0) && (column + j < 8)){ //checking  down
        if ((my_piece & square >> (8*i - j)) > 0)
            break;

        l -> m[l -> index] = create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i - j)), piece, NO_PROM); 
        (l -> index)++;
        
        if ((opp_piece & square >> (8 * i - j)) > 0)
            break;
        i++;
        j++;
    }
    
}

void queen_moves(const board *b, bitboard square, list_move *l){
    rook_moves(b, square, l, QUEEN);
    bishop_moves(b, square, l, QUEEN);
}

void knight_moves(const board *b, bitboard square, list_move *l){
    int square_idx = __builtin_ctzll(square);
    bitboard all_moves = knight_table[square_idx] & ~(b -> player_pieces[b -> turn]);
    int trailling_zeros;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = create_move(square_idx,trailling_zeros, KNIGHT, NO_PROM); 
        (l -> index)++;
        all_moves &= (all_moves -1);
    }
}

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
            l -> m[l -> index] = create_move(square_idx, 6, KING, NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == WHITE && (b -> castles & 2)){ //white can long castle
        if ((~occupied & 8) && (~occupied & 4) && (~occupied & 2)){
            l -> m[l -> index] = create_move(square_idx, 2, KING, NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 4)){ //black can castle
        if ((~occupied & (1ULL << 61)) && (~occupied & (1ULL << 62))){
            l -> m[l -> index] = create_move(square_idx, 62, KING, NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == BLACK && (b -> castles & 8)){ //black can long castle
        if ((~occupied & (1ULL << 59)) && (~occupied & (1ULL << 58)) && (~occupied & (1ULL << 57))){
            l -> m[l -> index] = create_move(square_idx, 58, KING, NO_PROM); 
            (l -> index)++;
        }
    }

}


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
                l -> m[l -> index] = create_move(square_idx + 1 , square_idx + 8, PAWN, NO_PROM);
                (l -> index)++;
            }
            if (no_prom_pawns & ROWS[4] & (square >> 1)){
                l -> m[l -> index] = create_move(square_idx - 1 , square_idx + 8, PAWN, NO_PROM);
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
                l -> m[l -> index] = create_move(square_idx + 1 , square_idx - 8, PAWN, NO_PROM);
                (l -> index)++;
            }
            if (no_prom_pawns & ROWS[3] & (square >> 1)){
                l -> m[l -> index] = create_move(square_idx - 1 , square_idx - 8, PAWN, NO_PROM);
                (l -> index)++;
            }
        }
    }
}


void rook_all_moves(const board *b, list_move *l){
    int i = 0;
    if (b -> turn == BLACK)
        i += 6; 
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        rook_moves(b, 1ULL << trailing_zeros, l, ROOK);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    } 
}


void bishop_all_moves(const board *b, list_move *l){
    int i = 2;
    if (b -> turn == BLACK)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        bishop_moves(b, 1ULL << trailing_zeros, l, BISHOP);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }     
}

void knight_all_moves(const board *b, list_move *l){
    int i = 1;
    if (b -> turn == BLACK)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        knight_moves(b, 1ULL << trailing_zeros, l);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }       
}

void queen_all_moves(const board *b, list_move *l){
    int i = 3;
    if (b -> turn == BLACK)
        i += 6;
    bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        queen_moves(b, 1ULL << trailing_zeros, l);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
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
    pseudo_legal_moves(b, l);
    
    if (l -> size == 0)
        return;
    int new_ind = 0;
    unmake_info info;
    bitboard pinned = get_pinned(b);
    int is_check_value = is_check(b);
    for (int index = 0 ; index < l -> size; index++){
        if ((get_m_bitboard(l -> m[index], 0) & pinned) == 0 && is_check_value == 0 && get_m_int(l -> m[index], 2) != KING && 
            is_not_en_passant(b, get_m_bitboard(l -> m[index], 0), get_m_bitboard(l -> m[index], 1), get_m_int(l -> m[index], 2))){
            l -> m[new_ind] = l -> m[index];
            new_ind++;
        }
        else{
            if (b -> pieces[KING] == get_m_bitboard(l -> m[index], 0) ||  b -> pieces[KING + 6] == get_m_bitboard(l -> m[index], 0)){
                if((get_m_bitboard(l -> m[index], 0) << 2) == get_m_bitboard(l -> m[index], 1) || 
                    (get_m_bitboard(l -> m[index], 0) >> 2) == get_m_bitboard(l -> m[index], 1)){
                    if (is_check(b))
                        continue;
                    if ((get_m_bitboard(l -> m[index], 0) << 2) == get_m_bitboard(l -> m[index], 1)){
                        if (is_attacked(b, get_m_bitboard(l -> m[index], 0) << 1) || is_attacked(b, get_m_bitboard(l -> m[index], 0) << 2))
                            continue;
                        l -> m[new_ind] = l -> m[index];
                        new_ind++;
                        continue;
                    }
                    else {
                        if (is_attacked(b, get_m_bitboard(l -> m[index], 0) >> 1) || is_attacked(b, get_m_bitboard(l -> m[index], 0) >> 2))
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
            if (is_attacked(b, (b -> pieces[KING + 6 * (b -> turn ^ 1)]) << 1) || is_attacked(b, (b -> pieces[KING + 6 * (b -> turn ^ 1)]) << 2))
                return 0;
            return 1;
        }
        else {
            if (is_attacked(b, (b -> pieces[KING + 6 * (b -> turn ^ 1)]) >> 1) || is_attacked(b, (b -> pieces[KING + 6 * (b -> turn ^ 1)]) >> 2))
                return 0;
            return 1;
        }
    }
    make_move(&temp, m, 0, NULL);
    temp.turn ^= 1;
    return !is_check(&temp);
}

void unmake(board *b, unmake_info *info){
    bitboard src = get_m_bitboard(info -> m, 0);
    bitboard dst = get_m_bitboard(info -> m, 1);
    uint32_t piece_src = get_m_int(info -> m, 2);
    //Castles part
    if ((b -> pieces[KING] & dst) > 0 && (src << 2) == dst ){ //White castle
        b -> pieces[ROOK] |= (1ULL << 7);
        b -> pieces[ROOK] &= ~(1ULL << 5);
    }
    else if ((b -> pieces[KING] & dst) > 0 && (src >> 2) == dst ){ //White long castle
        b -> pieces[ROOK] |= (1ULL << 0);
        b -> pieces[ROOK] &= ~(1ULL << 3);
    }
    else if ((b -> pieces[KING + 6] & dst) > 0 && (src << 2) == dst ){ //black castle
        b -> pieces[ROOK + 6] |= (1ULL << 63);
        b -> pieces[ROOK + 6] &= ~(1ULL << 61);
    }
    else if ((b -> pieces[KING + 6] & dst) > 0 && (src >> 2) == dst ){ //black long castle
        b -> pieces[ROOK + 6] |= (1ULL << 56);
        b -> pieces[ROOK + 6] &= ~(1ULL << 59);
    }
    //putting the original src piece back 
    b -> pieces[ piece_src + 6 * b -> turn] |= src;
    if (info -> flags_enP_prom <= 1) //No promotions happened
        b -> pieces[piece_src + 6 * b -> turn] &= ~(dst);
    else
        b -> pieces[ __builtin_ctzll((info -> flags_enP_prom ) >> 1) + 6 * b -> turn] &= ~(dst); //if prom happened
    
    //if captures happened (en_passant doesn't verify this condition)
    if (info -> piece_dst != 7)
        b -> pieces[info -> piece_dst + 6 * (b -> turn == BLACK)] |= dst; 
    if ((info -> flags_enP_prom & 1) == 1){  //en passant
        if (b -> turn == WHITE)
            b -> pieces[ piece_src] |= dst << 8;
        else   
            b -> pieces[ piece_src + 6] |= dst >> 8; 
    }
    b -> player_pieces[0] = 0;
    b -> player_pieces[1] = 0;
    for (int i = 0; i < 6; i++)
        b -> player_pieces[1] |= b -> pieces[i];
    for (int i = 6; i < 12; i++)
        b -> player_pieces[0] |= b -> pieces[i];
 
    b -> castles = info -> castles;
    b -> w_en_passant_flag = info -> w_en_passant_flag;
    b -> b_en_passant_flag = info -> b_en_passant_flag;
    b -> fifty_moves = info -> fifty_moves;
    if (info -> rep_idx != 255){
        if (b -> rep -> idx == 0)
            b -> rep -> idx = 149;
        else
            (b -> rep -> idx)--;
        b -> rep -> idx_start_looking = info -> rep_idx;
    }
    
    b -> turn ^= 1;
}


int is_aligned(const bitboard king_square, const bitboard piece_square){
    int piece_index = __builtin_ctzll(piece_square);
    return ((s_east_table[piece_index] | s_west_table[piece_index] | n_east_table[piece_index] | n_west_table[piece_index] |
        east_table[piece_index] | west_table[piece_index] | north_table[piece_index] | south_table[piece_index]) & king_square) > 0;
    
}

bitboard get_pinned(const board *b){
    bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    bitboard my_piece = b -> player_pieces[b -> turn];
    bitboard opp_piece;
    bitboard king_square = b -> pieces[KING + 6 - 6 * b -> turn];
    int index = __builtin_ctzll(king_square);
    bitboard pinned = 0;
    bitboard temp;
    bitboard temp2;
    bitboard blockers;
    // First rows
    if ((blockers = occupied & west_table[index])){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
                }
            }
        
            
    }
    if ((blockers = occupied & east_table[index])){
        temp = 1ULL << __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL <<  __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
            
    }
    //Seconde columns
    if ((blockers = occupied & north_table[index])){
        temp = 1ULL << __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL <<  __builtin_ctzll(blockers );
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
    }
    if ((blockers = occupied & south_table[index])){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[ROOK + 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
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
    if ((blockers = occupied & n_east_table[index])){
        temp = 1ULL <<  __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }
        }
    }
    //left up
    if ((blockers = occupied & n_west_table[index])){
        temp = 1ULL <<  __builtin_ctzll(blockers);
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << __builtin_ctzll(blockers);
                if (opp_piece & temp2)
                    pinned |= temp;
            }

        }
    }
    //right down
    if ((blockers = occupied & s_east_table[index])){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
            blockers &= ~temp;
            if (blockers){
                temp2 = 1ULL << (63 - __builtin_clzll(blockers));
                if (opp_piece & temp2)
                    pinned |= temp;
            }
        }
    }
    //left down
    if ((blockers = occupied & s_west_table[index])){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[BISHOP+ 6 * b -> turn] | b -> pieces[QUEEN + 6 * b -> turn];
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
    if (w == 2 && bl == 1 && (b -> pieces[BISHOP] || b -> pieces[KNIGHT])) 
        return 1;
    if (bl == 2 && w == 1 && (b -> pieces[BISHOP + 6] || b -> pieces[KNIGHT + 6]))
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