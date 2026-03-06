#include "../header.h"

void make_move(board *b, move m, int rep, unmake_info *info){
    bitboard src = get_m_bitboard(m, 0);
    bitboard dst = get_m_bitboard(m, 1);
    u32 flag = get_m_int(m, 3);
    //info for unmake function, info = NULL if you don't want to use unmake func
    if (info){
        info -> castles = b -> castles;
        info -> w_en_passant_flag = b -> w_en_passant_flag;
        info -> b_en_passant_flag = b -> b_en_passant_flag;
        info -> fifty_moves = b -> fifty_moves;
        info -> m = m;
        info -> piece_dst = 255; //Set to 255 while we don't know if there's a piece at src (ROOKs to pawns are between 0 and 5 include)
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
    u32 i = get_m_int(m, 2);
    int opp_piece = 7;
    if (dst & occupied){
        if (rep)
            b -> rep -> idx_start_looking = b -> rep ->idx;
        opp_piece = 6 * (b -> turn ^ 1);
        while ((b -> pieces[opp_piece] & dst) == 0)
            opp_piece++;
        opp_piece -= 6 * (b -> turn ^ 1);
        if (info)
            info -> piece_dst = opp_piece;
    }
    if (flag >= CASTLE)
        make_move_castle(b, m, flag, src, dst);
   
    else if (flag == EN_PASSANT)
        make_move_en_passant(b, m, info, src, dst);
        
    else if (flag < NO_PROM)
        make_move_promotion(b, m , info, flag, src, dst, opp_piece);
    
    else{
        
            if (i == PAWN){
                if (rep)
                    b -> rep -> idx_start_looking = b -> rep ->idx;
                if ((src & ROWS[(b -> turn ^ 1) * 5 + 1]) > 0 && dst & ROWS[(b -> turn ^ 1) * 1  + 3]){ // If we move from 2nd row to 4th or 7th to 5th (en_passant)
                    if (b -> turn)
                        b -> w_en_passant_flag = get_column(src);
                    else
                        b -> b_en_passant_flag = get_column(src);
                }
                

                
            }  
            if (i == KING)
                b -> castles &= ~(3 + (b -> turn ^ 1)* 9);
            delete_piece(b, b -> turn, i, src);
            add_piece(b, b -> turn, i, dst );
            if (dst & occupied)
                delete_piece(b, b -> turn ^ 1, opp_piece, dst);
        
    }
    if (i == ROOK && b -> turn){ // We delete rook castle right
        if (src == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (src == (1ULL << 7))
            b -> castles &= ~1;
    }
    if (i == ROOK && (b -> turn ^ 1)){
        if (src == (1ULL << 56)) //We can't long castle with blacks
            b -> castles &= ~8;
        if (src == (1ULL << 63))
            b -> castles &= ~4;
    }
    if ((dst & occupied ) > 0 && opp_piece == ROOK && (b -> turn ^ 1)){ // We delete a rook castle right
        if (dst == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (dst == (1ULL << 7))
            b -> castles &= ~1;
    }
    if ((dst & occupied ) > 0 && opp_piece == ROOK && b -> turn){
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
    u32 piece_src = get_m_int(info -> m, 2);
    u32 flag = get_m_int(info -> m, 3);
    //Castles part
    if (flag == CASTLE){
        add_piece(b, b -> turn ^ 1, ROOK, b -> pieces[KING + 6 * (b -> turn ^ 1)] <<  1);
        delete_piece(b, b -> turn ^ 1, ROOK, b -> pieces[KING + 6 * (b -> turn ^ 1)] >> 1);
    }
    else if (flag == LONG_CASTLE){
        add_piece(b, b -> turn ^ 1, ROOK, b -> pieces[KING + 6 * (b -> turn ^ 1)] >> 2);
        delete_piece(b, b -> turn ^ 1, ROOK, b -> pieces[KING + 6 * (b -> turn ^ 1)] << 1);
    }
    //putting the original src piece back 
    add_piece(b, b -> turn ^ 1, piece_src, src);
    //deleting the piece src from dst
    if (flag > 3) //No promotions happened
        delete_piece(b, b -> turn ^ 1, piece_src, dst);
    else 
        delete_piece(b, b -> turn ^ 1, flag, dst);

    
    //if captures happened and not en passant
    if (info -> piece_dst != 255 && flag != EN_PASSANT)
        add_piece(b, b -> turn, info -> piece_dst, dst);
    if (flag == EN_PASSANT){  //en passant
        if (b -> turn == WHITE)
            add_piece(b, b -> turn, PAWN, dst << 8);
        else   
            add_piece(b, b -> turn, PAWN, dst >> 8);
    }
 
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
