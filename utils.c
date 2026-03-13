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
    b -> rep -> idx_start_looking = 149;
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

long long get_real_time_ms(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
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



void perft_divide(board *b, int depth, int nb_threads) {
    if (depth == 0)
        return;

    list_move l;
    legal_moves(b, &l);

    if (l.size == 0) {
        printf("\ntotal moves 0\n");
        printf("total nodes 0\n");
        return;
    }

    int threads = nb_threads;
    if (threads < 1)
        threads = 1;
    if (threads > l.size)
        threads = l.size;

    long nodes = 0;
    long *results = malloc(sizeof(long) * l.size);
    if (!results)
        return;

    #pragma omp parallel for num_threads(threads) reduction(+:nodes)
    for (int i = 0; i < l.size; i++) {
        board local_board = *b;
        unmake_info info;

        make_move(&local_board, l.m[i], 0, &info);
        long temp_nodes = perft(&local_board, depth - 1);

        results[i] = temp_nodes;
        nodes += temp_nodes;
    }

    for (int i = 0; i < l.size; i++) {
        printf("%s %s : %ld\n",
               bitboard_to_string(get_m_bitboard(l.m[i], 0)),
               bitboard_to_string(get_m_bitboard(l.m[i], 1)),
               results[i]);
    }

    printf("\ntotal moves %d\n", l.size);
    printf("total nodes %ld\n", nodes);

    free(results);
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

    b -> w_en_passant_flag = -1;
    b -> b_en_passant_flag = -1;
    b -> castles = 0;
    b -> fifty_moves = 0;

    int index = 0;
    int col = 7;
    int file = 0;
    int sq;
    int digit;
    char c;

    while ((c = fen[index]) != '\0' && c != ' '){
        if (c == '/'){
            if (file != 8 || col == 0){
                init_board(b, rep);
                return;
            }
            col--;
            file = 0;
            index++;
            continue;
        }

        digit = is_digit(c);
        if (digit >= 1 && digit <= 8){
            if (file + digit > 8){
                init_board(b, rep);
                return;
            }
            file += digit;
            index++;
            continue;
        }

        if (file >= 8){
            init_board(b, rep);
            return;
        }

        sq = col * 8 + file;

        switch (c){
            case 'r':
                b -> pieces[ROOK] |=  (1ULL << sq);
                break;
            case 'n':
                b -> pieces[KNIGHT] |=  (1ULL << sq);
                break;
            case 'b':
                b -> pieces[BISHOP] |=  (1ULL << sq);
                break;
            case 'q':
                b -> pieces[QUEEN] |=  (1ULL << sq);
                break;
            case 'k':
                b -> pieces[KING] |=  (1ULL << sq);
                break;
            case 'p':
                b -> pieces[PAWN] |=  (1ULL << sq);
                break;
            case 'R':
                b -> pieces[ROOK + 6] |=  (1ULL << sq);
                break;
            case 'N':
                b -> pieces[KNIGHT + 6] |=  (1ULL << sq);
                break;
            case 'B':
                b -> pieces[BISHOP + 6] |=  (1ULL << sq);
                break;
            case 'Q':
                b -> pieces[QUEEN + 6] |=  (1ULL << sq);
                break;
            case 'K':
                b -> pieces[KING + 6] |=  (1ULL << sq);
                break;
            case 'P':
                b -> pieces[PAWN + 6] |=  (1ULL << sq);
                break;
            default:
                init_board(b, rep);
                return;
        }

        file++;
        index++;
    }

    if (col != 0 || file != 8 || fen[index] != ' '){
        init_board(b, rep);
        return;
    }

    index++;

    if (fen[index] == 'w')
        b -> turn = WHITE;
    else if (fen[index] == 'b')
        b -> turn = BLACK;
    else{
        init_board(b, rep);
        return;
    }

    index++;
    if (fen[index] != ' '){
        init_board(b, rep);
        return;
    }

    index++;

    if (fen[index] == '-')
        index++;
    else{
        int seen_castle = 0;

        while (fen[index] != '\0' && fen[index] != ' '){
            switch (fen[index]){
                case 'K':
                    if (b -> castles & 1){
                        init_board(b, rep);
                        return;
                    }
                    b -> castles |= 1;
                    break;
                case 'Q':
                    if (b -> castles & 2){
                        init_board(b, rep);
                        return;
                    }
                    b -> castles |= 2;
                    break;
                case 'k':
                    if (b -> castles & 4){
                        init_board(b, rep);
                        return;
                    }
                    b -> castles |= 4;
                    break;
                case 'q':
                    if (b -> castles & 8){
                        init_board(b, rep);
                        return;
                    }
                    b -> castles |= 8;
                    break;
                default:
                    init_board(b, rep);
                    return;
            }
            seen_castle = 1;
            index++;
        }

        if (!seen_castle){
            init_board(b, rep);
            return;
        }
    }

    if (fen[index] != ' '){
        init_board(b, rep);
        return;
    }

    index++;

    if (fen[index] == '-'){
        index++;
    }
    else{
        int col_ep = fen[index] - 'a';
        index++;
        int row = fen[index] - '1';

        if (col_ep < 0 || col_ep > 7 || row < 0 || row > 7){
            init_board(b, rep);
            return;
        }

        if (row == 2)
            b -> w_en_passant_flag = col_ep;
        else if (row == 5)
            b -> b_en_passant_flag = col_ep;
        else{
            init_board(b, rep);
            return;
        }

        index++;
    }

    if (fen[index] == ' '){
        index++;
        if (!is_digit(fen[index])){
            init_board(b, rep);
            return;
        }
        int value = 0;
        while (is_digit(fen[index]) >= 0){
            value = value * 10 + (fen[index] - '0');
            index++;
        }
        b -> fifty_moves = value;

        if (fen[index] == ' '){
            index++;
            if (!is_digit(fen[index])){
                init_board(b, rep);
                return;
            }
            while (is_digit(fen[index]) >= 0)
                index++;
        }
    }

    if (fen[index] != '\0'){
        init_board(b, rep);
        return;
    }

    b -> player_pieces[BLACK] = b -> pieces[PAWN] | b -> pieces[KNIGHT] | b -> pieces[BISHOP] | b -> pieces[ROOK] | b -> pieces[QUEEN] | b -> pieces[KING];
    b -> player_pieces[WHITE] = b -> pieces[PAWN + 6] | b -> pieces[KNIGHT + 6] | b -> pieces[BISHOP + 6] | b -> pieces[ROOK + 6] | b -> pieces[QUEEN + 6] | b -> pieces[KING + 6];
}

void print_board_info(board *b){
    printf("white pieces %lu\n", b -> player_pieces[WHITE]);
    printf("black pieces %lu\n", b -> player_pieces[BLACK]);
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

void verify_logics(int depth, int nb_threads){
    board b;
    rep_struct rep;
    init_board(&b, &rep);
    perft_divide(&b, depth, nb_threads);
    printf("\n\n");
    set_position_2(&b, &rep);
    perft_divide(&b, depth, nb_threads);
    printf("\n\n");
    set_position_3(&b, &rep);
    perft_divide(&b, depth, nb_threads);
    printf("\n\n");
    set_position_4(&b, &rep);
    perft_divide(&b, depth, nb_threads);
    printf("\n\n");
    set_position_5(&b, &rep);
    perft_divide(&b, depth, nb_threads);

}