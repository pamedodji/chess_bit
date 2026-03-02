#include "../header.h"

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
