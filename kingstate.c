#include "header.h"

extern rooksray rrays[64];
extern bishopray brays[64];
extern u64 kings_table[64];


king_state get_king_state(board *b){
    king_state ks;
    memset(&ks, 0, sizeof(king_state));

    u8 i, count, r_q_check_count, b_q_check_count;
    i8 dir;
    u64 attackers, safe_sq_temp, trailling_zeros;
    u64 my_piece = b -> player_pieces[b -> turn];
    u64 opp_piece = b -> player_pieces[b -> turn ^ 1];
    u64 occupied = my_piece | opp_piece;
    u8 offset = 6 * b -> turn;
    u8 opp_offset = 6 * (b -> turn ^ 1);
    u64 king_sq = b -> pieces[KING + offset];
    u8 king_sq_idx = __builtin_ctzll(king_sq);
    u8 king_row_idx = king_sq_idx / 8;
    u8 king_col_idx = king_sq_idx % 8;
    u64 pin = 0;
    //safe square (heavy version to upgrade later)
    ks.safe_sq = kings_table[king_sq_idx] & ~my_piece;
    safe_sq_temp = ks.safe_sq;
    delete_piece(b, b -> turn, KING, king_sq);
    while(safe_sq_temp){
        trailling_zeros = 1ULL << __builtin_ctzll(safe_sq_temp);
        if (is_attacked(b, trailling_zeros))
            ks.safe_sq &= ~trailling_zeros;
        safe_sq_temp &= (safe_sq_temp - 1);

    }
    add_piece(b, b -> turn, KING, king_sq);
    attackers = 0;
    if (b -> turn == WHITE && (king_sq & ~ROWS[7]))
        attackers = b -> pieces[PAWN] & (((king_sq << 9) & ~COLUMNS[0]) | ((king_sq << 7) & ~COLUMNS[7]));
    else if (b -> turn == BLACK && (king_sq & ~ROWS[0]))
        attackers = b -> pieces[PAWN + 6] & (((king_sq >> 9) & ~COLUMNS[7]) | ((king_sq >> 7) & ~COLUMNS[0]));
    if (attackers){
        ks.checkers = attackers;
        ks.num_checkers = 1;
        ks.block_mask = attackers;
        
    }

    u64 Q_R_mask = b -> pieces[QUEEN + opp_offset] | b -> pieces[ROOK + opp_offset];
    u64 Q_B_mask = b -> pieces[QUEEN + opp_offset] | b -> pieces[BISHOP + opp_offset];
    dir = 0;
    r_q_check_count = 0;
    attackers = rrays[king_sq_idx].east & Q_R_mask;
    if (attackers){ //If there is an attacker on this row we check if it's a check or pinned or nothing
        //We only care about the first attacker
        count = 0;
        i = 1;
        while ((Q_R_mask & (king_sq << i)) == 0){
            if (b -> player_pieces[b -> turn ^ 1] & (king_sq << i)) //There is an opp piece between the king and the queen/rook
                break; //no pinned, no check
            if (b -> player_pieces[b -> turn] & (king_sq << i)){ //There is one of our piece between the king and the queen/rook
                count++; //pin count + 1
                if (count == 2)
                    break; //there is 2 pieces between our king and the attacker so no pin
                pin = king_sq << i;
            }
            i++;
        }
        if (Q_R_mask & (king_sq << i)){
            if (count == 0){                            
                ks.checkers |= (king_sq << i);
                ks.num_checkers++;
                dir = 1;
                r_q_check_count++;
            } 
            else if (count == 1){                      
                ks.pinned |= pin;
            }
        }
        
    }
    if (r_q_check_count < 2){ //We can't have a check from a 2 rooks or 2 queens or queen/rook on rows/cols simultanously
        attackers = rrays[king_sq_idx].west & Q_R_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_R_mask & (king_sq >> i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq >> i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq >> i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq >> i;
                }
                i++;
            }
        if (Q_R_mask & (king_sq >> i)){
            if (count == 0){                            
                ks.checkers |= (king_sq >> i);
                ks.num_checkers++;
                r_q_check_count++;
                dir = -1;
            } 
            else if (count == 1){                      
                ks.pinned |= pin;
            }
        }
        }  
    }
    if (r_q_check_count < 2){ //We can't have a check from a 2 rooks or 2 queens or queen/rook on rows/cols simultanously
        attackers = rrays[king_sq_idx].north & Q_R_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_R_mask & (king_sq << 8 * i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq << 8 * i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq << 8 * i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq << 8 * i;
                }
                i++;
            }
            if (Q_R_mask & (king_sq << 8 * i)){
                if (count == 0){                            
                    ks.checkers |= (king_sq << 8 * i);
                    ks.num_checkers++;
                    r_q_check_count++;
                    dir = 8;
                } 
                else if (count == 1){                      
                    ks.pinned |= pin;
                }
            }
        }  
    }
    if (r_q_check_count < 2){ //We can't have a check from a 2 rooks or 2 queens or queen/rook on rows/cols simultanously
        attackers = rrays[king_sq_idx].south & Q_R_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_R_mask & (king_sq >> 8 * i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq >> 8 * i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq >> 8 * i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq >> 8 * i;
                }
                   
                i++;
            }
            if (Q_R_mask & (king_sq >> 8 * i)){
                if (count == 0){                            
                    ks.checkers |= (king_sq >> 8 * i);
                    ks.num_checkers++;
                    r_q_check_count++;
                    dir = -8;
                } 
                else if (count == 1){                      
                    ks.pinned |= pin;
                }
            }
        }  
    }
    attackers = brays[king_sq_idx].n_east & Q_B_mask;
    b_q_check_count = 0;
    if (attackers){ 
        count = 0;
        i = 1;
        while ((Q_B_mask & (king_sq << 9 * i)) == 0){
            if (b -> player_pieces[b -> turn ^ 1] & (king_sq << 9 * i)) 
                break; 
            if (b -> player_pieces[b -> turn] & (king_sq << 9 * i)){
                count++; //pin count + 1
                if (count == 2)
                    break; //there are 2 pieces between our king and the attacker so no pin
                pin = king_sq << 9 * i;
            }
                
            i++;
        }
        if (Q_B_mask & (king_sq << 9 * i)){
            if (count == 0){                            
                ks.checkers |= (king_sq << 9 * i);
                ks.num_checkers++;
                dir = 9;
                b_q_check_count++;
            } 
            else if (count == 1){                      
                ks.pinned |= pin;
            }
        }
    } 
    if (b_q_check_count < 2){
        attackers = brays[king_sq_idx].n_west & Q_B_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_B_mask & (king_sq << 7 * i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq << 7 * i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq << 7 * i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq << 7 * i;
                }
                    
                i++;
            }
            if (Q_B_mask & (king_sq << 7 * i)){
                if (count == 0){                            
                    ks.checkers |= (king_sq << 7 * i);
                    ks.num_checkers++;
                    dir = 7;
                    b_q_check_count++;
                } 
                else if (count == 1){                      
                    ks.pinned |= pin;
                }
            }
        } 
    } 
    if (b_q_check_count < 2){
        attackers = brays[king_sq_idx].s_east & Q_B_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_B_mask & (king_sq >> 7 * i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq >> 7 * i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq >> 7 * i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq >> 7 * i;
                }
                   
                i++;
            }
            if (Q_B_mask & (king_sq >> 7 * i)){
                if (count == 0){                            
                    ks.checkers |= (king_sq >> 7 * i);
                    ks.num_checkers++;
                    dir = -7;
                    b_q_check_count++;
                } 
                else if (count == 1){                      
                    ks.pinned |= pin;
                }
            }
        } 
    } 
    if (b_q_check_count < 2){
        attackers = brays[king_sq_idx].s_west & Q_B_mask;
        if (attackers){ 
            count = 0;
            i = 1;
            while ((Q_B_mask & (king_sq >> 9 * i)) == 0){
                if (b -> player_pieces[b -> turn ^ 1] & (king_sq >> 9 * i)) 
                    break; 
                if (b -> player_pieces[b -> turn] & (king_sq >> 9 * i)){
                    count++; //pin count + 1
                    if (count == 2)
                        break; //there are 2 pieces between our king and the attacker so no pin
                    pin = king_sq >> 9 * i;
                }
                
                i++;
            }
            if (Q_B_mask & (king_sq >> 9 * i)){
                if (count == 0){                            
                    ks.checkers |= (king_sq >> 9 * i);
                    ks.num_checkers++;
                    dir = -9;
                    b_q_check_count++;
                } 
                else if (count == 1){                      
                    ks.pinned |= pin;
                }
            }
        } 
    }
    attackers = knight_table[king_sq_idx] & b -> pieces[KNIGHT + 6 * (b -> turn ^ 1)]; //knight part
    if (attackers){ //the only way to stop a knight attack for a piece to protect the king is to eat the knight
        ks.num_checkers++;
        ks.block_mask |= attackers;
        ks.checkers |= attackers;
    }
    if (ks.num_checkers == 1) {
        u64 checker = ks.checkers & -ks.checkers;   
        ks.block_mask = checker;                   

        if (dir != 0) {                            
            u64 t = king_sq;

            for (int step = 0; step < 7; step++) {
                if (dir == 1)       
                    t <<= 1;
                else if (dir == -1)
                    t >>= 1;
                else if (dir == 8)
                    t <<= 8;
                else if (dir == -8) 
                    t >>= 8;
                else if (dir == 9)  
                    t <<= 9;
                else if (dir == -9)
                    t >>= 9;
                else if (dir == 7)
                    t <<= 7;
                else if (dir == -7) 
                    t >>= 7;

                ks.block_mask |= t;
                if (t == checker) 
                    break;
            }
        }
    } else {
        ks.block_mask = 0;
    }

    return ks;
}