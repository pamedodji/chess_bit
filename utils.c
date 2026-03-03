#include "header.h"

void init_board(board *b, rep_struct *r){
    memset(b, 0, sizeof(*b));
    b -> turn = WHITE;
    b -> pieces[6] = 0x81ULL;
    b -> pieces[7] = 0X42ULL;
    b -> pieces[8] = 0X24ULL;
    b -> pieces[9] = 0X8ULL; 
    b -> pieces[10] = 0X10ULL;
    b -> pieces[11] = ROWS[1];
    for (int i = 0; i < 5; i++)
        b -> pieces[i] = b -> pieces[i + 6] << 56;
    b -> pieces[5] = ROWS[6];
    b -> castles = 1 + 2 + 4 + 8;
    b -> w_en_passant_flag = -1;
    b -> b_en_passant_flag = -1;
    b -> fifty_moves = 0;
    b -> player_pieces[0] = 0;
    b -> player_pieces[1] = 0;
    for (int i = 0; i < 6; i++)
        b -> player_pieces[BLACK] |= b -> pieces[i];
    for (int i = 6; i < 12; i++)
        b -> player_pieces[WHITE] |= b -> pieces[i];
    
    b -> rep = r;
    b -> rep -> idx = 1;
    b -> rep -> idx_start_looking = 0;
    memset(b -> rep -> rep_table, 0, sizeof(bitboard)*150);
    board cpy = *b;
    b -> rep -> rep_table[0] = zobrist_key(&cpy);
}


char *bitboard_to_board(board *b){
    char *new_board = malloc(65 * sizeof(char));
    if (!new_board)
        return NULL;
    for (int i = 0; i < 64; i++)
        new_board[i] = '.';
    bitboard black_pieces = 0;
    for (int i = 0; i < 6; i++){
        black_pieces |= b -> pieces[i];
    }
    bitboard white_pieces = 0;
    for (int i = 6; i < 12; i++){
        white_pieces |= b -> pieces[i];
    }
    bitboard occupied =  black_pieces | white_pieces;
    for (int i = 0; i < 64; i++){
        if ((occupied & (1ULL << i)) > 0){
            if ((b -> pieces[0] & (1ULL << i)) > 0){
                new_board[i] = 'r';
                continue;
            }
            if ((b -> pieces[6] & (1ULL << i)) > 0){
                new_board[i] = 'R';
                continue;
            }
            if ((b -> pieces[1] & (1ULL << i)) > 0){
                new_board[i] = 'n';
                continue;
            }
            if ((b -> pieces[7] & (1ULL << i)) > 0){
                new_board[i] = 'N';
                continue;
            }
            if ((b -> pieces[2] & (1ULL << i)) > 0){
                new_board[i] = 'b';
                continue;
            }
            if ((b -> pieces[8] & (1ULL << i)) > 0){
                new_board[i] = 'B';
                continue;
            }
            if ((b -> pieces[3] & (1ULL << i)) > 0){
                new_board[i] = 'q';
                continue;
            }
            if ((b -> pieces[9] & (1ULL << i)) > 0){
                new_board[i] = 'Q';
                continue;
            }
            if ((b -> pieces[4] & (1ULL << i)) > 0){
                new_board[i] = 'k';
                continue;
            }
            if ((b -> pieces[10] & (1ULL << i)) > 0){
                new_board[i] = 'K';
                continue;
            }
            if ((b -> pieces[5] & (1ULL << i)) > 0){
                new_board[i] = 'p';
                continue;
            }
            if ((b -> pieces[11] & (1ULL << i)) > 0){
                new_board[i] = 'P';
                continue;
            }
        }
    }
    new_board[64] = '\0';
    return new_board;
}

void print_board(board *b){
    char *board = bitboard_to_board(b);
    for (int i = 7; i >= 0; i--){
        for (int j = 0; j < 8; j++)
            printf("\t%c\t|",board[i * 8 + j]);
        printf("\n");
        printf("__________________________________________________________________________________________________________________________________");
        printf("\n");
    }
}

bitboard string_to_bitboard(char *string_square){
    char letter = string_square[0];
    int number = (int)(string_square[1] - '0');
    return 1ULL << ((number - 1) * 8 + (letter - 'a'));
}

char *bitboard_to_string(bitboard square){
    char *tab = malloc(sizeof(char) * 3);
    bitboard copy_square = square;
    int row = 0;
    while ((copy_square = copy_square >> 8) > 0)
        row++;
    bitboard mask_row = ROWS[row];

    copy_square = square;
    int column = 0;
    while ((mask_row & (copy_square >> 1)) > 0){
        copy_square >>= 1;
        column++;
    }
    tab[0] = (char)('a' + column);
    tab[1] = (char)('1' + row);
    tab[2] = '\0';
    return tab;
}

void print_bitboard_tab(bitboard *tab){
    if (!tab)
        return;
    int i = 0;
    while (tab[i] != 0){
        printf("%d : %s\n",i, bitboard_to_string(tab[i]));
        i++;
    }
}


long get_time_ms(){
    clock_t time = clock();
    return time * 1000 / CLOCKS_PER_SEC;
}

void print_moves(list_move *l_moves){
    int i;
    int row;
    int col;
    for (i = 0; i < l_moves -> size; i++){
        row = get_row(get_m_bitboard(l_moves -> m[i], 0));
        col = get_column(get_m_bitboard(l_moves -> m[i], 0));
        printf("%c", (char)('a' + col));
        printf("%c", (char)('1' + row));
        printf("\t");
        row = get_row(get_m_bitboard(l_moves -> m[i], 1));
        col = get_column(get_m_bitboard(l_moves -> m[i], 1));
        printf("%c", (char)('a' + col));
        printf("%c", (char)('1' + row));
        printf("\n");
    }
    printf("size %d\n",l_moves -> size);
}



long perft(board *b, int depth){
    if (depth == 0)
        return 1;
    long nodes = 0;
    list_move l;
    legal_moves(b, &l);
    unmake_info info;
    if (depth == 1)
        return l.size;
    for (int i = 0; i < l.size; i++){
   
        make_move(b, l.m[i], 0, &info);
        nodes += perft(b, depth - 1);
        unmake(b, &info);
    }
    return nodes;
}

void perft_divide(board *b, int depth){
    if (depth == 0)
        return;
    long temp_nodes = 0;
    long nodes = 0;
    list_move l;
    unmake_info info;
    legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        make_move(b, l.m[i], 0, &info);
        temp_nodes = perft(b, depth - 1);
        nodes += temp_nodes;
        printf("%s %s : %ld\n", bitboard_to_string(get_m_bitboard(l.m[i], 0)), bitboard_to_string(get_m_bitboard(l.m[i], 1)), temp_nodes);
        unmake(b, &info);
    }
    printf("\ntotal moves %d\n", l.size);
    printf("total nodes %ld\n", nodes);
}

long perft_2(board *b, int depth){
    if (depth == 0)
        return 1;
    long nodes = 0;
    list_move l;
    
    unmake_info info;
    if (depth == 1){
        legal_moves(b, &l);
        return l.size;
    }

    pseudo_legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        if (!is_legal_move(b, l.m[i]))
            continue;
        make_move(b, l.m[i], 0, &info);
        nodes += perft_2(b, depth - 1);
        unmake(b, &info);
    }
    return nodes;
}

void perft_divide_2(board *b, int depth){
    if (depth == 0)
        return;
    long temp_nodes = 0;
    long nodes = 0;
    list_move l;
    unmake_info info;
    pseudo_legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        if (!is_legal_move(b, l.m[i]))
            continue;
        make_move(b, l.m[i], 0, &info);
        temp_nodes = perft_2(b, depth - 1);
        nodes += temp_nodes;
        printf("%s %s : %ld\n", bitboard_to_string(get_m_bitboard(l.m[i], 0)), bitboard_to_string(get_m_bitboard(l.m[i], 1)), temp_nodes);
        unmake(b, &info);
    }
    printf("\ntotal moves %d\n", l.size);
    printf("total nodes %ld\n", nodes);
}

void set_position_2(board *b, rep_struct *rep){
    fen_to_board(b, rep, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
}

void set_position_3(board *b, rep_struct *rep) {
    fen_to_board(b, rep, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
}

void set_position_4(board *b, rep_struct *rep) {
    fen_to_board(b, rep, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");
}

void set_position_5(board *b, rep_struct *rep) {
    fen_to_board(b, rep, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");
}

int is_digit(char c){
    if (c <= '9' && c >= '0') 
        return c - '0';
    return -1;
}

void fen_to_board(board *b, rep_struct *rep, char *fen){
    memset(b, 0, sizeof(*b));
    memset(rep -> rep_table, 0, sizeof(bitboard) * 150);
    char c;
    int index = 0; 
    int count = 56;
    b -> w_en_passant_flag = -1;
    b -> b_en_passant_flag = -1;
    int digit;
    int temp_count = 0;
    int i;
    do{
        
   
        c = fen[index];
        if (c == ' ')
            break;

        
            
       
        if (c == '/' ){
            index++;
            temp_count = 0;
            count -= 16;
            continue;
        }
        if ((digit = is_digit(c)) >= 0){
            if (digit == 9 || digit == 0 || temp_count + digit >= 9){
                //Parsing error
                init_board(b, rep);
                return;
            }
            i = 1;
            
            while (fen[index] != '\0' && i  < digit)
                i++;
         
            if (i < digit){
                //Parsing error
                init_board(b, rep);
                return;
            }
 
            temp_count += digit;
            count += digit;
            index += 1; 

            continue;
        }   
        
        temp_count++;
        if (temp_count >= 9){
            //Parsing error
            init_board(b, rep);
            return;
        } 
        switch (c){
            case 'r':
                b -> pieces[ROOK] |=  (1ULL << count);
                break;
            case 'n':
                b -> pieces[KNIGHT] |=  (1ULL << count);
                break;
            case 'b':
                b -> pieces[BISHOP] |=  (1ULL << count);
                break;
            case 'q':
                b -> pieces[QUEEN] |=  (1ULL << count);
                break;
            case 'k':
                b -> pieces[KING] |=  (1ULL << count);
                break;
            case 'p':
                b -> pieces[PAWN] |=  (1ULL << count);
                break;
            case 'R':
                b -> pieces[ROOK + 6] |=  (1ULL << count);
                break;
            case 'N':
                b -> pieces[KNIGHT + 6] |=  (1ULL << count);
                break;
            case 'B':
                b -> pieces[BISHOP + 6] |=  (1ULL << count);
                break;
            case 'Q':
                b -> pieces[QUEEN + 6] |=  (1ULL << count);
                break;
            case 'K':
                b -> pieces[KING + 6] |=  (1ULL << count);
                break;
            case 'P':
                b -> pieces[PAWN + 6] |=  (1ULL << count);
                break;
            //Parsing error
            init_board(b, rep);
            return;
        } 
        index += 1;
        count++;
    }while (c != ' ' && c != '\0');    

    if (fen[index] == '\0' || fen[index + 1] == '\0'){
        //Parsing error
        init_board(b, rep);
        return;
    }
    index++;
    if (fen[index] == 'w')
        b -> turn = WHITE;
    else
        b -> turn = BLACK;

    if (fen[index + 1] == '\0'){
        //Parsing error
        init_board(b, rep);
        return;
    }
    index+=2;
    
    b -> castles = 0;

    if (fen[index] == 'K'){
        b -> castles |= 1;
        index++;
    }
    if (fen[index] == 'Q'){
        b -> castles |= 2;
        index++;
    }
    if (fen[index] == 'k'){
        b -> castles |= 4;
        index++;
    }
    if (fen[index] == 'q'){
        b -> castles |= 8;
        index++;
    }
    if (fen[index] == '-')
        index ++;
    
    if (fen[index] == '\0'){
        //Parsing error
        init_board(b, rep);
        return ;
    }
    index++;
    if (fen[index] == '-')
        index += 2;
    else if (fen[index] != '\0'){
        int col = fen[index] - 'a';
        index++;
        int row = fen[index] - '1';
        if (row == 2)
            b -> w_en_passant_flag = col;
        else
            b -> b_en_passant_flag = col;
        index += 2;
    }
    else{
        //Parsing error
        init_board(b, rep);
        return ;
    }
  
    b -> player_pieces[0] = 0;
    b -> player_pieces[1] = 0;
    for (int i = 0; i < 6; i++)
        b -> player_pieces[BLACK] |= b -> pieces[i];
    for (int i = 6; i < 12; i++)
        b -> player_pieces[WHITE] |= b -> pieces[i];
    b -> fifty_moves = 0;
}

void print_board_info(board *b){
    bitboard white_pieces = 0;
    for (int i = 6; i < 12; i++)
        white_pieces |= b -> pieces[i];
    printf("white pieces %lu\n", white_pieces);
    bitboard black_pieces = 0;
    for (int i = 0; i < 6; i++)
        black_pieces |= b -> pieces[i];
    printf("black pieces %lu\n", black_pieces);
    printf("castles right %u\n", b -> castles);
    printf("turn %d\n", b -> turn);
    printf("w_en_passant %d\n", b -> w_en_passant_flag);
    printf("b_en_passant %d\n", b -> b_en_passant_flag);
    printf("fifty moves %d\n",  b -> fifty_moves);
    
    
}

void free_board(board *b){
    if (b)
        free(b);
}

void verify_logics(int depth){
    board b;
    rep_struct rep;
    init_board(&b, &rep);
    perft_divide(&b, depth);
    printf("\n\n");
    set_position_2(&b, &rep);
    perft_divide(&b, depth);
    printf("\n\n");
    set_position_3(&b, &rep);
    perft_divide(&b, depth);
    printf("\n\n");
    set_position_4(&b, &rep);
    perft_divide(&b, depth);
    printf("\n\n");
    set_position_5(&b, &rep);
    perft_divide(&b, depth);

}