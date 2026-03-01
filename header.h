#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <string.h>


typedef uint64_t bitboard;

extern bitboard east_table[64];
extern bitboard west_table[64];
extern bitboard south_table[64];
extern bitboard north_table[64];
extern bitboard knight_table[64];
extern bitboard n_east_table[64];
extern bitboard n_west_table[64];
extern bitboard s_east_table[64];
extern bitboard s_west_table[64];
extern bitboard kings_table[64];
extern bitboard rooks_table[64];
extern bitboard bishops_table[64];


enum {WHITE = 1, BLACK = 0};
enum {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, NO_PROM};


static const uint64_t MASK_BOARD = 0xFFFFFFFFFFFFFFFFULL;

static const uint64_t ROWS[8] = {0xFFULL, 0xFFULL << 8,0xFFULL << 16, 0xFFULL << 24, 0xFFULL << 32, 0xFFULL << 40, 0xFFULL << 48, 0xFFULL << 56};
static const uint64_t COLUMNS[8] = {0x0101010101010101ULL, 0x0101010101010101ULL << 1, 0x0101010101010101ULL << 2, 0x0101010101010101ULL << 3,
     0x0101010101010101ULL << 4, 0x0101010101010101ULL << 5, 0x0101010101010101ULL << 6, 0x0101010101010101ULL << 7};


typedef uint32_t move;

typedef struct{
    int idx;
    int idx_start_looking;
    bitboard rep_table[150];
}rep_struct;

typedef struct{
    bitboard pieces[12]; //FIRST 6 WHITE : 0 ROOK; 1 KNIGHT; 2 BISHOP ; 3 QUEEN; 4 KING; 5 PAWN
    bitboard player_pieces[2];
    uint8_t turn;
    uint8_t castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    int8_t w_en_passant_flag;
    int8_t b_en_passant_flag;
    uint8_t fifty_moves;
    rep_struct *rep;
}board;

typedef struct {
    uint8_t castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    int8_t w_en_passant_flag;
    int8_t b_en_passant_flag;
    uint8_t fifty_moves;
    move m;
    uint8_t piece_dst;
    uint8_t flags_enP_prom; //1 if en_passant, 2 for rooks promotion, 4 knight, 8 bishop, 16 queen
    uint8_t rep_idx;
}unmake_info;

typedef struct{
    int r;
    int c;
}knight_mov;


typedef struct{
    move m[240];
    int index;
    int size;
}list_move;

static const knight_mov n_mov[8] = {{-2, -1}, {-2, 1}, {2, 1}, {2, -1}, {-1, -2}, {-1, 2}, {1, 2}, {1, -2}};

//Zobrist key implementation
static const bitboard Zobrist_SEED = 0xABCDEFABCDEFABCDULL;


extern bitboard pos_table[12][64];
extern bitboard k, q, K, Q; //k for white king 
extern bitboard en_passant_table[8];
extern bitboard turn_table;


//utils
void init_board(board *b, rep_struct *rep);
char *bitboard_to_board(board *b);
void print_board(board *b);
bitboard string_to_bitboard(char *string_square);
void print_bitboard_tab(bitboard *tab);
char *bitboard_to_string(bitboard square);
long get_time_ms();
void print_moves(list_move *l_moves);
void set_position_2(board *b, rep_struct *rep);
void set_position_3(board *b, rep_struct *rep);
void set_position_4(board *b, rep_struct *rep);
void set_position_5(board *b, rep_struct *rep);
long perft(board *b, int depth);
void perft_divide(board *b, int depth);
void perft_divide_2(board *b, int depth);
void fen_to_board(board *b, rep_struct *rep, char *fen);
long perft_2(board *b, int depth);
void print_board_info(board *b);
void free_board(board *b);
void verify_logics(int depth);

//logics
void init();
void make_move(board *b, move m, int rep, unmake_info *info);
int is_attacked_row(const board *b, bitboard square);
int is_attacked_column(const board *b, bitboard square);
int is_attacked_diagonal(const board *b, bitboard square);
int is_attacked_by_knight(const board *b, bitboard square);
int is_attacked_pawn(const board *b, bitboard square);
int is_attacked(const board *b, bitboard square);
int is_attacked_king(const board *b, bitboard square);
int is_check(const board *b);
void legal_moves(board *b, list_move *l);
void rook_moves(const board *b, bitboard square, list_move *l, uint32_t piece);
void bishop_moves(const board *b, bitboard square, list_move *l, uint32_t piece);
void queen_moves(const board *b, bitboard square, list_move *l);
void king_moves(const board *b, list_move *l);
void knight_moves(const board *b, bitboard square, list_move *l);
void pawn_all_moves(const board *b, list_move *l);
void queen_all_moves(const board *b, list_move *l);
void pseudo_legal_moves(const board *b, list_move *l);
int is_legal_move(const board *b, move m);
void unmake(board *b, unmake_info *info);
int is_aligned(const bitboard king_square, const bitboard piece_square);
bitboard get_pinned(const board *b);
int game_state(board *b);
int insufficient_material(const board *b);
int is_checkmate(board *b);
int is_draw(board *b);

//Zobrist
void init_zobrist_tables();
bitboard zobrist_key(const board *b);


/*******************************************************************
**************************INLINE FUNCTIONS**************************
********************************************************************/

static inline int pop_inplace(bitboard *square){
    int trailling_zeros = __builtin_ctzll(*square);
    *square &= *square - 1;
    return trailling_zeros;
}

static inline int get_square_index(bitboard square){
    if (square == 0) 
        return -1;
    return __builtin_ctzll(square);
}

static inline bitboard index_to_bitboard(int idx){
    return 1ULL << idx;
}

static inline int get_row(bitboard square){
    int ind = get_square_index(square);
    return ind / 8;
}

static inline int get_column(bitboard square){
    int ind = get_square_index(square);
    return ind % 8;
}

static inline int on_board(int row, int column){
    return 0 <= row && row < 8 && 0 <= column && column < 8;
}

static inline int is_not_en_passant(const board *b, const bitboard src, const bitboard dst, const int piece){
    return piece != PAWN || src == dst << 8 || src == dst << 16 || src == dst >> 8 || src == dst >> 16 || (dst & (b -> player_pieces[0] | b -> player_pieces[1])) != 0;
}

static const uint32_t MASK_MOVE[4] = {
    0x000000FFu, //src
    0x0000FF00u, //dst
    0x00FF0000u, //piece
    0xFF000000u  //prom
};

static inline uint32_t create_move(uint32_t src, uint32_t dst, uint32_t piece, uint32_t prom){
    uint32_t m = src;
    m |= dst << 8;
    m |= piece << 16;
    m |= prom << 24;
    return m;
}

static inline uint32_t get_m_int(move m, int idx){
    return (m >> (idx * 8)) & 0xFFu;
}

static inline bitboard get_m_bitboard(move m, int idx){
    uint32_t sq = get_m_int(m, idx);    
    return 1ULL << sq;
}

static inline uint32_t modify_prom(move m, uint32_t prom){
    move new_m = m;
    new_m &= ~MASK_MOVE[3];
    new_m |= prom << 24;
    return new_m;
}

static inline bitboard splitmix64(bitboard *state) {
    bitboard z = (*state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}


enum {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8
};

