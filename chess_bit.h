#ifndef MYLIB_H
    #define MYLIB_H


#ifdef __cplusplus
    extern "C" {
#endif


enum {
  CB_A1, CB_B1, CB_C1, CB_D1, CB_E1, CB_F1, CB_G1, CB_H1,
  CB_A2, CB_B2, CB_C2, CB_D2, CB_E2, CB_F2, CB_G2, CB_H2,
  CB_A3, CB_B3, CB_C3, CB_D3, CB_E3, CB_F3, CB_G3, CB_H3,
  CB_A4, CB_B4, CB_C4, CB_D4, CB_E4, CB_F4, CB_G4, CB_H4,
  CB_A5, CB_B5, CB_C5, CB_D5, CB_E5, CB_F5, CB_G5, CB_H5,
  CB_A6, CB_B6, CB_C6, CB_D6, CB_E6, CB_F6, CB_G6, CB_H6,
  CB_A7, CB_B7, CB_C7, CB_D7, CB_E7, CB_F7, CB_G7, CB_H7,
  CB_A8, CB_B8, CB_C8, CB_D8, CB_E8, CB_F8, CB_G8, CB_H8
};

#include <stdint.h>

typedef uint64_t cb_bitboard;

enum {CB_WHITE = 1, CB_BLACK = 0};
enum {CB_ROOK, CB_KNIGHT, CB_BISHOP, CB_QUEEN, CB_KING, CB_PAWN, CB_NO_PROM};


extern const uint64_t CB_MASK_BOARD;

extern const uint64_t CB_ROWS[8];
extern const uint64_t CB_COLUMNS[8];


typedef uint32_t cb_move;

typedef struct{
    int idx;
    int idx_start_looking;
    cb_bitboard rep_table[150];
}cb_rep_struct;

typedef struct{
    uint8_t castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    int8_t w_en_passant_flag;
    int8_t b_en_passant_flag;
    uint8_t fifty_moves;
    cb_move m;
    uint8_t piece_dst;
    uint8_t flags_enP_prom; //1 if en_passant, 2 for rooks promotion, 4 knight, 8 bishop, 16 queen
    uint8_t rep_idx;
}cb_unmake_info;


typedef struct{
    cb_bitboard pieces[12]; //FIRST 6 CB_WHITE : 0 CB_ROOK; 1 CB_KNIGHT; 2 CB_BISHOP ; 3 CB_QUEEN; 4 CB_KING; 5 CB_PAWN
    cb_bitboard player_pieces[2];
    uint8_t turn;
    uint8_t castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    int8_t w_en_passant_flag;
    int8_t b_en_passant_flag;
    uint8_t fifty_moves;
    cb_rep_struct *rep;
}cb_board;


typedef struct{
    cb_move m[240];
    int index;
    int size;
}cb_list_move;

 void cb_init();
 void cb_init_board(cb_board *b, cb_rep_struct *rep);
 char *cb_bitboard_to_board(cb_board *b);
 void cb_print_board(cb_board *b);
 cb_bitboard cb_string_to_bitboard(char *string_square);
 char *cb_bitboard_to_string(cb_bitboard square);
 long cb_get_time_ms();
 void cb_print_moves(cb_list_move *l_moves);
 long cb_perft(cb_board *b, int depth);
 void cb_perft_divide(cb_board *b, int depth);
 void cb_fen_to_board(cb_board *b, cb_rep_struct *rep, char *fen);
 void cb_make_move(cb_board *b, cb_move m, int rep, cb_unmake_info *info);
 int cb_is_attacked(const cb_board *b, cb_bitboard square);
 int cb_is_check(const cb_board *b);
 void cb_legal_moves(cb_board *b, cb_list_move *l);
 void cb_pseudo_legal_moves(const cb_board *b, cb_list_move *l);
 int cb_is_legal_move(const cb_board *b, cb_move m);
 void cb_unmake(cb_board *b, cb_unmake_info *info);
 int cb_game_state(cb_board *b);
 int cb_insufficient_material(const cb_board *b);
 int cb_is_checkmate(cb_board *b);
 int cb_is_draw(cb_board *b);
 cb_bitboard cb_zobrist_key(const cb_board *b);


/*******************************************************************
**************************INLINE FUNCTIONS**************************
********************************************************************/

static inline int cb_get_square_index(cb_bitboard square){
    if (square == 0) 
        return -1;
    return __builtin_ctzll(square);
}

static inline cb_bitboard cb_index_to_bitboard(int idx){
    return 1ULL << idx;
}

static inline int cb_get_row(cb_bitboard square){
    int ind = cb_get_square_index(square);
    return ind / 8;
}

static inline int cb_get_column(cb_bitboard square){
    int ind = cb_get_square_index(square);
    return ind % 8;
}

static inline int cb_on_board(int row, int column){
    return 0 <= row && row < 8 && 0 <= column && column < 8;
}

static inline int cb_is_not_en_passant(const cb_board *b, const cb_bitboard src, const cb_bitboard dst, const int piece){
    return piece != CB_PAWN || src == dst << 8 || src == dst << 16 || src == dst >> 8 || src == dst >> 16 || (dst & (b -> player_pieces[0] | b -> player_pieces[1])) != 0;
}

static inline int cb_is_digit(char c){
    if (c <= '9' && c >= '0') 
        return c - '0';
    return -1;
}

static inline uint32_t cb_create_move(uint32_t src, uint32_t dst, uint32_t piece, uint32_t prom){
    uint32_t m = src;
    m |= dst << 8;
    m |= piece << 16;
    m |= prom << 24;
    return m;
}

static inline uint32_t cb_get_m_int(cb_move m, int idx){
    return (m >> (idx * 8)) & 0xFFu;
}

static inline cb_bitboard cb_get_m_bitboard(cb_move m, int idx){
    uint32_t sq = cb_get_m_int(m, idx);    
    return 1ULL << sq;
}

static const uint32_t CB_MASK_MOVE[4] = {
    0x000000FFu, //src
    0x0000FF00u, //dst
    0x00FF0000u, //piece
    0xFF000000u  //prom
};

static inline uint32_t cb_modify_prom(cb_move m, uint32_t prom){
    cb_move new_m = m;
    new_m &= ~CB_MASK_MOVE[3];
    new_m |= prom << 24;
    return new_m;
}

static inline cb_bitboard cb_splitmix64(cb_bitboard *state) {
    cb_bitboard z = (*state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}


#ifdef MYLIB_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>

const uint64_t CB_MASK_BOARD = 0xFFFFFFFFFFFFFFFFULL;

const uint64_t CB_ROWS[8] = {0xFFULL, 0xFFULL << 8,0xFFULL << 16, 0xFFULL << 24, 0xFFULL << 32, 0xFFULL << 40, 0xFFULL << 48, 0xFFULL << 56};
const uint64_t CB_COLUMNS[8] = {0x0101010101010101ULL, 0x0101010101010101ULL << 1, 0x0101010101010101ULL << 2, 0x0101010101010101ULL << 3,
     0x0101010101010101ULL << 4, 0x0101010101010101ULL << 5, 0x0101010101010101ULL << 6, 0x0101010101010101ULL << 7};

static cb_bitboard east_table[64];
static cb_bitboard west_table[64];
static cb_bitboard south_table[64];
static cb_bitboard north_table[64];
static cb_bitboard knight_table[64];
static cb_bitboard n_east_table[64];
static cb_bitboard n_west_table[64];
static cb_bitboard s_east_table[64];
static cb_bitboard s_west_table[64];
static cb_bitboard kings_table[64];
static cb_bitboard rooks_table[64];
static cb_bitboard bishops_table[64];

static cb_bitboard pos_table[12][64];
static cb_bitboard k, q, K, Q; //k for white king 
static cb_bitboard en_passant_table[8];
static cb_bitboard turn_table;

typedef struct{
    int r;
    int c;
}cb_knight_mov;

static const cb_knight_mov n_mov[8] = {{-2, -1}, {-2, 1}, {2, 1}, {2, -1}, {-1, -2}, {-1, 2}, {1, 2}, {1, -2}};

//Zobrist key implementation
static const cb_bitboard Zobrist_SEED = 0xABCDEFABCDEFABCDULL;



 void cb_init_board(cb_board *b, cb_rep_struct *r){
    memset(b, 0, sizeof(*b));
    b -> turn = CB_WHITE;
    b -> pieces[0] = 0x81ULL;
    b -> pieces[1] = 0X42ULL;
    b -> pieces[2] = 0X24ULL;
    b -> pieces[3] = 0X8ULL;
    b -> pieces[4] = 0X10ULL;
    b -> pieces[5] = CB_ROWS[1];
    for (int i = 6; i < 11; i++)
        b -> pieces[i] = b -> pieces[i - 6] << 56;
    b -> pieces[11] = CB_ROWS[6];
    b -> castles = 1 + 2 + 4 + 8;
    b -> w_en_passant_flag = -1;
    b -> b_en_passant_flag = -1;
    b -> fifty_moves = 0;
    b -> player_pieces[0] = 0;
    b -> player_pieces[1] = 0;
    for (int i = 0; i < 6; i++)
        b -> player_pieces[1] |= b -> pieces[i];
    for (int i = 6; i < 12; i++)
        b -> player_pieces[0] |= b -> pieces[i];
    
    b -> rep = r;
    b -> rep -> idx = 1;
    b -> rep -> idx_start_looking = 0;
    memset(b -> rep -> rep_table, 0, sizeof(cb_bitboard)*150);
    cb_board cpy = *b;
    b -> rep -> rep_table[0] = cb_zobrist_key(&cpy);
}


 char *cb_bitboard_to_board(cb_board *b){
    char *new_board = malloc(65 * sizeof(char));
    if (!new_board)
        return NULL;
    for (int i = 0; i < 64; i++)
        new_board[i] = '.';
    cb_bitboard black_pieces = 0;
    for (int i = 6; i < 12; i++){
        black_pieces |= b -> pieces[i];
    }
    cb_bitboard white_pieces = 0;
    for (int i = 0; i < 6; i++){
        white_pieces |= b -> pieces[i];
    }
    cb_bitboard occupied =  black_pieces | white_pieces;
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

 void cb_print_board(cb_board *b){
    char *board = cb_bitboard_to_board(b);
    for (int i = 7; i >= 0; i--){
        for (int j = 0; j < 8; j++)
            printf("\t%c\t|",board[i * 8 + j]);
        printf("\n");
        printf("__________________________________________________________________________________________________________________________________");
        printf("\n");
    }
}

 cb_bitboard cb_string_to_bitboard(char *string_square){
    char letter = string_square[0];
    int number = (int)(string_square[1] - '0');
    return 1ULL << ((number - 1) * 8 + (letter - 'a'));
}

 char *cb_bitboard_to_string(cb_bitboard square){
    char *tab = malloc(sizeof(char) * 3);
    cb_bitboard copy_square = square;
    int row = 0;
    while ((copy_square = copy_square >> 8) > 0)
        row++;
    cb_bitboard mask_row = CB_ROWS[row];

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

static void cb_print_bitboard_tab(cb_bitboard *tab){
    if (!tab)
        return;
    int i = 0;
    while (tab[i] != 0){
        printf("%d : %s\n",i, cb_bitboard_to_string(tab[i]));
        i++;
    }
}


 long cb_get_time_ms(){
    clock_t time = clock();
    return time * 1000 / CLOCKS_PER_SEC;
}

 void cb_print_moves(cb_list_move *l_moves){
    int i;
    int row;
    int col;
    for (i = 0; i < l_moves -> size; i++){
        row = cb_get_row(cb_get_m_bitboard(l_moves -> m[i], 0));
        col = cb_get_column(cb_get_m_bitboard(l_moves -> m[i], 0));
        printf("%c", (char)('a' + col));
        printf("%c", (char)('1' + row));
        printf("\t");
        row = cb_get_row(cb_get_m_bitboard(l_moves -> m[i], 1));
        col = cb_get_column(cb_get_m_bitboard(l_moves -> m[i], 1));
        printf("%c", (char)('a' + col));
        printf("%c", (char)('1' + row));
        printf("\n");
    }
    printf("size %d\n",l_moves -> size);
}



 long cb_perft(cb_board *b, int depth){
    if (depth == 0)
        return 1;
    long nodes = 0;
    cb_list_move l;
    cb_legal_moves(b, &l);
    cb_unmake_info info;
    if (depth == 1)
        return l.size;
    for (int i = 0; i < l.size; i++){
   
        cb_make_move(b, l.m[i], 0, &info);
        nodes += cb_perft(b, depth - 1);
        cb_unmake(b, &info);
    }
    return nodes;
}

 void cb_perft_divide(cb_board *b, int depth){
    if (depth == 0)
        return;
    long temp_nodes = 0;
    long nodes = 0;
    cb_list_move l;
    cb_unmake_info info;
    cb_legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        cb_make_move(b, l.m[i], 0, &info);
        temp_nodes = cb_perft(b, depth - 1);
        nodes += temp_nodes;
        printf("%s %s : %ld\n", cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 0)), cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 1)), temp_nodes);
        cb_unmake(b, &info);
    }
    printf("\ntotal moves %d\n", l.size);
    printf("total nodes %ld\n", nodes);
}

static long cb_perft_2(cb_board *b, int depth){
    if (depth == 0)
        return 1;
    long nodes = 0;
    cb_list_move l;
    
    cb_unmake_info info;
    if (depth == 1){
        cb_legal_moves(b, &l);
        return l.size;
    }

    cb_pseudo_legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        if (!cb_is_legal_move(b, l.m[i]))
            continue;
        cb_make_move(b, l.m[i], 0, &info);
        nodes += cb_perft_2(b, depth - 1);
        cb_unmake(b, &info);
    }
    return nodes;
}

static void cb_perft_divide_2(cb_board *b, int depth){
    if (depth == 0)
        return;
    long temp_nodes = 0;
    long nodes = 0;
    cb_list_move l;
    cb_unmake_info info;
    cb_pseudo_legal_moves(b, &l);
    for (int i = 0; i < l.size; i++){
        if (!cb_is_legal_move(b, l.m[i]))
            continue;
        cb_make_move(b, l.m[i], 0, &info);
        temp_nodes = cb_perft_2(b, depth - 1);
        nodes += temp_nodes;
        printf("%s %s : %ld\n", cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 0)), cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 1)), temp_nodes);
        cb_unmake(b, &info);
    }
    printf("\ntotal moves %d\n", l.size);
    printf("total nodes %ld\n", nodes);
}

static void cb_set_position_2(cb_board *b, cb_rep_struct *rep){
    cb_fen_to_board(b, rep, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
}

static void cb_set_position_3(cb_board *b, cb_rep_struct *rep) {
    cb_fen_to_board(b, rep, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
}

static void cb_set_position_4(cb_board *b, cb_rep_struct *rep) {
    cb_fen_to_board(b, rep, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");
}

static void cb_set_position_5(cb_board *b, cb_rep_struct *rep) {
    cb_fen_to_board(b, rep, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");
}



 void cb_fen_to_board(cb_board *b, cb_rep_struct *rep, char *fen){
    memset(b, 0, sizeof(*b));
    memset(rep -> rep_table, 0, sizeof(cb_bitboard) * 150);
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
        if ((digit = cb_is_digit(c)) >= 0){
            if (digit == 9 || digit == 0 || temp_count + digit >= 9){
                //Parsing error
                cb_init_board(b, rep);
                return;
            }
            i = 1;
            
            while (fen[index] != '\0' && i  < digit)
                i++;
         
            if (i < digit){
                //Parsing error
                cb_init_board(b, rep);
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
            cb_init_board(b, rep);
            return;
        } 
        switch (c){
            case 'r':
                b -> pieces[CB_ROOK + 6] |=  (1ULL << count);
                break;
            case 'n':
                b -> pieces[CB_KNIGHT + 6] |=  (1ULL << count);
                break;
            case 'b':
                b -> pieces[CB_BISHOP + 6] |=  (1ULL << count);
                break;
            case 'q':
                b -> pieces[CB_QUEEN + 6] |=  (1ULL << count);
                break;
            case 'k':
                b -> pieces[CB_KING + 6] |=  (1ULL << count);
                break;
            case 'p':
                b -> pieces[CB_PAWN + 6] |=  (1ULL << count);
                break;
            case 'R':
                b -> pieces[CB_ROOK] |=  (1ULL << count);
                break;
            case 'N':
                b -> pieces[CB_KNIGHT] |=  (1ULL << count);
                break;
            case 'B':
                b -> pieces[CB_BISHOP] |=  (1ULL << count);
                break;
            case 'Q':
                b -> pieces[CB_QUEEN] |=  (1ULL << count);
                break;
            case 'K':
                b -> pieces[CB_KING] |=  (1ULL << count);
                break;
            case 'P':
                b -> pieces[CB_PAWN ] |=  (1ULL << count);
                break;
            //Parsing error
            cb_init_board(b, rep);
            return;
        } 
        index += 1;
        count++;
    }while (c != ' ' && c != '\0');    

    if (fen[index] == '\0' || fen[index + 1] == '\0'){
        //Parsing error
        cb_init_board(b, rep);
        return;
    }
    index++;
    if (fen[index] == 'w')
        b -> turn = CB_WHITE;
    else
        b -> turn = CB_BLACK;

    if (fen[index + 1] == '\0'){
        //Parsing error
        cb_init_board(b, rep);
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
        cb_init_board(b, rep);
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
        cb_init_board(b, rep);
        return ;
    }
  
    b -> player_pieces[0] = 0;
    b -> player_pieces[1] = 0;
    for (int i = 0; i < 6; i++)
        b -> player_pieces[1] |= b -> pieces[i];
    for (int i = 6; i < 12; i++)
        b -> player_pieces[0] |= b -> pieces[i];
    b -> fifty_moves = 0;
}

static void cb_print_board_info(cb_board *b){
    cb_bitboard white_pieces = 0;
    for (int i = 0; i < 6; i++)
        white_pieces |= b -> pieces[i];
    printf("white pieces %lu\n", white_pieces);
    cb_bitboard black_pieces = 0;
    for (int i = 6; i < 12; i++)
        black_pieces |= b -> pieces[i];
    printf("black pieces %lu\n", black_pieces);
    printf("castles right %u\n", b -> castles);
    printf("turn %d\n", b -> turn);
    printf("w_en_passant %d\n", b -> w_en_passant_flag);
    printf("b_en_passant %d\n", b -> b_en_passant_flag);
    printf("fifty moves %d\n",  b -> fifty_moves);
    
    
}


static void cb_verify_logics(int depth){
    cb_board b;
    cb_rep_struct rep;
    cb_init_board(&b, &rep);
    cb_perft_divide(&b, depth);
    printf("\n\n");
    cb_set_position_2(&b, &rep);
    cb_perft_divide(&b, depth);
    printf("\n\n");
    cb_set_position_3(&b, &rep);
    cb_perft_divide(&b, depth);
    printf("\n\n");
    cb_set_position_4(&b, &rep);
    cb_perft_divide(&b, depth);
    printf("\n\n");
    cb_set_position_5(&b, &rep);
    cb_perft_divide(&b, depth);

}

 void cb_init_zobrist_tables(){
    cb_bitboard s = Zobrist_SEED;
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 64; j++)
            pos_table[i][j] = cb_splitmix64(&s);
    }
    k = cb_splitmix64(&s);
    q = cb_splitmix64(&s);
    K = cb_splitmix64(&s);
    Q = cb_splitmix64(&s);
    for (int i = 0; i < 8; i++)
        en_passant_table[i] = cb_splitmix64(&s);
    turn_table = cb_splitmix64(&s);
}

 void cb_init(){
    cb_init_zobrist_tables();
    memset(east_table, 0, 64 * sizeof(cb_bitboard));
    memset(west_table, 0, 64 * sizeof(cb_bitboard));
    memset(south_table, 0, 64 * sizeof(cb_bitboard));
    memset(north_table, 0, 64 * sizeof(cb_bitboard));

    cb_bitboard square;
    int count;
    for (int i = 0; i < 64; i++){
        square = 1ULL << i;
        count = 1;
        int row = i/8;
        int col = i % 8;
        while (count < 8 && ((square << count) & CB_ROWS[row]) != 0 ){
            east_table[i] |= square << count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> count) & CB_ROWS[row]) != 0 ){
            west_table[i] |= square >> count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square << 8 * count) & CB_COLUMNS[col]) != 0 ){
            north_table[i] |= square << 8 * count;
            count++;
        }
        count = 1;
        while (count < 8 && ((square >> 8 * count) & CB_COLUMNS[col]) != 0 ){
            south_table[i] |= square >> 8 * count;
            count++;
        }
    }

    memset(knight_table, 0, 64 * sizeof(cb_bitboard));
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
            if (!cb_on_board(row + row_mov, col + col_mov)) 
                continue;
            if (row_mov < 0)
                knight_table[i] |= square >> (-row_mov * 8 - col_mov);
            else
                knight_table[i] |= square << (row_mov * 8 + col_mov);
            
        }
    }
    memset(n_east_table, 0, 64 * sizeof(cb_bitboard));
    memset(n_west_table, 0, 64 * sizeof(cb_bitboard));
    memset(s_east_table, 0, 64 * sizeof(cb_bitboard));
    memset(s_west_table, 0, 64 * sizeof(cb_bitboard));
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
    memset(kings_table, 0, 64 * sizeof(cb_bitboard));
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


static int cb_is_attacked_row(const cb_board *b, cb_bitboard square){
    int j = CB_ROOK + 6 * b -> turn;
    cb_bitboard Q_R_MASK = b -> pieces[j] | b -> pieces[3 + j];
    cb_bitboard index;
    cb_bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    cb_bitboard blockers = occupied & east_table[__builtin_ctzll(square)];
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


static int cb_is_attacked_column(const cb_board *b, cb_bitboard square){

    int j = CB_ROOK + 6 * b -> turn;
    cb_bitboard Q_R_MASK = b -> pieces[j] | b -> pieces[3 + j];
    cb_bitboard index;
    cb_bitboard occupied =  b -> player_pieces[0] | b -> player_pieces[1];
    cb_bitboard blockers = occupied & south_table[__builtin_ctzll(square)];
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


static int cb_is_attacked_diagonal(const cb_board *b, cb_bitboard square){
    int j = CB_BISHOP + 6 * b -> turn;
    cb_bitboard Q_B_MASK = b -> pieces[j] | b -> pieces[ 1 + j];
    cb_bitboard index;
    cb_bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    cb_bitboard blockers = occupied & n_east_table[__builtin_ctzll(square)];
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

static int cb_is_attacked_by_knight(const cb_board *b, cb_bitboard square){
    cb_bitboard opp = b -> pieces[CB_KNIGHT + 6 * b -> turn];
    return (opp & knight_table[__builtin_ctzll(square)]) > 0;
}

static int cb_is_attacked_pawn(const cb_board *b, cb_bitboard square){
    int row = cb_get_row(square);
    int column = cb_get_column(square);
    if (b -> turn == CB_WHITE)
        return ((cb_on_board(row + 1 ,column + 1) &&  (((square << 9) & b -> pieces[CB_PAWN + 6]) > 0))) || ((cb_on_board(row +1 ,column - 1) &&  (((square << 7) & b -> pieces[CB_PAWN + 6]) > 0)));
    return ((cb_on_board(row - 1 ,column + 1) &&  (((square >> 7) & b -> pieces[CB_PAWN]) > 0))) || ((cb_on_board(row - 1 ,column - 1) &&  (((square >> 9) & b -> pieces[CB_PAWN]) > 0)));
}     

static int cb_is_attacked_king(const cb_board *b, cb_bitboard square){
    cb_bitboard opp_king = b -> pieces[CB_KING + 6 * b -> turn];
    return (opp_king & kings_table[__builtin_ctzll(square)]) > 0;
}

 int cb_is_attacked(const cb_board *b, cb_bitboard square){
    return cb_is_attacked_column(b, square) || cb_is_attacked_diagonal(b, square) || 
        cb_is_attacked_row(b, square) || cb_is_attacked_by_knight(b, square) || cb_is_attacked_pawn(b, square) || cb_is_attacked_king(b, square);
}

 void cb_make_move(cb_board *b, cb_move m, int rep, cb_unmake_info *info){
    cb_bitboard src = cb_get_m_bitboard(m, 0);
    cb_bitboard dst = cb_get_m_bitboard(m, 1);
    uint32_t promotion = cb_get_m_int(m, 3);
    //info for unmake function, info = NULL if you don't want to use unmake func
    if (info){
        info -> castles = b -> castles;
        info -> w_en_passant_flag = b -> w_en_passant_flag;
        info -> b_en_passant_flag = b -> b_en_passant_flag;
        info -> fifty_moves = b -> fifty_moves;
        info -> m = m;
        info -> piece_dst = 7; //Set to 7 while we don't know if there's a piece at cb_src(CB_ROOKs to pawns are between 0 and 5 include)
        info  -> flags_enP_prom = 0; 
        if (rep)
            info -> rep_idx = b -> rep -> idx_start_looking;
        else
            info -> rep_idx = 255;
    }
    b -> fifty_moves++;
    cb_bitboard occupied =  b -> player_pieces[0] | b -> player_pieces[1];
    if (b -> turn)
        b -> w_en_passant_flag = -1;
    else
        b -> b_en_passant_flag = -1;
    uint32_t i = cb_get_m_int(m, 2) + 6 * (b -> turn ^ 1);
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

    if (i == CB_KING || i == CB_PAWN ||i == CB_KING + 6 || i == CB_PAWN + 6){
        if (i == CB_PAWN || i == CB_PAWN + 6){
            if (rep)
                b -> rep -> idx_start_looking = b -> rep ->idx;
            if ((src & CB_ROWS[(b -> turn ^ 1) * 5 + 1]) > 0 && dst & CB_ROWS[(b -> turn ^ 1) * 1  + 3]){ // If we cb_move from 2nd row to 4th or 7th to 5th (en_passant)
                if (b -> turn)
                    b -> w_en_passant_flag = cb_get_column(src);
                else
                    b -> b_en_passant_flag = cb_get_column(src);
            }
   
            b -> pieces[CB_PAWN + (b -> turn ^ 1) * 6] &= ~src;
            b -> player_pieces[b -> turn] &= ~src;
            b -> pieces[CB_PAWN + (b -> turn ^ 1) * 6] |= dst;
            b -> player_pieces[b -> turn] |= dst;
            if (dst & occupied){
                b -> player_pieces[b -> turn ^ 1] &= ~dst; 
                b -> pieces[opp_piece] &= ~dst;
            }
            
            if (b -> turn && ((dst & ((src << 9) | (src << 7))) > 0) && (dst & occupied) == 0){ //We're en passening with whites
                if (info)
                    info  -> flags_enP_prom |= 1; //info end here
                b -> pieces[CB_PAWN + 6] &= ~(dst >> 8);
                b -> player_pieces[0] &= ~(dst >> 8);
            } 
            if (b -> turn == 0 && ((dst & ((src >> 9) | (src >> 7))) > 0) && (dst & occupied) == 0){ //We're en passening with black
                if (info)
                    info  -> flags_enP_prom |= 1; //info end here
                b -> pieces[CB_PAWN ] &= ~(dst << 8);
                b -> player_pieces[1] &= ~(dst << 8);
            } 
            if (b -> turn && (src & CB_ROWS[6]) > 0){ //white promotion part
                b -> pieces[CB_PAWN + (b -> turn ^ 1) * 6] &= ~dst; // We delete the pawn that we added 10 lines before
                if (promotion == CB_ROOK){
                    b -> pieces[CB_ROOK] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 2;
                }
                else if (promotion == CB_KNIGHT){
                    b -> pieces[CB_KNIGHT] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 4;
                }
                else if (promotion == CB_BISHOP){
                    b -> pieces[CB_BISHOP] |= dst;
                    b -> player_pieces[1] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 8;
                }

                else{
                    b -> pieces[CB_QUEEN] |= dst;
                    b -> player_pieces[1] |= dst;
                      if (info)
                        info -> flags_enP_prom |= 16;
                }
            }
            if (b -> turn == 0 && (src & CB_ROWS[1]) > 0){ //black promotion part
                b -> pieces[CB_PAWN + (b -> turn ^ 1) * 6] &= ~dst; // We delete the pawn that we added 10 lines before
                if (promotion == CB_ROOK){
                    b -> pieces[CB_ROOK + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 2;
                }
                else if (promotion == CB_KNIGHT){
                    b -> pieces[CB_KNIGHT + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 4;
                }
                else if (promotion == CB_BISHOP){
                    b -> pieces[CB_BISHOP + 6] |= dst;
                    b -> player_pieces[0] |= dst;
                    if (info)
                        info -> flags_enP_prom |= 8;
                }
                else{
                    b -> pieces[CB_QUEEN + 6] |= dst;
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
    if (i == CB_ROOK){ // We delete rook castle right
        if (src == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (src == (1ULL << 7))
            b -> castles &= ~1;
    }
    if (i == CB_ROOK + 6){
        if (src == (1ULL << 56)) //We can't long castle with blacks
            b -> castles &= ~8;
        if (src == (1ULL << 63))
            b -> castles &= ~4;
    }
    if ((dst & occupied ) > 0 && opp_piece == CB_ROOK){ // We delete a rook castle right
        if (dst == 1ULL) //We can't long castle with whites
            b -> castles &= ~2;
        if (dst == (1ULL << 7))
            b -> castles &= ~1;
    }
    if ((dst & occupied ) > 0 && opp_piece == CB_ROOK + 6){
        if (dst == (1ULL << 56)) //We can't long castle with blacks
            b -> castles &= ~8;
        if (dst == (1ULL << 63))
            b -> castles &= ~4;
    }
    if (rep){
        (b -> rep) -> rep_table[(b -> rep) -> idx] = cb_zobrist_key(b);
        if(b -> rep -> idx >= 149)
            b -> rep -> idx = 0;
        else
            (b -> rep -> idx)++;
    }
    b -> turn ^= 1; 
}  

 int cb_is_check(const cb_board *b){
    cb_bitboard square = b -> pieces[CB_KING + 6 * (b -> turn ^ 1)];
    return cb_is_attacked(b,square);
}

static void cb_rook_moves(const cb_board *b, cb_bitboard square, cb_list_move *l, uint32_t piece){
    cb_bitboard my_piece = b -> player_pieces[1];
    cb_bitboard opp_piece = b -> player_pieces[0];
    if (b -> turn == CB_BLACK){
        my_piece = b -> player_pieces[0];
        opp_piece = b -> player_pieces[1];
    }
    int row = cb_get_row(square);
    int column = cb_get_column(square );
    cb_bitboard mask_row = CB_ROWS[row];
    cb_bitboard mask_column = CB_COLUMNS[column];

    int i = 1;
    while(((square << i) & mask_row) > 0){ //checking right rows 
        if ((my_piece & square << i) > 0)
            break;
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << i), piece, CB_NO_PROM); 
        (l -> index)++;
        if ((opp_piece & square << i) > 0)
            break;
        i++;
    }

    i = 1;
    while(((square >> i) & mask_row) > 0){ //checking left rows 
        if ((my_piece & square >> i) > 0)
            break;

        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> i), piece, CB_NO_PROM); 
        (l -> index)++;

        if ((opp_piece & square >> i) > 0)
            break;
        i++;
    }

    i = 1;
    while(((square << 8*i) & mask_column ) > 0){ //checking up 
        if ((my_piece & square << 8* i) > 0)
            break;
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i)), piece, CB_NO_PROM); 

        (l -> index)++;
        if ((opp_piece & square << 8 * i) > 0)
            break;
        i++;
    }
    
    i = 1;
    while(((square >> 8*i) & mask_column) > 0){ //checking  down
        if ((my_piece & square >> 8* i) > 0)
            break;
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i)), piece, CB_NO_PROM); 
        (l -> index)++;
        if ((opp_piece & square >> 8 * i) > 0)
            break;
        i++;
    }
}


static void cb_bishop_moves(const cb_board *b, cb_bitboard square, cb_list_move *l, uint32_t piece){
    cb_bitboard my_piece = b -> player_pieces[1];
    cb_bitboard opp_piece = b -> player_pieces[0];
    if (b -> turn == CB_BLACK){
        my_piece = b -> player_pieces[0];
        opp_piece = b -> player_pieces[1];
    }
    
    int row = cb_get_row(square);
    int column = cb_get_column(square);

    int i = 1;
    int j = 1;
   
    while((square << (8*i + j)  > 0) && (row + i < 8 ) && (column + j < 8)){ //checking right_up diag
        if ((my_piece & square << (8*i + j)) > 0)
            break;
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i + j)), piece, CB_NO_PROM); 
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
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i + j)), piece, CB_NO_PROM); 
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
 
        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << (8 * i - j)), piece, CB_NO_PROM); 
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

        l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> (8 * i - j)), piece, CB_NO_PROM); 
        (l -> index)++;
        
        if ((opp_piece & square >> (8 * i - j)) > 0)
            break;
        i++;
        j++;
    }
    
}

static void cb_queen_moves(const cb_board *b, cb_bitboard square, cb_list_move *l){
    cb_rook_moves(b, square, l, CB_QUEEN);
    cb_bishop_moves(b, square, l, CB_QUEEN);
}

static void cb_knight_moves(const cb_board *b, cb_bitboard square, cb_list_move *l){
    int square_idx = __builtin_ctzll(square);
    cb_bitboard all_moves = knight_table[square_idx] & ~(b -> player_pieces[b -> turn]);
    int trailling_zeros;
    while (all_moves){
        trailling_zeros = __builtin_ctzll(all_moves);
        l -> m[l -> index] = cb_create_move(square_idx,trailling_zeros, CB_KNIGHT, CB_NO_PROM); 
        (l -> index)++;
        all_moves &= (all_moves -1);
    }
}

static void cb_king_moves(const cb_board *b, cb_list_move *l){
    cb_bitboard square_idx = __builtin_ctzll(b -> pieces[CB_KING + (b -> turn ^ 1) * 6]);
    cb_bitboard to_add = kings_table[square_idx] & ~b -> player_pieces[b -> turn];
    int trailling_zeros;
    while (to_add){
        trailling_zeros = __builtin_ctzll(to_add);
        l -> m[l -> index] = cb_create_move(square_idx, trailling_zeros, CB_KING, CB_NO_PROM); 
        (l -> index)++;
        to_add &= (to_add - 1);
    }
    cb_bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    if (b -> turn == CB_WHITE && (b -> castles & 1)){ //white can castle
        if ((~occupied & 32) && (~occupied & 64)){
            l -> m[l -> index] = cb_create_move(square_idx, 6, CB_KING, CB_NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == CB_WHITE && (b -> castles & 2)){ //white can long castle
        if ((~occupied & 8) && (~occupied & 4) && (~occupied & 2)){
            l -> m[l -> index] = cb_create_move(square_idx, 2, CB_KING, CB_NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == CB_BLACK && (b -> castles & 4)){ //black can castle
        if ((~occupied & (1ULL << 61)) && (~occupied & (1ULL << 62))){
            l -> m[l -> index] = cb_create_move(square_idx, 62, CB_KING, CB_NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == CB_BLACK && (b -> castles & 8)){ //black can long castle
        if ((~occupied & (1ULL << 59)) && (~occupied & (1ULL << 58)) && (~occupied & (1ULL << 57))){
            l -> m[l -> index] = cb_create_move(square_idx, 58, CB_KING, CB_NO_PROM); 
            (l -> index)++;
        }
    }

}

static void cb_pawn_moves(const cb_board *b, cb_bitboard square, cb_list_move *l){
    int row = cb_get_row(square);
    int col = cb_get_column(square);
    cb_bitboard mask_row = CB_ROWS[1];
    cb_bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    cb_bitboard opp = b -> player_pieces[0];
    if (b -> turn == CB_BLACK){
        mask_row = CB_ROWS[6];
        opp = b -> player_pieces[1];
    }
    if (b -> turn == CB_WHITE){
        if (((square << 8) & ~occupied) > 0){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 8), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        
            if (((square & mask_row) > 0) && ((square << 16) & ~occupied) > 0){
                l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 16), CB_PAWN, CB_NO_PROM); 
                (l -> index)++;
            }
        }
        if ((((square << 9) & opp)  > 0 ) && (row + 1 < 8) && (col + 1 < 8)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 9), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
            
        }
        if ((((square << 7) & opp)  > 0 ) && (row + 1 < 8) && (col - 1 >= 0)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 7), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
        if (4 == row && (b -> b_en_passant_flag == col + 1 ) && (col + 1 < 8)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 9), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
        if (4 == row && ((b -> b_en_passant_flag == col - 1) && (col - 1 >= 0))){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square << 7), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
    }
    if (b -> turn == CB_BLACK){
        if ((square >> 8  & ~occupied) > 0){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 8), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        
            if (((square & mask_row) > 0) && ((square >> 16) & ~occupied) > 0){
                l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 16), CB_PAWN, CB_NO_PROM); 
                (l -> index)++;
            }
        }
        if ((((square >> 9) & opp)  > 0 ) && (row - 1 >= 0) && (col - 1 >= 0)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 9), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
        if ((((square >> 7) & opp)  > 0 ) && (row - 1 >= 0) && (col + 1 < 8)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 7), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
        if (3 == row && (b -> w_en_passant_flag == col + 1 ) && (col + 1 < 8)){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 7), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
        if (3 == row && ((b -> w_en_passant_flag == col - 1) && (col - 1 >= 0))){
            l -> m[l -> index] = cb_create_move(__builtin_ctzll(square), __builtin_ctzll(square >> 9), CB_PAWN, CB_NO_PROM); 
            (l -> index)++;
        }
    } 
}

static void cb_rook_all_moves(const cb_board *b, cb_list_move *l){
    int i = 0;
    if (b -> turn == CB_BLACK)
        i += 6; 
    cb_bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        cb_rook_moves(b, 1ULL << trailing_zeros, l, CB_ROOK);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    } 
}


static void cb_bishop_all_moves(const cb_board *b, cb_list_move *l){
    int i = 2;
    if (b -> turn == CB_BLACK)
        i += 6;
    cb_bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        cb_bishop_moves(b, 1ULL << trailing_zeros, l, CB_BISHOP);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }     
}

static void cb_knight_all_moves(const cb_board *b, cb_list_move *l){
    int i = 1;
    if (b -> turn == CB_BLACK)
        i += 6;
    cb_bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        cb_knight_moves(b, 1ULL << trailing_zeros, l);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }       
}

static void cb_pawn_all_moves(const cb_board *b, cb_list_move *l){
    int i = 5;
    if (b -> turn == CB_BLACK)
        i += 6;
    cb_bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    int index = l -> index;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); 
        cb_pawn_moves(b, 1ULL << trailing_zeros, l);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }

    int j = 0;

    for (int i = index; i < l -> index; i++){
        if((cb_get_m_bitboard(l -> m[i], 0) & CB_ROWS[6] && b -> turn) > 0 || (cb_get_m_bitboard(l -> m[i], 0)& CB_ROWS[1] && (b -> turn ^1)) > 0){
            l -> m[i] = cb_modify_prom(l ->m[i], CB_QUEEN);
            l -> m[l -> index + j] = cb_modify_prom(l ->m[i], CB_ROOK);
            l -> m[l -> index + j + 1] = cb_modify_prom(l ->m[i], CB_BISHOP);
            l -> m[l -> index + j + 2] = cb_modify_prom(l ->m[i], CB_KNIGHT);
            j += 3;
            
        }
    }
    
    l -> index += j;
}

static void cb_queen_all_moves(const cb_board *b, cb_list_move *l){
    int i = 3;
    if (b -> turn == CB_BLACK)
        i += 6;
    cb_bitboard copy_mask = (b -> pieces)[i];
    int trailing_zeros;
    while (copy_mask > 0){
        trailing_zeros = __builtin_ctzll(copy_mask); //This function returns the index of the lowest bit = 1
        cb_queen_moves(b, 1ULL << trailing_zeros, l);
        copy_mask = copy_mask & ~(1ULL << trailing_zeros);
    }     
}

 void cb_pseudo_legal_moves(const cb_board *b, cb_list_move *l){
    l -> index = 0;
    cb_rook_all_moves(b, l);
    cb_bishop_all_moves(b, l);
    cb_knight_all_moves(b, l);
    cb_pawn_all_moves(b, l);
    cb_queen_all_moves(b, l);
    cb_king_moves(b, l);    
    l -> size = l -> index;  
    l -> index = 0;
}

static cb_bitboard cb_get_pinned(const cb_board *b);

 void cb_legal_moves(cb_board *b, cb_list_move *l){
    cb_pseudo_legal_moves(b, l);
    
    if (l -> size == 0)
        return;
    int new_ind = 0;
    cb_unmake_info info;
    cb_bitboard pinned = cb_get_pinned(b);
    int is_check_value = cb_is_check(b);
    for (int index = 0 ; index < l -> size; index++){
        if ((cb_get_m_bitboard(l -> m[index], 0) & pinned) == 0 && is_check_value == 0 && cb_get_m_int(l -> m[index], 2) != CB_KING && 
            cb_is_not_en_passant(b, cb_get_m_bitboard(l -> m[index], 0), cb_get_m_bitboard(l -> m[index], 1), cb_get_m_int(l -> m[index], 2))){
            l -> m[new_ind] = l -> m[index];
            new_ind++;
        }
        else{
            if (b -> pieces[CB_KING] == cb_get_m_bitboard(l -> m[index], 0) ||  b -> pieces[CB_KING + 6] == cb_get_m_bitboard(l -> m[index], 0)){
                if((cb_get_m_bitboard(l -> m[index], 0) << 2) == cb_get_m_bitboard(l -> m[index], 1) || 
                    (cb_get_m_bitboard(l -> m[index], 0) >> 2) == cb_get_m_bitboard(l -> m[index], 1)){
                    if (cb_is_check(b))
                        continue;
                    if ((cb_get_m_bitboard(l -> m[index], 0) << 2) == cb_get_m_bitboard(l -> m[index], 1)){
                        if (cb_is_attacked(b, cb_get_m_bitboard(l -> m[index], 0) << 1) || cb_is_attacked(b, cb_get_m_bitboard(l -> m[index], 0) << 2))
                            continue;
                        l -> m[new_ind] = l -> m[index];
                        new_ind++;
                        continue;
                    }
                    else {
                        if (cb_is_attacked(b, cb_get_m_bitboard(l -> m[index], 0) >> 1) || cb_is_attacked(b, cb_get_m_bitboard(l -> m[index], 0) >> 2))
                            continue;
                        l -> m[new_ind] = l -> m[index];
                        new_ind++;
                        continue;
                    }
                }
            }
            cb_make_move(b, l -> m[index], 0, &info);
    
            b -> turn ^= 1;

            if (!cb_is_check(b)){
                l -> m[new_ind] = l -> m[index];
                new_ind++;
            }
            b -> turn ^=1;
            cb_unmake(b, &info);
        }
    }
    l -> size = new_ind;
}

 int cb_is_legal_move(const cb_board *b, cb_move m){
    cb_board temp = *b;
    cb_bitboard src;
    cb_bitboard dst1;
    cb_bitboard dst2;
    if (cb_get_m_int(m, 3) == CB_KING && (((src = cb_get_m_bitboard(m, 0)) == (dst1 = cb_get_m_bitboard(m, 1) >> 2)) || (src == (dst2 = cb_get_m_bitboard(m, 1) << 2)))){
        if (cb_is_check(b))
            return 0;
        if ((src << 2) == dst1){
            if (cb_is_attacked(b, (b -> pieces[CB_KING + 6 * (b -> turn ^ 1)]) << 1) || cb_is_attacked(b, (b -> pieces[CB_KING + 6 * (b -> turn ^ 1)]) << 2))
                return 0;
            return 1;
        }
        else {
            if (cb_is_attacked(b, (b -> pieces[CB_KING + 6 * (b -> turn ^ 1)]) >> 1) || cb_is_attacked(b, (b -> pieces[CB_KING + 6 * (b -> turn ^ 1)]) >> 2))
                return 0;
            return 1;
        }
    }
    cb_make_move(&temp, m, 0, NULL);
    temp.turn ^= 1;
    return !cb_is_check(&temp);
}

 void cb_unmake(cb_board *b, cb_unmake_info *info){
    cb_bitboard src = cb_get_m_bitboard(info -> m, 0);
    cb_bitboard dst = cb_get_m_bitboard(info -> m, 1);
    uint32_t piece_src = cb_get_m_int(info -> m, 2);
    //Castles part
    if ((b -> pieces[CB_KING] & dst) > 0 && (src << 2) == dst ){ //White castle
        b -> pieces[CB_ROOK] |= (1ULL << 7);
        b -> pieces[CB_ROOK] &= ~(1ULL << 5);
    }
    else if ((b -> pieces[CB_KING] & dst) > 0 && (src >> 2) == dst ){ //White long castle
        b -> pieces[CB_ROOK] |= (1ULL << 0);
        b -> pieces[CB_ROOK] &= ~(1ULL << 3);
    }
    else if ((b -> pieces[CB_KING + 6] & dst) > 0 && (src << 2) == dst ){ //black castle
        b -> pieces[CB_ROOK + 6] |= (1ULL << 63);
        b -> pieces[CB_ROOK + 6] &= ~(1ULL << 61);
    }
    else if ((b -> pieces[CB_KING + 6] & dst) > 0 && (src >> 2) == dst ){ //black long castle
        b -> pieces[CB_ROOK + 6] |= (1ULL << 56);
        b -> pieces[CB_ROOK + 6] &= ~(1ULL << 59);
    }
    //putting the original src piece back 
    b -> pieces[ piece_src + 6 * b -> turn] |= src;
    if (info -> flags_enP_prom <= 1) //No promotions happened
        b -> pieces[piece_src + 6 * b -> turn] &= ~(dst);
    else
        b -> pieces[ __builtin_ctzll((info -> flags_enP_prom ) >> 1) + 6 * b -> turn] &= ~(dst); //if prom happened
    
    //if captures cb_happened(en_passant doesn't verify this condition)
    if (info -> piece_dst != 7)
        b -> pieces[info -> piece_dst + 6 * (b -> turn == CB_BLACK)] |= dst; 
    if ((info -> flags_enP_prom & 1) == 1){  //en passant
        if (b -> turn == CB_WHITE)
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


static int cb_is_aligned(const cb_bitboard king_square, const cb_bitboard piece_square){
    int piece_index = __builtin_ctzll(piece_square);
    return ((s_east_table[piece_index] | s_west_table[piece_index] | n_east_table[piece_index] | n_west_table[piece_index] |
        east_table[piece_index] | west_table[piece_index] | north_table[piece_index] | south_table[piece_index]) & king_square) > 0;
    
}

static cb_bitboard cb_get_pinned(const cb_board *b){
    cb_bitboard occupied = b -> player_pieces[0] | b -> player_pieces[1];
    cb_bitboard my_piece = b -> player_pieces[b -> turn];
    cb_bitboard opp_piece;
    cb_bitboard king_square = b -> pieces[CB_KING + 6 - 6 * b -> turn];
    int index = __builtin_ctzll(king_square);
    cb_bitboard pinned = 0;
    cb_bitboard temp;
    cb_bitboard temp2;
    cb_bitboard blockers;
    // First rows
    if ((blockers = occupied & west_table[index])){
        temp = 1ULL << (63 - __builtin_clzll(blockers));
        if (my_piece & temp){
            opp_piece = b -> pieces[CB_ROOK + 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_ROOK + 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_ROOK + 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_ROOK + 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_BISHOP+ 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_BISHOP+ 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_BISHOP+ 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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
            opp_piece = b -> pieces[CB_BISHOP+ 6 * b -> turn] | b -> pieces[CB_QUEEN + 6 * b -> turn];
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

 int cb_is_repetition(const cb_board *b){
    //Saying if the last cb_move caused draw
    int last = (b -> rep->idx == 0) ? 149 : (b -> rep -> idx - 1);
    cb_bitboard last_move = (b -> rep) -> rep_table[last];
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


 int cb_insufficient_material(const cb_board *b){
    int w = __builtin_popcountll(b -> player_pieces[0]);
    int bl = __builtin_popcountll(b -> player_pieces[1]);

    // K vs K
    if (w == 1 && bl == 1) 
        return 1;

    // K+(bishop or knight) vs K
    if (w == 2 && bl == 1 && (b -> pieces[CB_BISHOP] || b -> pieces[CB_KNIGHT])) 
        return 1;
    if (bl == 2 && w == 1 && (b -> pieces[CB_BISHOP + 6] || b -> pieces[CB_KNIGHT + 6]))
         return 1;

    // K+(bishop or knight) vs K+(bishop or knight)
    if (w == 2 && bl == 2 &&
        (b->pieces[CB_BISHOP] || b->pieces[CB_KNIGHT]) &&
        (b->pieces[CB_BISHOP + 6] || b->pieces[CB_KNIGHT + 6])) 
        return 1;
    return 0;
}

 int cb_is_checkmate(cb_board *b){
    cb_list_move l;
    cb_legal_moves(b , &l);
    if (l.size == 0){
        if (cb_is_check(b))
            return 1;
    }
    return 0;
}

 int  cb_is_draw(cb_board *b){
    if (b->fifty_moves >= 150)
        return 1;
    if (cb_is_repetition(b))
        return 1;
    if (cb_insufficient_material(b))
        return 1;
    cb_list_move l;
    cb_legal_moves(b , &l);
    if (l.size == 0){
        if (cb_is_check(b))
            return 0;
        return 1;
    }
    return 0;
}

 int cb_game_state(cb_board *b){
    /*0 for draw and 1 for win, 2 for nothing */
    if (b->fifty_moves >= 150)
        return 0;
    cb_list_move l;
    cb_legal_moves(b, &l);
    if (l.size == 0){
        if (cb_is_check(b))
            return 1;
        return 0;
    }
    if (cb_is_repetition(b))
        return 0;
    if (cb_insufficient_material(b))
        return 0;
    return 2;
}


 cb_bitboard cb_zobrist_key(const cb_board *b){
    cb_bitboard hash = 0;
    int trailling_zeros;
    cb_bitboard copy_board;
    for (int piece = 0; piece < 12; piece++){
        copy_board = b -> pieces[piece];
        while (copy_board > 0){
            trailling_zeros = __builtin_ctzll(copy_board);
            hash ^= pos_table[piece][trailling_zeros];
            
            copy_board &= ~(1ULL << trailling_zeros);
        }
            
    }
    if (b -> castles & 1)
        hash ^= k;
    if (b -> castles & 2) 
        hash ^= q;
    if (b -> castles & 4)
        hash ^= K;
    if (b -> castles & 8)
        hash ^= Q;
    if (b -> w_en_passant_flag >= 0)
        hash ^= en_passant_table[b -> w_en_passant_flag];
    if (b -> b_en_passant_flag >= 0)
        hash ^= en_passant_table[b -> b_en_passant_flag];
    if (b -> turn == CB_WHITE)
        hash ^= turn_table;

    return hash;
}

#endif

#ifdef __cplusplus
} 
#endif

#endif

