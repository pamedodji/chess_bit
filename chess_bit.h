#ifndef CHESS_BIT_H
#define CHESS_BIT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>
#include <omp.h>

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;
typedef int8_t i8;

typedef u32 cb_move;

static const u64 CB_MASK_BOARD = 0xFFFFFFFFFFFFFFFFULL;

static const u64 CB_ROWS[8] = {
	0xFFULL, 0xFFULL << 8, 0xFFULL << 16, 0xFFULL << 24,
	0xFFULL << 32, 0xFFULL << 40, 0xFFULL << 48, 0xFFULL << 56
};

static const u64 CB_COLUMNS[8] = {
	0x0101010101010101ULL, 0x0101010101010101ULL << 1,
	0x0101010101010101ULL << 2, 0x0101010101010101ULL << 3,
	0x0101010101010101ULL << 4, 0x0101010101010101ULL << 5,
	0x0101010101010101ULL << 6, 0x0101010101010101ULL << 7
};

static const u32 CB_MASK_MOVE[4] = {
	0x000000FFu,   /* src   */
	0x0000FF00u,   /* dst   */
	0x00FF0000u,   /* piece */
	0xFF000000u    /* flags */
};

static const u64 CB_ZOBRIST_SEED = 0xABCDEFABCDEFABCDULL;

/*===========================================================================
 * Enums
 *=========================================================================*/
enum { CB_WHITE = 1, CB_BLACK = 0 };

enum {
	CB_ROOK, CB_KNIGHT, CB_BISHOP, CB_QUEEN, CB_KING, CB_PAWN,
	CB_NO_PROM,
	CB_EN_PASSANT,
	CB_CAPTURES_PAWN,
	CB_CAPTURES_BISHOP,
	CB_CAPTURES_KNIGHT,
	CB_CAPTURES_ROOK,
	CB_CAPTURES_QUEEN,
	CB_CASTLE,
	CB_LONG_CASTLE
};

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

/*===========================================================================
 * Struct types
 *=========================================================================*/
typedef struct {
	int idx;
	int idx_start_looking;
	u64 rep_table[150];
} cb_rep_struct;

typedef struct {
	u64 checkers;
	u64 pinned;
	u64 block_mask;
	u64 safe_sq;
	u8 num_checkers;
} cb_king_state;

typedef struct {
	u64 pieces[12]; /* [0-5] black, [6-11] white */
	u64 player_pieces[2];
	u8 turn;
	u8 castles; /* bit0 w_castle, bit1 w_long, bit2 b_castle, bit3 b_long */
	i8 w_en_passant_flag;
	i8 b_en_passant_flag;
	u8 fifty_moves;
	cb_rep_struct *rep;
} cb_board;

typedef struct {
	u8 castles;
	i8 w_en_passant_flag;
	i8 b_en_passant_flag;
	u8 fifty_moves;
	cb_move m;
	u8 piece_dst;
	u8 rep_idx;
} cb_unmake_info;

typedef struct {
	i8 r;
	i8 c;
} cb_knight_mov;

typedef struct {
	cb_move m[240];
	u8 index;
	u8 size;
} cb_list_move;

typedef struct {
	u64 north, south, east, west;
} cb_rooksray;

typedef struct {
	u64 n_east, n_west, s_east, s_west;
} cb_bishopsray;

typedef struct {
    long wtime;       // ms left for white          (-1 if not provided) 
    long btime;       // ms left for black          (-1 if not provided) 
    long winc;        // white increment in ms      (0 if absent)        
    long binc;        // black increment in ms      (0 if absent)        
    int  movestogo;   // moves to next time control (0 = unknown)       
    long movetime;    // exact time per move        (-1 if not provided) 
    int  depth;       // fixed search depth         (-1 if not provided) 
} cb_time_control;

/*===========================================================================
 * Global tables (extern declarations)
 *=========================================================================*/
extern u64 cb_knight_table[64];
extern u64 cb_kings_table[64];

extern u64 cb_pos_table[12][64];
extern u64 cb_zob_k, cb_zob_q, cb_zob_K, cb_zob_Q;
extern u64 cb_en_passant_table[8];
extern u64 cb_turn_table;

extern cb_rooksray cb_rrays[64];
extern cb_bishopsray cb_brays[64];

/* Knight move offsets */
static const cb_knight_mov cb_n_mov[8] = {
	{-2, -1}, {-2, 1}, {2, 1}, {2, -1}, {-1, -2}, {-1, 2}, {1, 2}, {1, -2}
};

/*===========================================================================
 * Public function declarations
 *=========================================================================*/

/* utils */
void cb_init_board(cb_board *b, cb_rep_struct *rep);
char *cb_bitboard_to_board(cb_board *b);
void cb_print_board(cb_board *b);
u64 cb_string_to_bitboard(char *string_square);
void cb_print_bitboard_tab(u64 *tab);
char *cb_bitboard_to_string(u64 square);
long cb_get_time_ms();
void cb_print_moves(cb_list_move *l);
void cb_set_position_2(cb_board *b, cb_rep_struct *rep);
void cb_set_position_3(cb_board *b, cb_rep_struct *rep);
void cb_set_position_4(cb_board *b, cb_rep_struct *rep);
void cb_set_position_5(cb_board *b, cb_rep_struct *rep);
long cb_perft(cb_board *b, int depth);
void cb_perft_divide(cb_board *b, int depth, int nb_threads);
void cb_perft_divide_2(cb_board *b, int depth);
void cb_fen_to_board(cb_board *b, cb_rep_struct *rep, char *fen);
long cb_perft_2(cb_board *b, int depth);
void cb_print_board_info(cb_board *b);
void cb_free_board(cb_board *b);
void cb_verify_logics(int depth, int nb_threads);
long long cb_get_real_time_ms();

/* logics */
void cb_init();
void cb_make_move(cb_board *b, cb_move m, int rep, cb_unmake_info *info);
int cb_is_attacked_row(const cb_board *b, u64 square);
int cb_is_attacked_column(const cb_board *b, u64 square);
int cb_is_attacked_diagonal(const cb_board *b, u64 square);
int cb_is_attacked_by_knight(const cb_board *b, u64 square);
int cb_is_attacked_pawn(const cb_board *b, u64 square);
int cb_is_attacked(const cb_board *b, u64 square);
int cb_is_attacked_king(const cb_board *b, u64 square);
int cb_is_check(const cb_board *b);
void cb_legal_moves(cb_board *b, cb_list_move *l);
void cb_rook_moves(const cb_board *b, u32 sq_idx, cb_list_move *l, u32 piece);
void cb_bishop_moves(const cb_board *b, u32 sq_idx, cb_list_move *l, u32 piece);
void cb_queen_moves(const cb_board *b, u32 sq_idx, cb_list_move *l);
void cb_king_moves(const cb_board *b, cb_list_move *l);
void cb_knight_moves(const cb_board *b, u32 sq_idx, cb_list_move *l);
void cb_pawn_all_moves(const cb_board *b, cb_list_move *l);
void cb_queen_all_moves(const cb_board *b, cb_list_move *l);
void cb_pseudo_legal_moves(const cb_board *b, cb_list_move *l);
int cb_is_legal_move(const cb_board *b, cb_move m);
void cb_unmake(cb_board *b, cb_unmake_info *info);
int cb_is_repetition(const cb_board *b);
int cb_game_state(cb_board *b);
int cb_insufficient_material(const cb_board *b);
int cb_is_checkmate(cb_board *b);
int cb_is_draw(cb_board *b);
cb_king_state cb_get_king_state(cb_board *b);

/* zobrist */
void cb_init_zobrist_tables();
u64 cb_zobrist_key(const cb_board *b);

/* uci */
cb_move cb_parse_move(cb_board *b, const char *str);
void cb_move_to_str(cb_move m, char *out);
void cb_handle_position(cb_board *b, cb_rep_struct *rep, const char *line);
void cb_handle_go(cb_board *b, int nb_threads, const char *line);
void cb_play_game(int nb_threads);

/*===========================================================================
 * Inline functions
 *=========================================================================*/

static inline int cb_pop_inplace(u64 *square) {
	int tz = __builtin_ctzll(*square);
	*square &= *square - 1;
	return tz;
}

static inline int cb_get_square_index(u64 square) {
	if (square == 0)
		return -1;
	return __builtin_ctzll(square);
}

static inline u64 cb_index_to_bitboard(int idx) {
	return 1ULL << idx;
}

static inline int cb_get_row(u64 square) {
	return cb_get_square_index(square) / 8;
}

static inline int cb_get_column(u64 square) {
	return cb_get_square_index(square) % 8;
}

static inline int cb_on_board(int row, int column) {
	return 0 <= row && row < 8 && 0 <= column && column < 8;
}

static inline int cb_is_not_en_passant(const cb_board *b, const u64 src, const u64 dst, const int piece) {
	return piece != CB_PAWN
		|| src == dst << 8 || src == dst << 16
		|| src == dst >> 8 || src == dst >> 16
		|| (dst & (b->player_pieces[0] | b->player_pieces[1])) != 0;
}

static inline u32 cb_create_move(u32 src, u32 dst, u32 piece, u32 flags) {
	return src | (dst << 8) | (piece << 16) | (flags << 24);
}

static inline u32 cb_get_m_int(cb_move m, int idx) {
	return (m >> (idx * 8)) & 0xFFu;
}

static inline u64 cb_get_m_bitboard(cb_move m, int idx) {
	return 1ULL << cb_get_m_int(m, idx);
}

static inline u32 cb_modify_prom(cb_move m, u32 prom) {
	return (m & ~CB_MASK_MOVE[3]) | (prom << 24);
}

static inline u64 cb_splitmix64(u64 *state) {
	u64 z = (*state += 0x9e3779b97f4a7c15ULL);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
	return z ^ (z >> 31);
}

static inline u64 cb_bzlo_u64(u64 squares, u32 idx) {
	u64 mask = (1ULL << idx) - 1;
	return _andn_u64(mask, squares);
}

static inline void cb_add_piece(cb_board *b, u32 turn, u32 piece, u64 square) {
	b->player_pieces[turn] |= square;
	b->pieces[piece + 6 * turn] |= square;
}

static inline void cb_delete_piece(cb_board *b, u32 turn, u32 piece, u64 square) {
	b->player_pieces[turn] &= ~square;
	b->pieces[piece + 6 * turn] &= ~square;
}

static inline void cb_make_move_castle(cb_board *b, const cb_move m, const u32 flag, const u64 src, const u64 dst) {
	b->castles &= ~(3 + (b->turn ^ 1) * 9);
	cb_delete_piece(b, b->turn, CB_KING, src);
	cb_add_piece(b, b->turn, CB_KING, dst);
	if (flag == CB_CASTLE) {
		cb_delete_piece(b, b->turn, CB_ROOK, dst << 1);
		cb_add_piece(b, b->turn, CB_ROOK, src << 1);
	}
	else {
		cb_delete_piece(b, b->turn, CB_ROOK, dst >> 2);
		cb_add_piece(b, b->turn, CB_ROOK, dst << 1);
	}
}

static inline void cb_make_move_en_passant(cb_board *b, const cb_move m, cb_unmake_info *info, u64 src, u64 dst) {
	cb_delete_piece(b, b->turn, CB_PAWN, src);
	cb_add_piece(b, b->turn, CB_PAWN, dst);
	if (b->turn == CB_WHITE)
		cb_delete_piece(b, CB_BLACK, CB_PAWN, dst >> 8);
	else
		cb_delete_piece(b, CB_WHITE, CB_PAWN, dst << 8);
}

static inline void cb_make_move_promotion(cb_board *b, const cb_move m, cb_unmake_info *info, u32 flag, u64 src, u64 dst, u64 opp_piece) {
	cb_delete_piece(b, b->turn, CB_PAWN, src);
	cb_add_piece(b, b->turn, flag, dst);
	if (opp_piece != 7)
		cb_delete_piece(b, b->turn ^ 1, opp_piece, dst);
}


static inline void cb_debug_board_state(const cb_board *b, int i) {
	printf("=== i:%d ===\n", i);
	printf("turn: %d\n", b->turn);
	printf("castles: %d\n", b->castles);
	printf("w_ep: %d\n", b->w_en_passant_flag);
	printf("b_ep: %d\n", b->b_en_passant_flag);
	printf("fifty: %d\n", b->fifty_moves);
	printf("hash: %lu\n\n", cb_zobrist_key(b));
}

/*===========================================================================
 * IMPLEMENTATION
 *=========================================================================*/
#ifdef CHESSBIT_IMPLEMENTATION

u64 cb_knight_table[64];
u64 cb_kings_table[64];
cb_rooksray cb_rrays[64];
cb_bishopsray cb_brays[64];
unsigned int cb_nodes;

/* -------------------------------------------------------------------------
 * logics.c
 * -----------------------------------------------------------------------*/

static void rook_all_moves(const cb_board *b, cb_list_move *l) {
	u64 copy_mask = b->pieces[CB_ROOK + 6 * b->turn];
	while (copy_mask > 0) {
		u64 square = copy_mask & -copy_mask;
		copy_mask ^= square;
		cb_rook_moves(b, __builtin_ctzll(square), l, CB_ROOK);
	}
}

static void bishop_all_moves(const cb_board *b, cb_list_move *l) {
	u64 copy_mask = b->pieces[CB_BISHOP + 6 * b->turn];
	while (copy_mask > 0) {
		int tz = __builtin_ctzll(copy_mask);
		cb_bishop_moves(b, tz, l, CB_BISHOP);
		copy_mask &= copy_mask - 1;
	}
}

static void knight_all_moves(const cb_board *b, cb_list_move *l) {
	u64 copy_mask = b->pieces[CB_KNIGHT + 6 * b->turn];
	while (copy_mask > 0) {
		int tz = __builtin_ctzll(copy_mask);
		cb_knight_moves(b, tz, l);
		copy_mask &= copy_mask - 1;
	}
}

void cb_init() {
	cb_init_zobrist_tables();

	memset(cb_rrays, 0, 64 * sizeof(cb_rooksray));
	for (int i = 0; i < 64; i++) {
		u64 square = 1ULL << i;
		int row = i / 8;
		int col = i % 8;
		int count = 1;

		while (count < 8 && ((square << count) & CB_ROWS[row]))
			cb_rrays[i].east |= square << count++;
		count = 1;
		while (count < 8 && ((square >> count) & CB_ROWS[row]))
			cb_rrays[i].west |= square >> count++;
		count = 1;
		while (count < 8 && ((square << 8 * count) & CB_COLUMNS[col]))
			cb_rrays[i].north |= square << 8 * count++;
		count = 1;
		while (count < 8 && ((square >> 8 * count) & CB_COLUMNS[col]))
			cb_rrays[i].south |= square >> 8 * count++;
	}

	memset(cb_knight_table, 0, 64 * sizeof(u64));
	for (int i = 0; i < 64; i++) {
		u64 square = 1ULL << i;
		int row = i / 8;
		int col = i % 8;

		for (int j = 0; j < 8; j++) {
			int rm = cb_n_mov[j].r;
			int cm = cb_n_mov[j].c;
			if (!cb_on_board(row + rm, col + cm))
				continue;
			if (rm < 0)
				cb_knight_table[i] |= square >> (-rm * 8 - cm);
			else
				cb_knight_table[i] |= square << (rm * 8 + cm);
		}
	}

	memset(cb_brays, 0, 64 * sizeof(cb_bishopsray));
	for (int i = 0; i < 64; i++) {
		u64 square = 1ULL << i;
		int row = i / 8;
		int col = i % 8;
		int h = 1;
		int v = 1;

		while (row + v < 8 && col + h < 8) {
			cb_brays[i].n_east |= square << (8 * v + h);
			h++;
			v++;
		}
		h = v = 1;
		while (row + v < 8 && col - h >= 0) {
			cb_brays[i].n_west |= square << (8 * v - h);
			h++;
			v++;
		}
		h = v = 1;
		while (row - v >= 0 && col + h < 8) {
			cb_brays[i].s_east |= square >> (8 * v - h);
			h++;
			v++;
		}
		h = v = 1;
		while (row - v >= 0 && col - h >= 0) {
			cb_brays[i].s_west |= square >> (8 * v + h);
			h++;
			v++;
		}
	}

	memset(cb_kings_table, 0, 64 * sizeof(u64));
	for (int i = 0; i < 64; i++) {
		u64 sq = 1ULL << i;
		int row = i / 8;
		int col = i % 8;

		if (row + 1 < 8) {
			if (col + 1 < 8)
				cb_kings_table[i] |= sq << 9;
			if (col - 1 >= 0)
				cb_kings_table[i] |= sq << 7;
			cb_kings_table[i] |= sq << 8;
		}
		if (col + 1 < 8)
			cb_kings_table[i] |= sq << 1;
		if (col - 1 >= 0)
			cb_kings_table[i] |= sq >> 1;
		if (row - 1 >= 0) {
			if (col + 1 < 8)
				cb_kings_table[i] |= sq >> 7;
			if (col - 1 >= 0)
				cb_kings_table[i] |= sq >> 9;
			cb_kings_table[i] |= sq >> 8;
		}
	}
}

int cb_is_attacked_row(const cb_board *b, u64 square) {
	int piece = CB_ROOK + 6 * (b->turn ^ 1);
	u64 QR = b->pieces[piece] | b->pieces[3 + piece];
	u64 occ = b->player_pieces[CB_BLACK] | b->player_pieces[CB_WHITE];
	u32 sq = __builtin_ctzll(square);
	u64 bl = occ & cb_rrays[sq].east;

	if (bl && ((1ULL << __builtin_ctzll(bl)) & QR))
		return 1;
	bl = occ & cb_rrays[sq].west;
	if (bl && ((1ULL << (63 - __builtin_clzll(bl))) & QR))
		return 1;
	return 0;
}

int cb_is_attacked_column(const cb_board *b, u64 square) {
	int piece = CB_ROOK + 6 * (b->turn ^ 1);
	u64 QR = b->pieces[piece] | b->pieces[3 + piece];
	u64 occ = b->player_pieces[0] | b->player_pieces[1];
	u32 sq = __builtin_ctzll(square);
	u64 bl = occ & cb_rrays[sq].south;

	if (bl && ((1ULL << (63 - __builtin_clzll(bl))) & QR))
		return 1;
	bl = occ & cb_rrays[sq].north;
	if (bl && ((1ULL << __builtin_ctzll(bl)) & QR))
		return 1;
	return 0;
}

int cb_is_attacked_diagonal(const cb_board *b, u64 square) {
	int piece = CB_BISHOP + 6 * (b->turn ^ 1);
	u64 QB = b->pieces[piece] | b->pieces[1 + piece];
	u64 occ = b->player_pieces[0] | b->player_pieces[1];
	u32 sq = __builtin_ctzll(square);
	u64 bl = occ & cb_brays[sq].n_east;

	if (bl && ((1ULL << __builtin_ctzll(bl)) & QB))
		return 1;
	bl = occ & cb_brays[sq].n_west;
	if (bl && ((1ULL << __builtin_ctzll(bl)) & QB))
		return 1;
	bl = occ & cb_brays[sq].s_east;
	if (bl && ((1ULL << (63 - __builtin_clzll(bl))) & QB))
		return 1;
	bl = occ & cb_brays[sq].s_west;
	if (bl && ((1ULL << (63 - __builtin_clzll(bl))) & QB))
		return 1;
	return 0;
}

int cb_is_attacked_by_knight(const cb_board *b, u64 square) {
	u64 opp = b->pieces[CB_KNIGHT + 6 * (b->turn ^ 1)];
	return (opp & cb_knight_table[__builtin_ctzll(square)]) > 0;
}

int cb_is_attacked_pawn(const cb_board *b, u64 square) {
	int row = cb_get_row(square);
	int col = cb_get_column(square);

	if (b->turn == CB_WHITE)
		return (cb_on_board(row + 1, col + 1) && ((square << 9) & b->pieces[CB_PAWN]))
			|| (cb_on_board(row + 1, col - 1) && ((square << 7) & b->pieces[CB_PAWN]));
	return (cb_on_board(row - 1, col + 1) && ((square >> 7) & b->pieces[CB_PAWN + 6]))
		|| (cb_on_board(row - 1, col - 1) && ((square >> 9) & b->pieces[CB_PAWN + 6]));
}

int cb_is_attacked_king(const cb_board *b, u64 square) {
	u64 opp = b->pieces[CB_KING + 6 * (b->turn ^ 1)];
	return (opp & cb_kings_table[__builtin_ctzll(square)]) > 0;
}

int cb_is_attacked(const cb_board *b, u64 square) {
	return cb_is_attacked_column(b, square)
		|| cb_is_attacked_row(b, square)
		|| cb_is_attacked_diagonal(b, square)
		|| cb_is_attacked_by_knight(b, square)
		|| cb_is_attacked_pawn(b, square)
		|| cb_is_attacked_king(b, square);
}

int cb_is_check(const cb_board *b) {
	return cb_is_attacked(b, b->pieces[CB_KING + 6 * b->turn]);
}

void cb_queen_all_moves(const cb_board *b, cb_list_move *l) {
	u64 copy_mask = b->pieces[CB_QUEEN + 6 * b->turn];
	while (copy_mask > 0) {
		int tz = __builtin_ctzll(copy_mask);
		cb_queen_moves(b, tz, l);
		copy_mask &= copy_mask - 1;
	}
}

void cb_pseudo_legal_moves(const cb_board *b, cb_list_move *l) {
	l->index = 0;
	rook_all_moves(b, l);
	bishop_all_moves(b, l);
	knight_all_moves(b, l);
	cb_pawn_all_moves(b, l);
	cb_queen_all_moves(b, l);
	cb_king_moves(b, l);
	l->size = l->index;
	l->index = 0;
}

void cb_legal_moves(cb_board *b, cb_list_move *l) {
	cb_pseudo_legal_moves(b, l);
	if (l->size == 0)
		return;

	cb_king_state ks = cb_get_king_state(b);
	int new_ind = 0;
	u64 sq_src;
	u64 sq_dst;
	u32 piece;
	u32 flag;
	cb_unmake_info info;

	if (ks.num_checkers == 0) {
		for (int i = 0; i < l->size; i++) {
			sq_src = cb_get_m_bitboard(l->m[i], 0);
			piece = cb_get_m_int(l->m[i], 2);
			flag = cb_get_m_int(l->m[i], 3);

			if (flag == CB_CASTLE || flag == CB_LONG_CASTLE) {
				sq_dst = cb_get_m_bitboard(l->m[i], 1);
				if ((sq_src << 2) == sq_dst) {
					if (cb_is_attacked(b, sq_src << 1) || cb_is_attacked(b, sq_src << 2))
						continue;
				}
				else {
					if (cb_is_attacked(b, sq_src >> 1) || cb_is_attacked(b, sq_src >> 2))
						continue;
				}
				l->m[new_ind++] = l->m[i];
			}
			else if (piece == CB_KING) {
				sq_dst = cb_get_m_bitboard(l->m[i], 1);
				if (sq_dst & ks.safe_sq)
					l->m[new_ind++] = l->m[i];
			}
			else if ((sq_src & ks.pinned) || flag == CB_EN_PASSANT) {
				cb_make_move(b, l->m[i], 0, &info);
				b->turn ^= 1;
				if (!cb_is_check(b))
					l->m[new_ind++] = l->m[i];
				b->turn ^= 1;
				cb_unmake(b, &info);
			}
			else
				l->m[new_ind++] = l->m[i];
		}
	}
	else if (ks.num_checkers == 1) {
		for (int i = 0; i < l->size; i++) {
			sq_src = cb_get_m_bitboard(l->m[i], 0);
			sq_dst = cb_get_m_bitboard(l->m[i], 1);
			piece = cb_get_m_int(l->m[i], 2);
			flag = cb_get_m_int(l->m[i], 3);

			if (flag == CB_CASTLE || flag == CB_LONG_CASTLE)
				continue;
			if (piece == CB_KING) {
				if (sq_dst & ks.safe_sq)
					l->m[new_ind++] = l->m[i];
				continue;
			}
			if (!(sq_dst & ks.block_mask) && flag != CB_EN_PASSANT)
				continue;

			cb_make_move(b, l->m[i], 0, &info);
			b->turn ^= 1;
			if (!cb_is_check(b))
				l->m[new_ind++] = l->m[i];
			b->turn ^= 1;
			cb_unmake(b, &info);
		}
	}
	else {
		u64 king_sq = b->pieces[CB_KING + 6 * b->turn];
		u32 king_idx = __builtin_ctzll(king_sq);
		u64 legals = ks.safe_sq;

		while (legals) {
			u32 tz = __builtin_ctzll(legals);
			l->m[new_ind++] = cb_create_move(king_idx, tz, CB_KING, CB_NO_PROM);
			legals &= legals - 1;
		}
	}
	l->size = new_ind;
}

int cb_is_legal_move(const cb_board *b, cb_move m) {
	cb_board temp = *b;
	u64 src = cb_get_m_bitboard(m, 0);
	u64 dst = cb_get_m_bitboard(m, 1);

	if (cb_get_m_int(m, 3) == CB_KING) {
		if (cb_is_check(b))
			return 0;
		if ((src << 2) == dst) {
			if (cb_is_attacked(b, b->pieces[CB_KING + 6 * b->turn] << 1)
			 || cb_is_attacked(b, b->pieces[CB_KING + 6 * b->turn] << 2))
				return 0;
			return 1;
		}
		if ((src >> 2) == dst) {
			if (cb_is_attacked(b, b->pieces[CB_KING + 6 * b->turn] >> 1)
			 || cb_is_attacked(b, b->pieces[CB_KING + 6 * b->turn] >> 2))
				return 0;
			return 1;
		}
	}

	cb_make_move(&temp, m, 0, NULL);
	temp.turn ^= 1;
	return !cb_is_check(&temp);
}

static int is_repetition_internal(const cb_board *b) {
	int last = (b->rep->idx == 0) ? 149 : (b->rep->idx - 1);
	u64 last_move = b->rep->rep_table[last];
	int i = b->rep->idx_start_looking;
	int count = 1;

	while (i != last) {
		if (b->rep->rep_table[i] == last_move) {
			count++;
			if (count == 3)
				return 1;
		}
		if (++i == 150)
			i = 0;
	}
	return count == 3;
}

int cb_is_repetition(const cb_board *b) {
	return is_repetition_internal(b);
}

int cb_insufficient_material(const cb_board *b) {
	int w = __builtin_popcountll(b->player_pieces[0]);
	int bl = __builtin_popcountll(b->player_pieces[1]);

	if (w == 1 && bl == 1)
		return 1;
	if (w == 2 && bl == 1 && (b->pieces[CB_BISHOP + 6] || b->pieces[CB_KNIGHT + 6]))
		return 1;
	if (bl == 2 && w == 1 && (b->pieces[CB_BISHOP] || b->pieces[CB_KNIGHT]))
		return 1;
	if (w == 2 && bl == 2
		&& (b->pieces[CB_BISHOP] || b->pieces[CB_KNIGHT])
		&& (b->pieces[CB_BISHOP + 6] || b->pieces[CB_KNIGHT + 6]))
		return 1;
	return 0;
}

int cb_is_checkmate(cb_board *b) {
	cb_list_move l;
	cb_legal_moves(b, &l);
	return l.size == 0 && cb_is_check(b);
}

int cb_is_draw(cb_board *b) {
	if (b->fifty_moves >= 150)
		return 1;
	if (is_repetition_internal(b))
		return 1;
	if (cb_insufficient_material(b))
		return 1;

	cb_list_move l;
	cb_legal_moves(b, &l);
	if (l.size == 0 && !cb_is_check(b))
		return 1;
	return 0;
}

int cb_game_state(cb_board *b) {
	if (b->fifty_moves >= 150)
		return 0;

	cb_list_move l;
	cb_legal_moves(b, &l);
	if (l.size == 0)
		return cb_is_check(b) ? 1 : 0;
	if (is_repetition_internal(b))
		return 0;
	if (cb_insufficient_material(b))
		return 0;
	return 2;
}

/* -------------------------------------------------------------------------
 * kingstate.c
 * -----------------------------------------------------------------------*/

cb_king_state cb_get_king_state(cb_board *b) {
	cb_king_state ks;
	memset(&ks, 0, sizeof(cb_king_state));

	u8 i;
	u8 count;
	u8 rq_cnt;
	u8 bq_cnt;
	i8 dir;
	u64 attackers;
	u64 safe_tmp;
	u64 my = b->player_pieces[b->turn];
	u64 opp = b->player_pieces[b->turn ^ 1];
	u8 off = 6 * b->turn;
	u8 opp_off = 6 * (b->turn ^ 1);
	u64 king_sq = b->pieces[CB_KING + off];
	u8 ksq = __builtin_ctzll(king_sq);
	u64 pin = 0;

	ks.safe_sq = cb_kings_table[ksq] & ~my;
	safe_tmp = ks.safe_sq;
	cb_delete_piece(b, b->turn, CB_KING, king_sq);
	while (safe_tmp) {
		u64 tsq = 1ULL << __builtin_ctzll(safe_tmp);
		if (cb_is_attacked(b, tsq))
			ks.safe_sq &= ~tsq;
		safe_tmp &= safe_tmp - 1;
	}
	cb_add_piece(b, b->turn, CB_KING, king_sq);

	attackers = 0;
	if (b->turn == CB_WHITE && (king_sq & ~CB_ROWS[7]))
		attackers = b->pieces[CB_PAWN] & (((king_sq << 9) & ~CB_COLUMNS[0]) | ((king_sq << 7) & ~CB_COLUMNS[7]));
	else if (b->turn == CB_BLACK && (king_sq & ~CB_ROWS[0]))
		attackers = b->pieces[CB_PAWN + 6] & (((king_sq >> 9) & ~CB_COLUMNS[7]) | ((king_sq >> 7) & ~CB_COLUMNS[0]));
	if (attackers) {
		ks.checkers = attackers;
		ks.num_checkers = 1;
		ks.block_mask = attackers;
	}

	u64 QR = b->pieces[CB_QUEEN + opp_off] | b->pieces[CB_ROOK + opp_off];
	u64 QB = b->pieces[CB_QUEEN + opp_off] | b->pieces[CB_BISHOP + opp_off];
	dir = 0;
	rq_cnt = 0;

	if (rq_cnt < 2) {
		attackers = cb_rrays[ksq].east & QR;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QR & (king_sq << i)) == 0) {
				if (opp & (king_sq << i))
					break;
				if (my & (king_sq << i)) {
					if (++count == 2)
						break;
					pin = king_sq << i;
				}
				i++;
			}
			if (QR & (king_sq << i)) {
				if (count == 0) {
					ks.checkers |= king_sq << i;
					ks.num_checkers++;
					dir = 1;
					rq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	if (rq_cnt < 2) {
		attackers = cb_rrays[ksq].west & QR;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QR & (king_sq >> i)) == 0) {
				if (opp & (king_sq >> i))
					break;
				if (my & (king_sq >> i)) {
					if (++count == 2)
						break;
					pin = king_sq >> i;
				}
				i++;
			}
			if (QR & (king_sq >> i)) {
				if (count == 0) {
					ks.checkers |= king_sq >> i;
					ks.num_checkers++;
					dir = -1;
					rq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	if (rq_cnt < 2) {
		attackers = cb_rrays[ksq].north & QR;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QR & (king_sq << 8 * i)) == 0) {
				if (opp & (king_sq << 8 * i))
					break;
				if (my & (king_sq << 8 * i)) {
					if (++count == 2)
						break;
					pin = king_sq << 8 * i;
				}
				i++;
			}
			if (QR & (king_sq << 8 * i)) {
				if (count == 0) {
					ks.checkers |= king_sq << 8 * i;
					ks.num_checkers++;
					dir = 8;
					rq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	if (rq_cnt < 2) {
		attackers = cb_rrays[ksq].south & QR;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QR & (king_sq >> 8 * i)) == 0) {
				if (opp & (king_sq >> 8 * i))
					break;
				if (my & (king_sq >> 8 * i)) {
					if (++count == 2)
						break;
					pin = king_sq >> 8 * i;
				}
				i++;
			}
			if (QR & (king_sq >> 8 * i)) {
				if (count == 0) {
					ks.checkers |= king_sq >> 8 * i;
					ks.num_checkers++;
					dir = -8;
					rq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	bq_cnt = 0;

	attackers = cb_brays[ksq].n_east & QB;
	if (attackers) {
		count = 0;
		i = 1;
		pin = 0;
		while ((QB & (king_sq << 9 * i)) == 0) {
			if (opp & (king_sq << 9 * i))
				break;
			if (my & (king_sq << 9 * i)) {
				if (++count == 2)
					break;
				pin = king_sq << 9 * i;
			}
			i++;
		}
		if (QB & (king_sq << 9 * i)) {
			if (count == 0) {
				ks.checkers |= king_sq << 9 * i;
				ks.num_checkers++;
				dir = 9;
				bq_cnt++;
			}
			else if (count == 1)
				ks.pinned |= pin;
		}
	}

	if (bq_cnt < 2) {
		attackers = cb_brays[ksq].n_west & QB;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QB & (king_sq << 7 * i)) == 0) {
				if (opp & (king_sq << 7 * i))
					break;
				if (my & (king_sq << 7 * i)) {
					if (++count == 2)
						break;
					pin = king_sq << 7 * i;
				}
				i++;
			}
			if (QB & (king_sq << 7 * i)) {
				if (count == 0) {
					ks.checkers |= king_sq << 7 * i;
					ks.num_checkers++;
					dir = 7;
					bq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	if (bq_cnt < 2) {
		attackers = cb_brays[ksq].s_east & QB;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QB & (king_sq >> 7 * i)) == 0) {
				if (opp & (king_sq >> 7 * i))
					break;
				if (my & (king_sq >> 7 * i)) {
					if (++count == 2)
						break;
					pin = king_sq >> 7 * i;
				}
				i++;
			}
			if (QB & (king_sq >> 7 * i)) {
				if (count == 0) {
					ks.checkers |= king_sq >> 7 * i;
					ks.num_checkers++;
					dir = -7;
					bq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	if (bq_cnt < 2) {
		attackers = cb_brays[ksq].s_west & QB;
		if (attackers) {
			count = 0;
			i = 1;
			pin = 0;
			while ((QB & (king_sq >> 9 * i)) == 0) {
				if (opp & (king_sq >> 9 * i))
					break;
				if (my & (king_sq >> 9 * i)) {
					if (++count == 2)
						break;
					pin = king_sq >> 9 * i;
				}
				i++;
			}
			if (QB & (king_sq >> 9 * i)) {
				if (count == 0) {
					ks.checkers |= king_sq >> 9 * i;
					ks.num_checkers++;
					dir = -9;
					bq_cnt++;
				}
				else if (count == 1)
					ks.pinned |= pin;
			}
		}
	}

	attackers = cb_knight_table[ksq] & b->pieces[CB_KNIGHT + opp_off];
	if (attackers) {
		ks.num_checkers++;
		ks.block_mask |= attackers;
		ks.checkers |= attackers;
	}

	if (ks.num_checkers == 1) {
		u64 checker = ks.checkers & -ks.checkers;
		ks.block_mask = checker;
		if (dir != 0) {
			u64 t = king_sq;
			for (int s = 0; s < 7; s++) {
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
	}
	else
		ks.block_mask = 0;

	return ks;
}

/* -------------------------------------------------------------------------
 * moves/moves.c
 * -----------------------------------------------------------------------*/

void cb_make_move(cb_board *b, cb_move m, int rep, cb_unmake_info *info) {
	u64 src = cb_get_m_bitboard(m, 0);
	u64 dst = cb_get_m_bitboard(m, 1);
	u32 flag = cb_get_m_int(m, 3);

	if (info) {
		info->castles = b->castles;
		info->w_en_passant_flag = b->w_en_passant_flag;
		info->b_en_passant_flag = b->b_en_passant_flag;
		info->fifty_moves = b->fifty_moves;
		info->m = m;
		info->piece_dst = 255;
		info->rep_idx = rep ? b->rep->idx_start_looking : 255;
	}

	b->fifty_moves++;
	u64 occ = b->player_pieces[0] | b->player_pieces[1];
	if (b->turn)
		b->w_en_passant_flag = -1;
	else
		b->b_en_passant_flag = -1;

	u32 i = cb_get_m_int(m, 2);
	int opp_piece = 7;
	if (dst & occ) {
		if (rep)
			b->rep->idx_start_looking = b->rep->idx;
		opp_piece = 6 * (b->turn ^ 1);
		while ((b->pieces[opp_piece] & dst) == 0)
			opp_piece++;
		opp_piece -= 6 * (b->turn ^ 1);
		if (info)
			info->piece_dst = opp_piece;
	}

	if (flag >= CB_CASTLE)
		cb_make_move_castle(b, m, flag, src, dst);
	else if (flag == CB_EN_PASSANT)
		cb_make_move_en_passant(b, m, info, src, dst);
	else if (flag < CB_NO_PROM)
		cb_make_move_promotion(b, m, info, flag, src, dst, opp_piece);
	else {
		if (i == CB_PAWN) {
			if (rep)
				b->rep->idx_start_looking = b->rep->idx;
			if ((src & CB_ROWS[(b->turn ^ 1) * 5 + 1]) && (dst & CB_ROWS[(b->turn ^ 1) * 1 + 3])) {
				if (b->turn)
					b->w_en_passant_flag = cb_get_column(src);
				else
					b->b_en_passant_flag = cb_get_column(src);
			}
		}
		if (i == CB_KING)
			b->castles &= ~(3 + (b->turn ^ 1) * 9);
		cb_delete_piece(b, b->turn, i, src);
		cb_add_piece(b, b->turn, i, dst);
		if (dst & occ)
			cb_delete_piece(b, b->turn ^ 1, opp_piece, dst);
	}

	if (i == CB_ROOK && b->turn) {
		if (src == 1ULL)
			b->castles &= ~2;
		if (src == (1ULL << 7))
			b->castles &= ~1;
	}
	if (i == CB_ROOK && (b->turn ^ 1)) {
		if (src == (1ULL << 56))
			b->castles &= ~8;
		if (src == (1ULL << 63))
			b->castles &= ~4;
	}
	if ((dst & occ) && opp_piece == CB_ROOK && (b->turn ^ 1)) {
		if (dst == 1ULL)
			b->castles &= ~2;
		if (dst == (1ULL << 7))
			b->castles &= ~1;
	}
	if ((dst & occ) && opp_piece == CB_ROOK && b->turn) {
		if (dst == (1ULL << 56))
			b->castles &= ~8;
		if (dst == (1ULL << 63))
			b->castles &= ~4;
	}

	b->turn ^= 1;
	if (rep) {
		b->rep->rep_table[b->rep->idx] = cb_zobrist_key(b);
		b->rep->idx = (b->rep->idx >= 149) ? 0 : b->rep->idx + 1;
	}
}

void cb_unmake(cb_board *b, cb_unmake_info *info) {
	u64 src = cb_get_m_bitboard(info->m, 0);
	u64 dst = cb_get_m_bitboard(info->m, 1);
	u32 piece_src = cb_get_m_int(info->m, 2);
	u32 flag = cb_get_m_int(info->m, 3);

	if (flag == CB_CASTLE) {
		cb_add_piece(b, b->turn ^ 1, CB_ROOK, b->pieces[CB_KING + 6 * (b->turn ^ 1)] << 1);
		cb_delete_piece(b, b->turn ^ 1, CB_ROOK, b->pieces[CB_KING + 6 * (b->turn ^ 1)] >> 1);
	}
	else if (flag == CB_LONG_CASTLE) {
		cb_add_piece(b, b->turn ^ 1, CB_ROOK, b->pieces[CB_KING + 6 * (b->turn ^ 1)] >> 2);
		cb_delete_piece(b, b->turn ^ 1, CB_ROOK, b->pieces[CB_KING + 6 * (b->turn ^ 1)] << 1);
	}

	cb_add_piece(b, b->turn ^ 1, piece_src, src);
	if (flag > 3)
		cb_delete_piece(b, b->turn ^ 1, piece_src, dst);
	else
		cb_delete_piece(b, b->turn ^ 1, flag, dst);

	if (info->piece_dst != 255 && flag != CB_EN_PASSANT)
		cb_add_piece(b, b->turn, info->piece_dst, dst);
	if (flag == CB_EN_PASSANT) {
		if (b->turn == CB_WHITE)
			cb_add_piece(b, b->turn, CB_PAWN, dst << 8);
		else
			cb_add_piece(b, b->turn, CB_PAWN, dst >> 8);
	}

	b->castles = info->castles;
	b->w_en_passant_flag = info->w_en_passant_flag;
	b->b_en_passant_flag = info->b_en_passant_flag;
	b->fifty_moves = info->fifty_moves;
	if (info->rep_idx != 255) {
		b->rep->idx = (b->rep->idx == 0) ? 149 : b->rep->idx - 1;
		b->rep->idx_start_looking = info->rep_idx;
	}
	b->fifty_moves--;
	b->turn ^= 1;
}

/* -------------------------------------------------------------------------
 * pieces/rook.c
 * -----------------------------------------------------------------------*/

void cb_rook_moves(const cb_board *b, u32 sq_idx, cb_list_move *l, u32 piece) {
	u64 occ = b->player_pieces[CB_BLACK] | b->player_pieces[CB_WHITE];
	u64 opps = b->player_pieces[b->turn ^ 1];
	u64 north = cb_rrays[sq_idx].north;
	u64 south = cb_rrays[sq_idx].south;
	u64 east = cb_rrays[sq_idx].east;
	u64 west = cb_rrays[sq_idx].west;

	if (north & occ)
		north = _bzhi_u64(north, __builtin_ctzll(north & occ) + 1);
	if (east & occ)
		east = _bzhi_u64(east, __builtin_ctzll(east & occ) + 1);
	if (south & occ)
		south = cb_bzlo_u64(south, 63 - __builtin_clzll(south & occ));
	if (west & occ)
		west = cb_bzlo_u64(west, 63 - __builtin_clzll(west & occ));

	u64 all = north | east | south | west;
	u64 cpy = all & ~occ;
	int tz;

	while (cpy) {
		tz = __builtin_ctzll(cpy);
		l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_NO_PROM);
		cpy &= cpy - 1;
	}

	cpy = all & opps;
	int ind;
	while (cpy) {
		tz = __builtin_ctzll(cpy);
		ind = 6 * (b->turn == CB_BLACK);
		while ((b->pieces[ind] & (1ULL << tz)) == 0)
			ind++;
		ind -= 6 * (b->turn == CB_BLACK);

		switch (ind) {
			case CB_ROOK:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_ROOK);
				break;
			case CB_BISHOP:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_BISHOP);
				break;
			case CB_QUEEN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_QUEEN);
				break;
			case CB_PAWN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_PAWN);
				break;
			case CB_KNIGHT:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_KNIGHT);
				break;
		}
		cpy &= cpy - 1;
	}
}

/* -------------------------------------------------------------------------
 * pieces/bishop.c
 * -----------------------------------------------------------------------*/

void cb_bishop_moves(const cb_board *b, u32 sq_idx, cb_list_move *l, u32 piece) {
	u64 occ = b->player_pieces[CB_BLACK] | b->player_pieces[CB_WHITE];
	u64 opps = b->player_pieces[b->turn ^ 1];
	u64 ne = cb_brays[sq_idx].n_east;
	u64 nw = cb_brays[sq_idx].n_west;
	u64 se = cb_brays[sq_idx].s_east;
	u64 sw = cb_brays[sq_idx].s_west;

	if (ne & occ)
		ne = _bzhi_u64(ne, __builtin_ctzll(ne & occ) + 1);
	if (nw & occ)
		nw = _bzhi_u64(nw, __builtin_ctzll(nw & occ) + 1);
	if (se & occ)
		se = cb_bzlo_u64(se, 63 - __builtin_clzll(se & occ));
	if (sw & occ)
		sw = cb_bzlo_u64(sw, 63 - __builtin_clzll(sw & occ));

	u64 all = ne | nw | se | sw;
	u64 cpy = all & ~occ;
	int tz;

	while (cpy) {
		tz = __builtin_ctzll(cpy);
		l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_NO_PROM);
		cpy &= cpy - 1;
	}

	cpy = all & opps;
	int ind;
	while (cpy) {
		tz = __builtin_ctzll(cpy);
		ind = 6 * (b->turn ^ 1);
		while ((b->pieces[ind] & (1ULL << tz)) == 0)
			ind++;
		ind -= 6 * (b->turn == CB_BLACK);

		switch (ind) {
			case CB_ROOK:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_ROOK);
				break;
			case CB_BISHOP:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_BISHOP);
				break;
			case CB_QUEEN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_QUEEN);
				break;
			case CB_PAWN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_PAWN);
				break;
			case CB_KNIGHT:
				l->m[l->index++] = cb_create_move(sq_idx, tz, piece, CB_CAPTURES_KNIGHT);
				break;
		}
		cpy &= cpy - 1;
	}
}

/* -------------------------------------------------------------------------
 * pieces/queen.c
 * -----------------------------------------------------------------------*/

void cb_queen_moves(const cb_board *b, u32 sq_idx, cb_list_move *l) {
	cb_rook_moves(b, sq_idx, l, CB_QUEEN);
	cb_bishop_moves(b, sq_idx, l, CB_QUEEN);
}

/* -------------------------------------------------------------------------
 * pieces/knight.c
 * -----------------------------------------------------------------------*/

void cb_knight_moves(const cb_board *b, u32 sq_idx, cb_list_move *l) {
	u64 opps = b->player_pieces[b->turn ^ 1];
	u64 occ = b->player_pieces[CB_WHITE] | b->player_pieces[CB_BLACK];
	u64 moves = cb_knight_table[sq_idx] & ~occ;
	u64 caps = cb_knight_table[sq_idx] & opps;
	int tz;

	while (moves) {
		tz = __builtin_ctzll(moves);
		l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_NO_PROM);
		moves &= moves - 1;
	}

	int ind;
	while (caps) {
		tz = __builtin_ctzll(caps);
		ind = 6 * (b->turn == CB_BLACK);
		while ((b->pieces[ind] & (1ULL << tz)) == 0)
			ind++;
		ind -= 6 * (b->turn == CB_BLACK);

		switch (ind) {
			case CB_ROOK:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_CAPTURES_ROOK);
				break;
			case CB_BISHOP:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_CAPTURES_BISHOP);
				break;
			case CB_QUEEN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_CAPTURES_QUEEN);
				break;
			case CB_PAWN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_CAPTURES_PAWN);
				break;
			case CB_KNIGHT:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KNIGHT, CB_CAPTURES_KNIGHT);
				break;
		}
		caps &= caps - 1;
	}
}

/* -------------------------------------------------------------------------
 * pieces/king.c
 * -----------------------------------------------------------------------*/

void cb_king_moves(const cb_board *b, cb_list_move *l) {
	u32 sq_idx = __builtin_ctzll(b->pieces[CB_KING + b->turn * 6]);
	u64 occ = b->player_pieces[CB_WHITE] | b->player_pieces[CB_BLACK];
	u64 opps = b->player_pieces[b->turn ^ 1];
	u64 moves = cb_kings_table[sq_idx] & ~occ;
	int tz;

	while (moves) {
		tz = __builtin_ctzll(moves);
		l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_NO_PROM);
		moves &= moves - 1;
	}

	u64 caps = cb_kings_table[sq_idx] & opps;
	int ind;
	while (caps) {
		tz = __builtin_ctzll(caps);
		ind = 6 * (b->turn == CB_BLACK);
		while ((b->pieces[ind] & (1ULL << tz)) == 0)
			ind++;
		ind -= 6 * (b->turn == CB_BLACK);

		switch (ind) {
			case CB_ROOK:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_CAPTURES_ROOK);
				break;
			case CB_BISHOP:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_CAPTURES_BISHOP);
				break;
			case CB_QUEEN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_CAPTURES_QUEEN);
				break;
			case CB_PAWN:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_CAPTURES_PAWN);
				break;
			case CB_KNIGHT:
				l->m[l->index++] = cb_create_move(sq_idx, tz, CB_KING, CB_CAPTURES_KNIGHT);
				break;
		}
		caps &= caps - 1;
	}

	if (b->turn == CB_WHITE) {
		if ((b->castles & 1) && (~occ & 32) && (~occ & 64))
			l->m[l->index++] = cb_create_move(sq_idx, 6, CB_KING, CB_CASTLE);
		if ((b->castles & 2) && (~occ & 8) && (~occ & 4) && (~occ & 2))
			l->m[l->index++] = cb_create_move(sq_idx, 2, CB_KING, CB_LONG_CASTLE);
	}
	else {
		if ((b->castles & 4) && (~occ & (1ULL << 61)) && (~occ & (1ULL << 62)))
			l->m[l->index++] = cb_create_move(sq_idx, 62, CB_KING, CB_CASTLE);
		if ((b->castles & 8) && (~occ & (1ULL << 59)) && (~occ & (1ULL << 58)) && (~occ & (1ULL << 57)))
			l->m[l->index++] = cb_create_move(sq_idx, 58, CB_KING, CB_LONG_CASTLE);
	}
}

/* -------------------------------------------------------------------------
 * pieces/pawn.c
 * -----------------------------------------------------------------------*/

void cb_pawn_all_moves(const cb_board *b, cb_list_move *l) {
	u64 not_occ = ~(b->player_pieces[0] | b->player_pieces[1]);
	u64 no_prom = b->pieces[CB_PAWN + 6 * b->turn] & ~CB_ROWS[6 - 5 * (b->turn ^ 1)];
	u64 prom_pawns = b->pieces[CB_PAWN + 6 * b->turn] & CB_ROWS[6 - 5 * (b->turn ^ 1)];
	u64 opps = b->player_pieces[b->turn ^ 1];
	u64 all = 0;
	int tz;
	int ind;

#define CAP_PIECE_IND(turn_val) \
	ind = 6 * (b->turn == CB_BLACK); \
	while ((b->pieces[ind] & (1ULL << tz)) == 0) \
		ind++; \
	ind -= 6 * (b->turn == CB_BLACK);

	if (b->turn == CB_WHITE) {
		all = (no_prom << 8) & not_occ;
		u64 dbl = ((all & CB_ROWS[2]) << 8) & not_occ;
		while (all) {
			tz = cb_pop_inplace(&all);
			l->m[l->index++] = cb_create_move(tz - 8, tz, CB_PAWN, CB_NO_PROM);
		}
		while (dbl) {
			tz = cb_pop_inplace(&dbl);
			l->m[l->index++] = cb_create_move(tz - 16, tz, CB_PAWN, CB_NO_PROM);
		}

		all = ((no_prom & ~CB_COLUMNS[7]) << 9) & opps;
		while (all) {
			tz = cb_pop_inplace(&all);
			CAP_PIECE_IND(CB_BLACK);
			switch (ind) {
				case CB_ROOK:
					l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_CAPTURES_ROOK);
					break;
				case CB_BISHOP:
					l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_CAPTURES_BISHOP);
					break;
				case CB_QUEEN:
					l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_CAPTURES_QUEEN);
					break;
				case CB_PAWN:
					l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_CAPTURES_PAWN);
					break;
				case CB_KNIGHT:
					l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_CAPTURES_KNIGHT);
					break;
			}
		}

		all = ((no_prom & ~CB_COLUMNS[0]) << 7) & opps;
		while (all) {
			tz = cb_pop_inplace(&all);
			CAP_PIECE_IND(CB_BLACK);
			switch (ind) {
				case CB_ROOK:
					l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_CAPTURES_ROOK);
					break;
				case CB_BISHOP:
					l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_CAPTURES_BISHOP);
					break;
				case CB_QUEEN:
					l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_CAPTURES_QUEEN);
					break;
				case CB_PAWN:
					l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_CAPTURES_PAWN);
					break;
				case CB_KNIGHT:
					l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_CAPTURES_KNIGHT);
					break;
			}
		}

		if (prom_pawns) {
			all = (prom_pawns << 8) & not_occ;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz - 8, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz - 8, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz - 8, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz - 8, tz, CB_PAWN, CB_KNIGHT);
			}
			all = ((prom_pawns & ~CB_COLUMNS[7]) << 9) & opps;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz - 9, tz, CB_PAWN, CB_KNIGHT);
			}
			all = ((prom_pawns & ~CB_COLUMNS[0]) << 7) & opps;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz - 7, tz, CB_PAWN, CB_KNIGHT);
			}
		}

		if (b->b_en_passant_flag != -1) {
			int sidx = 32 + b->b_en_passant_flag;
			u64 sq = 1ULL << sidx;
			if (no_prom & CB_ROWS[4] & (sq << 1))
				l->m[l->index++] = cb_create_move(sidx + 1, sidx + 8, CB_PAWN, CB_EN_PASSANT);
			if (no_prom & CB_ROWS[4] & (sq >> 1))
				l->m[l->index++] = cb_create_move(sidx - 1, sidx + 8, CB_PAWN, CB_EN_PASSANT);
		}
	}
	else {
		all = (no_prom >> 8) & not_occ;
		u64 dbl = ((all & CB_ROWS[5]) >> 8) & not_occ;
		while (all) {
			tz = cb_pop_inplace(&all);
			l->m[l->index++] = cb_create_move(tz + 8, tz, CB_PAWN, CB_NO_PROM);
		}
		while (dbl) {
			tz = cb_pop_inplace(&dbl);
			l->m[l->index++] = cb_create_move(tz + 16, tz, CB_PAWN, CB_NO_PROM);
		}

		all = ((no_prom & ~CB_COLUMNS[0]) >> 9) & opps;
		while (all) {
			tz = cb_pop_inplace(&all);
			CAP_PIECE_IND(CB_WHITE);
			switch (ind) {
				case CB_ROOK:
					l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_CAPTURES_ROOK);
					break;
				case CB_BISHOP:
					l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_CAPTURES_BISHOP);
					break;
				case CB_QUEEN:
					l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_CAPTURES_QUEEN);
					break;
				case CB_PAWN:
					l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_CAPTURES_PAWN);
					break;
				case CB_KNIGHT:
					l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_CAPTURES_KNIGHT);
					break;
			}
		}

		all = ((no_prom & ~CB_COLUMNS[7]) >> 7) & opps;
		while (all) {
			tz = cb_pop_inplace(&all);
			CAP_PIECE_IND(CB_WHITE);
			switch (ind) {
				case CB_ROOK:
					l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_CAPTURES_ROOK);
					break;
				case CB_BISHOP:
					l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_CAPTURES_BISHOP);
					break;
				case CB_QUEEN:
					l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_CAPTURES_QUEEN);
					break;
				case CB_PAWN:
					l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_CAPTURES_PAWN);
					break;
				case CB_KNIGHT:
					l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_CAPTURES_KNIGHT);
					break;
			}
		}

		if (prom_pawns) {
			all = (prom_pawns >> 8) & not_occ;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz + 8, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz + 8, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz + 8, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz + 8, tz, CB_PAWN, CB_KNIGHT);
			}
			all = ((prom_pawns & ~CB_COLUMNS[0]) >> 9) & opps;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz + 9, tz, CB_PAWN, CB_KNIGHT);
			}
			all = ((prom_pawns & ~CB_COLUMNS[7]) >> 7) & opps;
			while (all) {
				tz = cb_pop_inplace(&all);
				l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_QUEEN);
				l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_ROOK);
				l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_BISHOP);
				l->m[l->index++] = cb_create_move(tz + 7, tz, CB_PAWN, CB_KNIGHT);
			}
		}

		if (b->w_en_passant_flag != -1) {
			int sidx = 24 + b->w_en_passant_flag;
			u64 sq = 1ULL << sidx;
			if (no_prom & CB_ROWS[3] & (sq << 1))
				l->m[l->index++] = cb_create_move(sidx + 1, sidx - 8, CB_PAWN, CB_EN_PASSANT);
			if (no_prom & CB_ROWS[3] & (sq >> 1))
				l->m[l->index++] = cb_create_move(sidx - 1, sidx - 8, CB_PAWN, CB_EN_PASSANT);
		}
	}
#undef CAP_PIECE_IND
}

/* -------------------------------------------------------------------------
 * zobrist.c
 * -----------------------------------------------------------------------*/

u64 cb_pos_table[12][64];
u64 cb_zob_k, cb_zob_q, cb_zob_K, cb_zob_Q;
u64 cb_en_passant_table[8];
u64 cb_turn_table;

void cb_init_zobrist_tables() {
	u64 s = CB_ZOBRIST_SEED;

	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 64; j++)
			cb_pos_table[i][j] = cb_splitmix64(&s);

	cb_zob_k = cb_splitmix64(&s);
	cb_zob_q = cb_splitmix64(&s);
	cb_zob_K = cb_splitmix64(&s);
	cb_zob_Q = cb_splitmix64(&s);

	for (int i = 0; i < 8; i++)
		cb_en_passant_table[i] = cb_splitmix64(&s);

	cb_turn_table = cb_splitmix64(&s);
}

u64 cb_zobrist_key(const cb_board *b) {
	u64 hash = 0;

	for (int piece = 0; piece < 12; piece++) {
		u64 copy = b->pieces[piece];
		while (copy > 0) {
			int tz = __builtin_ctzll(copy);
			hash ^= cb_pos_table[piece][tz];
			copy &= ~(1ULL << tz);
		}
	}

	if (b->castles & 1)
		hash ^= cb_zob_k;
	if (b->castles & 2)
		hash ^= cb_zob_q;
	if (b->castles & 4)
		hash ^= cb_zob_K;
	if (b->castles & 8)
		hash ^= cb_zob_Q;
	if (b->w_en_passant_flag >= 0)
		hash ^= cb_en_passant_table[b->w_en_passant_flag];
	if (b->b_en_passant_flag >= 0)
		hash ^= cb_en_passant_table[b->b_en_passant_flag];
	if (b->turn == CB_WHITE)
		hash ^= cb_turn_table;

	return hash;
}

/* -------------------------------------------------------------------------
 * utils.c
 * -----------------------------------------------------------------------*/

static int is_digit(char c) {
	return (c >= '0' && c <= '9') ? c - '0' : -1;
}

void cb_init_board(cb_board *b, cb_rep_struct *r) {
	memset(b, 0, sizeof(*b));
	b->turn = CB_WHITE;
	b->pieces[6] = 0x81ULL;
	b->pieces[7] = 0x42ULL;
	b->pieces[8] = 0x24ULL;
	b->pieces[9] = 0x08ULL;
	b->pieces[10] = 0x10ULL;
	b->pieces[11] = CB_ROWS[1];

	for (int i = 0; i < 5; i++)
		b->pieces[i] = b->pieces[i + 6] << 56;

	b->pieces[5] = CB_ROWS[6];
	b->castles = 15;
	b->w_en_passant_flag = -1;
	b->b_en_passant_flag = -1;

	for (int i = 0; i < 6; i++)
		b->player_pieces[CB_BLACK] |= b->pieces[i];
	for (int i = 6; i < 12; i++)
		b->player_pieces[CB_WHITE] |= b->pieces[i];

	b->rep = r;
	b->rep->idx = 1;
	b->rep->idx_start_looking = 149;
	memset(b->rep->rep_table, 0, sizeof(u64) * 150);

	cb_board cpy = *b;
	b->rep->rep_table[0] = cb_zobrist_key(&cpy);
}

char *cb_bitboard_to_board(cb_board *b) {
	char *new_board = malloc(65 * sizeof(char));
	if (!new_board)
		return NULL;

	for (int i = 0; i < 64; i++)
		new_board[i] = '.';

	u64 occ = b->player_pieces[0] | b->player_pieces[1];
	for (int i = 0; i < 64; i++) {
		if (!(occ & (1ULL << i)))
			continue;
		if (b->pieces[0] & (1ULL << i))
			new_board[i] = 'r';
		else if (b->pieces[6] & (1ULL << i))
			new_board[i] = 'R';
		else if (b->pieces[1] & (1ULL << i))
			new_board[i] = 'n';
		else if (b->pieces[7] & (1ULL << i))
			new_board[i] = 'N';
		else if (b->pieces[2] & (1ULL << i))
			new_board[i] = 'b';
		else if (b->pieces[8] & (1ULL << i))
			new_board[i] = 'B';
		else if (b->pieces[3] & (1ULL << i))
			new_board[i] = 'q';
		else if (b->pieces[9] & (1ULL << i))
			new_board[i] = 'Q';
		else if (b->pieces[4] & (1ULL << i))
			new_board[i] = 'k';
		else if (b->pieces[10] & (1ULL << i))
			new_board[i] = 'K';
		else if (b->pieces[5] & (1ULL << i))
			new_board[i] = 'p';
		else if (b->pieces[11] & (1ULL << i))
			new_board[i] = 'P';
	}

	new_board[64] = '\0';
	return new_board;
}

void cb_print_board(cb_board *b) {
	char *board = cb_bitboard_to_board(b);

	for (int i = 7; i >= 0; i--) {
		for (int j = 0; j < 8; j++)
			printf("\t%c\t|", board[i * 8 + j]);
		printf("\n__________________________________________________________________________________________________________________________________\n");
	}
	free(board);
}

u64 cb_string_to_bitboard(char *s) {
	return 1ULL << ((s[1] - '1') * 8 + (s[0] - 'a'));
}

char *cb_bitboard_to_string(u64 square) {
	char *tab = malloc(3);
	u64 cpy = square;
	int row = 0;

	while ((cpy >>= 8))
		row++;

	u64 mask = CB_ROWS[row];
	cpy = square;
	int col = 0;
	while ((mask & (cpy >> 1))) {
		cpy >>= 1;
		col++;
	}

	tab[0] = (char)('a' + col);
	tab[1] = (char)('1' + row);
	tab[2] = '\0';
	return tab;
}

void cb_print_bitboard_tab(u64 *tab) {
	if (!tab)
		return;

	for (int i = 0; tab[i]; i++)
		printf("%d : %s\n", i, cb_bitboard_to_string(tab[i]));
}

long cb_get_time_ms() {
	return clock() * 1000 / CLOCKS_PER_SEC;
}

long long cb_get_real_time_ms() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

void cb_print_moves(cb_list_move *l) {
	for (int i = 0; i < l->size; i++) {
		int row = cb_get_row(cb_get_m_bitboard(l->m[i], 0));
		int col = cb_get_column(cb_get_m_bitboard(l->m[i], 0));
		printf("%c%c\t", (char)('a' + col), (char)('1' + row));
		row = cb_get_row(cb_get_m_bitboard(l->m[i], 1));
		col = cb_get_column(cb_get_m_bitboard(l->m[i], 1));
		printf("%c%c\n", (char)('a' + col), (char)('1' + row));
	}
	printf("size %d\n", l->size);
}

long cb_perft(cb_board *b, int depth) {
	if (depth == 0)
		return 1;

	cb_list_move l;
	cb_legal_moves(b, &l);
	if (depth == 1)
		return l.size;

	long nodes = 0;
	cb_unmake_info info;
	for (int i = 0; i < l.size; i++) {
		cb_make_move(b, l.m[i], 0, &info);
		nodes += cb_perft(b, depth - 1);
		cb_unmake(b, &info);
	}
	return nodes;
}

void cb_perft_divide(cb_board *b, int depth, int nb_threads) {
	if (depth == 0)
		return;

	cb_list_move l;
	cb_legal_moves(b, &l);
	if (l.size == 0) {
		printf("\ntotal moves 0\ntotal nodes 0\n");
		return;
	}

	int threads = (nb_threads < 1) ? 1 : (nb_threads > l.size ? l.size : nb_threads);
	long nodes = 0;
	long *results = malloc(sizeof(long) * l.size);
	if (!results)
		return;

	#pragma omp parallel for num_threads(threads) reduction(+:nodes)
	for (int i = 0; i < l.size; i++) {
		cb_board local = *b;
		cb_unmake_info info;
		cb_make_move(&local, l.m[i], 0, &info);
		results[i] = cb_perft(&local, depth - 1);
		nodes += results[i];
	}

	for (int i = 0; i < l.size; i++) {
		char *s = cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 0));
		char *d = cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 1));
		printf("%s %s : %ld\n", s, d, results[i]);
		free(s);
		free(d);
	}
	printf("\ntotal moves %d\ntotal nodes %ld\n", l.size, nodes);
	free(results);
}

long cb_perft_2(cb_board *b, int depth) {
	if (depth == 0)
		return 1;

	cb_list_move l;
	cb_unmake_info info;
	if (depth == 1) {
		cb_legal_moves(b, &l);
		return l.size;
	}

	cb_pseudo_legal_moves(b, &l);
	long nodes = 0;
	for (int i = 0; i < l.size; i++) {
		if (!cb_is_legal_move(b, l.m[i]))
			continue;
		cb_make_move(b, l.m[i], 0, &info);
		nodes += cb_perft_2(b, depth - 1);
		cb_unmake(b, &info);
	}
	return nodes;
}

void cb_perft_divide_2(cb_board *b, int depth) {
	if (depth == 0)
		return;

	cb_list_move l;
	cb_unmake_info info;
	cb_pseudo_legal_moves(b, &l);
	long nodes = 0;

	for (int i = 0; i < l.size; i++) {
		if (!cb_is_legal_move(b, l.m[i]))
			continue;
		cb_make_move(b, l.m[i], 0, &info);
		long tmp = cb_perft_2(b, depth - 1);
		nodes += tmp;
		char *s = cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 0));
		char *d = cb_bitboard_to_string(cb_get_m_bitboard(l.m[i], 1));
		printf("%s %s : %ld\n", s, d, tmp);
		free(s);
		free(d);
		cb_unmake(b, &info);
	}
	printf("\ntotal moves %d\ntotal nodes %ld\n", l.size, nodes);
}

void cb_set_position_2(cb_board *b, cb_rep_struct *rep) {
	cb_fen_to_board(b, rep, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
}

void cb_set_position_3(cb_board *b, cb_rep_struct *rep) {
	cb_fen_to_board(b, rep, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
}

void cb_set_position_4(cb_board *b, cb_rep_struct *rep) {
	cb_fen_to_board(b, rep, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");
}

void cb_set_position_5(cb_board *b, cb_rep_struct *rep) {
	cb_fen_to_board(b, rep, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");
}

void cb_fen_to_board(cb_board *b, cb_rep_struct *rep, char *fen) {
	memset(b, 0, sizeof(*b));
	memset(rep->rep_table, 0, sizeof(u64) * 150);
	b->w_en_passant_flag = -1;
	b->b_en_passant_flag = -1;

	int index = 0;
	int col = 7;
	int file = 0;
	int sq;
	int digit;
	char c;

	while ((c = fen[index]) != '\0' && c != ' ') {
		if (c == '/') {
			if (file != 8 || col == 0) {
				cb_init_board(b, rep);
				return;
			}
			col--;
			file = 0;
			index++;
			continue;
		}

		digit = is_digit(c);
		if (digit >= 1 && digit <= 8) {
			if (file + digit > 8) {
				cb_init_board(b, rep);
				return;
			}
			file += digit;
			index++;
			continue;
		}

		if (file >= 8) {
			cb_init_board(b, rep);
			return;
		}

		sq = col * 8 + file;
		switch (c) {
			case 'r':
				b->pieces[CB_ROOK] |= 1ULL << sq;
				break;
			case 'n':
				b->pieces[CB_KNIGHT] |= 1ULL << sq;
				break;
			case 'b':
				b->pieces[CB_BISHOP] |= 1ULL << sq;
				break;
			case 'q':
				b->pieces[CB_QUEEN] |= 1ULL << sq;
				break;
			case 'k':
				b->pieces[CB_KING] |= 1ULL << sq;
				break;
			case 'p':
				b->pieces[CB_PAWN] |= 1ULL << sq;
				break;
			case 'R':
				b->pieces[CB_ROOK + 6] |= 1ULL << sq;
				break;
			case 'N':
				b->pieces[CB_KNIGHT + 6] |= 1ULL << sq;
				break;
			case 'B':
				b->pieces[CB_BISHOP + 6] |= 1ULL << sq;
				break;
			case 'Q':
				b->pieces[CB_QUEEN + 6] |= 1ULL << sq;
				break;
			case 'K':
				b->pieces[CB_KING + 6] |= 1ULL << sq;
				break;
			case 'P':
				b->pieces[CB_PAWN + 6] |= 1ULL << sq;
				break;
			default:
				cb_init_board(b, rep);
				return;
		}
		file++;
		index++;
	}

	if (col != 0 || file != 8 || fen[index] != ' ') {
		cb_init_board(b, rep);
		return;
	}

	index++;
	if (fen[index] == 'w')
		b->turn = CB_WHITE;
	else if (fen[index] == 'b')
		b->turn = CB_BLACK;
	else {
		cb_init_board(b, rep);
		return;
	}

	index++;
	if (fen[index] != ' ') {
		cb_init_board(b, rep);
		return;
	}

	index++;
	if (fen[index] == '-')
		index++;
	else {
		int seen = 0;
		while (fen[index] != '\0' && fen[index] != ' ') {
			switch (fen[index]) {
				case 'K':
					if (b->castles & 1) {
						cb_init_board(b, rep);
						return;
					}
					b->castles |= 1;
					break;
				case 'Q':
					if (b->castles & 2) {
						cb_init_board(b, rep);
						return;
					}
					b->castles |= 2;
					break;
				case 'k':
					if (b->castles & 4) {
						cb_init_board(b, rep);
						return;
					}
					b->castles |= 4;
					break;
				case 'q':
					if (b->castles & 8) {
						cb_init_board(b, rep);
						return;
					}
					b->castles |= 8;
					break;
				default:
					cb_init_board(b, rep);
					return;
			}
			seen = 1;
			index++;
		}
		if (!seen) {
			cb_init_board(b, rep);
			return;
		}
	}

	if (fen[index] != ' ') {
		cb_init_board(b, rep);
		return;
	}

	index++;
	if (fen[index] == '-')
		index++;
	else {
		int col_ep = fen[index] - 'a';
		index++;
		int row_ep = fen[index] - '1';

		if (col_ep < 0 || col_ep > 7 || row_ep < 0 || row_ep > 7) {
			cb_init_board(b, rep);
			return;
		}

		if (row_ep == 2)
			b->w_en_passant_flag = col_ep;
		else if (row_ep == 5)
			b->b_en_passant_flag = col_ep;
		else {
			cb_init_board(b, rep);
			return;
		}
		index++;
	}

	if (fen[index] == ' ') {
		index++;
		if (is_digit(fen[index]) < 0) {
			cb_init_board(b, rep);
			return;
		}

		int val = 0;
		while (is_digit(fen[index]) >= 0) {
			val = val * 10 + (fen[index] - '0');
			index++;
		}
		b->fifty_moves = val;

		if (fen[index] == ' ') {
			index++;
			while (is_digit(fen[index]) >= 0)
				index++;
		}
	}

	if (fen[index] != '\0') {
		cb_init_board(b, rep);
		return;
	}

	b->player_pieces[CB_BLACK] = b->pieces[CB_PAWN] | b->pieces[CB_KNIGHT] | b->pieces[CB_BISHOP] | b->pieces[CB_ROOK] | b->pieces[CB_QUEEN] | b->pieces[CB_KING];
	b->player_pieces[CB_WHITE] = b->pieces[CB_PAWN + 6] | b->pieces[CB_KNIGHT + 6] | b->pieces[CB_BISHOP + 6] | b->pieces[CB_ROOK + 6] | b->pieces[CB_QUEEN + 6] | b->pieces[CB_KING + 6];
	b->rep = rep;
	b->rep->idx = 1;
	b->rep->idx_start_looking = 149;
}

void cb_print_board_info(cb_board *b) {
	printf("white pieces %lu\n", b->player_pieces[CB_WHITE]);
	printf("black pieces %lu\n", b->player_pieces[CB_BLACK]);
	printf("castles right %u\n", b->castles);
	printf("turn %d\n", b->turn);
	printf("w_en_passant %d\n", b->w_en_passant_flag);
	printf("b_en_passant %d\n", b->b_en_passant_flag);
	printf("fifty moves %d\n", b->fifty_moves);
}

void cb_free_board(cb_board *b) {
	if (b)
		free(b);
}

void cb_verify_logics(int depth, int nb_threads) {
	cb_board b;
	cb_rep_struct rep;

	cb_init_board(&b, &rep);
	cb_perft_divide(&b, depth, nb_threads);
	printf("\n\n");

	cb_set_position_2(&b, &rep);
	cb_perft_divide(&b, depth, nb_threads);
	printf("\n\n");

	cb_set_position_3(&b, &rep);
	cb_perft_divide(&b, depth, nb_threads);
	printf("\n\n");

	cb_set_position_4(&b, &rep);
	cb_perft_divide(&b, depth, nb_threads);
	printf("\n\n");

	cb_set_position_5(&b, &rep);
	cb_perft_divide(&b, depth, nb_threads);
}

/* -------------------------------------------------------------------------
 * uci/uci.c
 * -----------------------------------------------------------------------*/

cb_move cb_parse_move(cb_board *b, const char *str) {
	u32 src_idx = (str[1] - '1') * 8 + (str[0] - 'a');
	u32 dst_idx = (str[3] - '1') * 8 + (str[2] - 'a');
	cb_list_move l;
	cb_legal_moves(b, &l);
	char prom = str[4];

	for (int i = 0; i < l.size; i++) {
		u32 ms = cb_get_m_int(l.m[i], 0);
		u32 md = cb_get_m_int(l.m[i], 1);
		u32 mf = cb_get_m_int(l.m[i], 3);

		if (ms != src_idx || md != dst_idx)
			continue;

		if (prom == '\0' || prom == ' ' || prom == '\n') {
			if (mf >= CB_QUEEN && mf <= CB_KNIGHT)
				continue;
			return l.m[i];
		}

		u32 wanted = (prom == 'q') ? CB_QUEEN : (prom == 'r') ? CB_ROOK : (prom == 'b') ? CB_BISHOP : CB_KNIGHT;
		if (mf == wanted)
			return l.m[i];
	}

	return 0;
}

void cb_move_to_str(cb_move m, char *out) {
	u32 src = cb_get_m_int(m, 0);
	u32 dst = cb_get_m_int(m, 1);
	u32 flag = cb_get_m_int(m, 3);

	out[0] = 'a' + (src % 8);
	out[1] = '1' + (src / 8);
	out[2] = 'a' + (dst % 8);
	out[3] = '1' + (dst / 8);
	out[4] = '\0';

	if (flag == CB_QUEEN) {
		out[4] = 'q';
		out[5] = '\0';
	}
	else if (flag == CB_ROOK) {
		out[4] = 'r';
		out[5] = '\0';
	}
	else if (flag == CB_BISHOP) {
		out[4] = 'b';
		out[5] = '\0';
	}
	else if (flag == CB_KNIGHT) {
		out[4] = 'n';
		out[5] = '\0';
	}
}

void cb_handle_position(cb_board *b, cb_rep_struct *rep, const char *line) {
	const char *p = line + 9;

	if (strncmp(p, "startpos", 8) == 0) {
		cb_init_board(b, rep);
		p += 8;
	}
	else if (strncmp(p, "fen", 3) == 0) {
		p += 4;
		char fen[128];
		const char *mp = strstr(p, " moves");

		if (mp) {
			int len = mp - p;
			strncpy(fen, p, len);
			fen[len] = '\0';
			p = mp;
		}
		else {
			strncpy(fen, p, 127);
			fen[127] = '\0';
			p += strlen(p);
		}
		cb_fen_to_board(b, rep, fen);
	}

	p = strstr(p, "moves");
	if (!p)
		return;

	p += 5;
	while (*p == ' ')
		p++;

	while (*p != '\0') {
		cb_move m = cb_parse_move(b, p);
		if (m) {
			cb_unmake_info info;
			cb_make_move(b, m, 1, &info);
		}
		while (*p != ' ' && *p != '\0')
			p++;
		while (*p == ' ')
			p++;
	}
}

void cb_handle_go(cb_board *b, int nb_threads, const char *line) {
    long wtime    = -1, btime = -1;
    long winc     =  0, binc  = 0;
    int  movestogo = 0;
    long movetime  = -1;
    int  depth     = -1;

    char buf[512];
    strncpy(buf, line, sizeof(buf)-1);
    char *tok = strtok(buf, " ");
    while (tok) {
        if      (!strcmp(tok, "wtime"))     { tok = strtok(NULL," "); if(tok) wtime     = atol(tok); }
        else if (!strcmp(tok, "btime"))     { tok = strtok(NULL," "); if(tok) btime     = atol(tok); }
        else if (!strcmp(tok, "winc"))      { tok = strtok(NULL," "); if(tok) winc      = atol(tok); }
        else if (!strcmp(tok, "binc"))      { tok = strtok(NULL," "); if(tok) binc      = atol(tok); }
        else if (!strcmp(tok, "movestogo")){ tok = strtok(NULL," "); if(tok) movestogo = atoi(tok); }
        else if (!strcmp(tok, "movetime")) { tok = strtok(NULL," "); if(tok) movetime  = atol(tok); }
        else if (!strcmp(tok, "depth"))    { tok = strtok(NULL," "); if(tok) depth     = atoi(tok); }
        tok = strtok(NULL, " ");
    }

    cb_time_control tc;
    tc.wtime     = wtime;
    tc.btime     = btime;
    tc.winc      = winc;
    tc.binc      = binc;
    tc.movestogo = movestogo;
    tc.movetime  = movetime;
    tc.depth     = depth;

    cb_move m = 0; /* Replace with yout best_move function*/

    if (m == 0) {
        printf("bestmove 0000\n");
        fflush(stdout);
        return;
    }
    char str[6];
    cb_move_to_str(m, str);
    printf("bestmove %s\n", str);
    fflush(stdout);
}

void cb_play_game(int nb_threads) {
	cb_board b;
	cb_rep_struct rep;
	char line[4096];
	cb_init_board(&b, &rep);

	while (fgets(line, sizeof(line), stdin)) {
		if (strncmp(line, "uci", 3) == 0) {
			printf("id name bot\nid author X\nuciok\n");
			fflush(stdout);
		}
		else if (strncmp(line, "isready", 7) == 0) {
			printf("readyok\n");
			fflush(stdout);
		}
		else if (strncmp(line, "ucinewgame", 10) == 0)
			cb_init_board(&b, &rep);
		else if (strncmp(line, "position", 8) == 0)
			cb_handle_position(&b, &rep, line);
		else if (strncmp(line, "go", 2) == 0)
			cb_handle_go(&b, nb_threads, line);
		else if (strncmp(line, "quit", 4) == 0)
			break;
	}
}

#endif /* CHESSBIT_IMPLEMENTATION */
#endif /* CHESS_BIT_H */