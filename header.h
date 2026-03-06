#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <immintrin.h>

typedef uint64_t bitboard;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;
typedef int8_t i8;

extern bitboard knight_table[64];
extern bitboard kings_table[64];



enum {WHITE = 1, BLACK = 0};
enum {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, NO_PROM, EN_PASSANT, CASTLE, LONG_CASTLE};

static const u64 MASK_BOARD = 0xFFFFFFFFFFFFFFFFULL;

static const u64 ROWS[8] = {0xFFULL, 0xFFULL << 8,0xFFULL << 16, 0xFFULL << 24, 0xFFULL << 32, 0xFFULL << 40, 0xFFULL << 48, 0xFFULL << 56};
static const u64 COLUMNS[8] = {0x0101010101010101ULL, 0x0101010101010101ULL << 1, 0x0101010101010101ULL << 2, 0x0101010101010101ULL << 3,
     0x0101010101010101ULL << 4, 0x0101010101010101ULL << 5, 0x0101010101010101ULL << 6, 0x0101010101010101ULL << 7};


typedef u32 move;

typedef struct{
    int idx;
    int idx_start_looking;
    bitboard rep_table[150];
}rep_struct;

typedef struct {
    u64 checkers;     
    u64 pinned;        
    u64 block_mask;    
    u64 safe_sq; //Safe squares next to the king    
    u8 num_checkers; 
}king_state;

typedef struct{
    bitboard pieces[12]; //FIRST 6 BLACK : 0 ROOK; 1 KNIGHT; 2 BISHOP ; 3 QUEEN; 4 KING; 5 PAWN
    bitboard player_pieces[2];
    u8 turn;
    u8 castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    i8 w_en_passant_flag;
    i8 b_en_passant_flag;
    u8 fifty_moves;
    rep_struct *rep;
}board;

typedef struct {
    u8 castles; //1 w_can_castle; 2 w_can_long; //3 b_can_castle; 4 b_can_long 
    i8 w_en_passant_flag;
    i8 b_en_passant_flag;
    u8 fifty_moves;
    move m;
    u8 piece_dst;
    u8 rep_idx;
}unmake_info;

typedef struct{
    i8 r;
    i8 c;
}knight_mov;


typedef struct{
    move m[240];
    u8 index;
    u8 size;
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
void rook_moves(const board *b, u32 sq_idx, list_move *l, uint32_t piece);
void bishop_moves(const board *b, u32 sq_idx, list_move *l, uint32_t piece);
void queen_moves(const board *b, u32 sq_idx, list_move *l);
void king_moves(const board *b, list_move *l);
void knight_moves(const board *b, u32 sq_idx, list_move *l);
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
king_state get_king_state(board *b);

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

static inline uint32_t create_move(uint32_t src, uint32_t dst, uint32_t piece, uint32_t flags){
    uint32_t m = src;
    m |= dst << 8;
    m |= piece << 16;
    m |= flags << 24;
    return m;
}

static inline uint32_t get_m_int(move m, int idx){
    return (m >> (idx * 8)) & 0xFFu;
}

static inline bitboard get_m_bitboard(move m, int idx){
    uint32_t sq = get_m_int(m, idx);    
    return 1ULL << sq;
}

static inline uint32_t modify_prom(move m, u32 prom){
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

static inline bitboard bzlo_u64(bitboard squares, u32 idx){
    bitboard mask = (1ULL << idx) - 1;
    return _andn_u64(mask, squares);
}

static inline void add_piece(board *b, u32 turn, u32 piece, bitboard square){
    b -> player_pieces[turn] |= square;
    b -> pieces[piece + 6 * (turn )] |= square;
}

static inline void delete_piece(board *b, u32 turn, u32 piece, bitboard square){
    b -> player_pieces[turn] &= ~square;
    b -> pieces[piece + 6 * (turn)] &= ~square;
}

static inline void make_move_castle(board *b, const move m, const u32 flag, const u64 src, const u64 dst){
    
        b -> castles &= ~(3 + (b -> turn ^ 1)* 9);

        delete_piece(b, b -> turn, KING, src);
        add_piece(b, b -> turn, KING, dst);
        if (flag == CASTLE) {// If we're castling
            delete_piece(b, b -> turn, ROOK, dst << 1);
            add_piece(b, b -> turn, ROOK, src << 1);
        }
        else{ //LONG_CASTLE
            delete_piece(b, b -> turn , ROOK, dst >> 2);
            add_piece(b, b -> turn, ROOK, dst << 1);
        }
}

static inline void make_move_en_passant(board *b, const move m, unmake_info *info, u64 src, u64 dst){
    delete_piece(b, b -> turn, PAWN, src);
    add_piece(b, b -> turn, PAWN, dst);
    if (b -> turn == WHITE) //We're en passening with whites
        delete_piece(b, BLACK, PAWN, dst >> 8);
    
    else    //We're en passening with black
        delete_piece(b, WHITE, PAWN, dst << 8);
}

static inline void make_move_promotion(board *b, const move m, unmake_info *info, u32 flag, u64 src, u64 dst, u64 opp_piece){
    delete_piece(b, b -> turn, PAWN, src);
    add_piece(b, b -> turn, flag, dst);
    if (opp_piece != 7)
        delete_piece(b, b -> turn ^ 1, opp_piece, dst);
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

typedef struct{
    bitboard north, south, east, west;
}rooksray;

extern rooksray rrays[64];

typedef struct{
    bitboard n_east, n_west, s_east, s_west;
}bishopray;

extern bishopray brays[64];