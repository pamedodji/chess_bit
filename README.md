# chessbit

A single-header C chess engine library with bitboard move generation, legal move validation, Zobrist hashing, and a UCI interface. I built this as a personal project to learn how chess engines work under the hood.

---

## Quick Start

```bash
git clone https://github.com/pamedodji/chess_bit
cd chess_bit
gcc -O3 -march=native -flto -funroll-loops -fopenmp -o engine engine.c
./engine
```

---

## What it does

chessbit lives entirely in one header file (`chess_bit.h`). You define `CHESSBIT_IMPLEMENTATION` in one `.c` file and include the header everywhere else. That's it, no build system needed. It handles all the chess rules, gives you legal move generation, and plugs into any UCI-compatible GUI. You bring your own search function.

---

## Features

- Bitboard representation with 64-bit integers for all twelve piece types
- Precomputed ray tables for rooks and bishops (all 8 directions)
- Precomputed knight and king attack tables
- Pseudo-legal move generation + legal filtering via king state analysis
- Pin detection and check resolution (single and double check)
- Full castling support (kingside and queenside for both colors)
- En passant capture generation and validation
- Pawn promotion to queen, rook, bishop, or knight
- Fifty-move rule counter
- Threefold repetition detection using a circular Zobrist hash table
- Insufficient material detection
- FEN string parser
- Perft and divide functions, with optional OpenMP parallelism
- UCI protocol handler (`position`, `go`, `isready`, `ucinewgame`, `quit`)

---

## Requirements

- GCC or Clang, C99 or later
- BMI2 intrinsics (`_bzhi_u64`, `_andn_u64`) — you need `-mbmi2`
- OpenMP is optional, only needed for parallel perft

---

## Integration

Define `CHESSBIT_IMPLEMENTATION` in exactly one `.c` file. Every other file just includes the header normally.

```c
// engine.c  —  only here
#define CHESSBIT_IMPLEMENTATION
#include "chess_bit.h"
```

```c
// anywhere else
#include "chess_bit.h"
```

---

## Compilation

Linux:

```bash
gcc -O3 -march=native -flto -funroll-loops -fopenmp -o engine engine.c
```

Windows (cross-compile or MinGW), add `-static` to avoid runtime DLL dependencies:

```bash
gcc -O3 -march=native -flto -funroll-loops -fopenmp -static -o engine.exe engine.c
```

---

## Quick Start

### Initialize and print the starting position

```c
#define CHESSBIT_IMPLEMENTATION
#include "chess_bit.h"

int main(void) {
    cb_init(); // must be called first, builds all the lookup tables

    cb_rep_struct rep;
    cb_board b;
    cb_init_board(&b, &rep);
    cb_print_board(&b);
    return 0;
}
```

### Load a position from FEN

```c
cb_fen_to_board(&b, &rep,
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
```

### Generate legal moves

```c
cb_list_move moves;
cb_legal_moves(&b, &moves);
// moves.m[0..moves.size-1] has all the legal moves
```

### Make and unmake a move

```c
cb_unmake_info info;
cb_make_move(&b, moves.m[0], 1 /* track repetitions */, &info);
// do your search / eval here
cb_unmake(&b, &info);
```

### Run a perft test

```c
long nodes = cb_perft(&b, 5);
printf("nodes: %ld\n", nodes);

// parallel divide
cb_perft_divide(&b, 5, 4 /* threads */);
```

### Run the UCI loop

```c
cb_init();
cb_play_game(4 /* threads */);
```

---

## API Reference

### Initialization

| Function | Description |
|---|---|
| `cb_init()` | Build all lookup tables. Call this once before anything else. |
| `cb_init_board(b, rep)` | Set up the starting position. |
| `cb_fen_to_board(b, rep, fen)` | Load a position from FEN. Falls back to the starting position if the FEN is invalid. |

### Move Generation

| Function | Description |
|---|---|
| `cb_legal_moves(b, l)` | Generate all legal moves into `l`. |
| `cb_pseudo_legal_moves(b, l)` | Generate pseudo-legal moves (no legality filter). |
| `cb_is_legal_move(b, m)` | Return 1 if `m` is legal in the current position. |

### Make / Unmake

| Function | Description |
|---|---|
| `cb_make_move(b, m, rep, info)` | Apply move `m`. Pass `rep = 1` to record the position hash for repetition detection. Pass a valid `cb_unmake_info *` to be able to call `cb_unmake` afterwards. |
| `cb_unmake(b, info)` | Restore the board to its state before the last `cb_make_move`. |

### Game State

| Function | Description |
|---|---|
| `cb_is_check(b)` | Return 1 if the side to move is in check. |
| `cb_is_checkmate(b)` | Return 1 if the position is checkmate. |
| `cb_is_draw(b)` | Return 1 if the position is a draw (50-move, repetition, or insufficient material). |
| `cb_game_state(b)` | Return 0 for draw, 1 for checkmate, 2 for ongoing. |
| `cb_is_repetition(b)` | Return 1 if threefold repetition has occurred. |
| `cb_insufficient_material(b)` | Return 1 if neither side has mating material. |

### Zobrist Hashing

| Function | Description |
|---|---|
| `cb_zobrist_key(b)` | Compute the full Zobrist hash of the current board state. |
| `cb_init_zobrist_tables()` | Initialize Zobrist tables (called automatically by `cb_init`). |

### UCI

| Function | Description |
|---|---|
| `cb_play_game(nb_threads)` | Enter the UCI read loop. |
| `cb_handle_position(b, rep, line)` | Parse and apply a `position` command. |
| `cb_handle_go(b, nb_threads, line)` | Parse a `go` command and output `bestmove`. This is where you plug in your search. |
| `cb_parse_move(b, str)` | Parse a move in long algebraic notation (e.g. `e2e4`, `e7e8q`). |
| `cb_move_to_str(m, out)` | Convert a `cb_move` to its string representation. |

### Perft

| Function | Description |
|---|---|
| `cb_perft(b, depth)` | Return the node count at `depth`. |
| `cb_perft_divide(b, depth, threads)` | Print per-move node counts, parallelized with OpenMP. |
| `cb_perft_2(b, depth)` | Alternative perft using pseudo-legal generation + `cb_is_legal_move`. Slower but useful to cross-check. |

---

## Data Structures

### cb_board

The main board state. Everything you need is in here.

| Field | Type | Description |
|---|---|---|
| `pieces[12]` | `u64[12]` | Bitboards for each piece. Indices 0-5 are black, 6-11 are white. Order: rook, knight, bishop, queen, king, pawn. |
| `player_pieces[2]` | `u64[2]` | Combined occupancy per side. Index 0 = black, index 1 = white. |
| `turn` | `u8` | Side to move: `CB_WHITE` (1) or `CB_BLACK` (0). |
| `castles` | `u8` | Castling rights bitmask. Bit 0 = white kingside, bit 1 = white queenside, bit 2 = black kingside, bit 3 = black queenside. |
| `w_en_passant_flag` | `i8` | Column (0-7) of the white pawn that just pushed two squares, or -1. |
| `b_en_passant_flag` | `i8` | Column (0-7) of the black pawn that just pushed two squares, or -1. |
| `fifty_moves` | `u8` | Half-move clock for the fifty-move rule. |
| `rep` | `cb_rep_struct *` | Pointer to the repetition table. |

### cb_move

A `u32` that packs four bytes of information about a move:

```
bits  0- 7  : source square index (0-63)
bits  8-15  : destination square index (0-63)
bits 16-23  : moving piece type
bits 24-31  : move flag (see Move Flags below)
```

Use `cb_create_move(src, dst, piece, flag)` to build one, and `cb_get_m_int(m, byte_index)` to read a field back out.

### cb_list_move

Holds up to 240 moves. After calling `cb_legal_moves` or `cb_pseudo_legal_moves`, iterate from 0 to `size - 1`.

---

## Move Flags

Every `cb_move` carries a flag in its upper byte that describes what kind of move it is. This is the main way the engine communicates metadata: whether a move is a capture, which piece was captured, whether it is a promotion, a castle, etc. You will want to check this flag in your move ordering or evaluation.

| Constant | Value | Meaning |
|---|---|---|
| `CB_ROOK` | 0 | Promotion to rook (doubles as piece index) |
| `CB_KNIGHT` | 1 | Promotion to knight (doubles as piece index) |
| `CB_BISHOP` | 2 | Promotion to bishop (doubles as piece index) |
| `CB_QUEEN` | 3 | Promotion to queen (doubles as piece index) |
| `CB_KING` | 4 | Piece index for the king, not used as a move flag |
| `CB_PAWN` | 5 | Piece index for a pawn, not used as a move flag |
| `CB_NO_PROM` | 6 | Normal move, no capture, no special case |
| `CB_EN_PASSANT` | 7 | En passant capture |
| `CB_CAPTURES_PAWN` | 8 | Captures a pawn |
| `CB_CAPTURES_BISHOP` | 9 | Captures a bishop |
| `CB_CAPTURES_KNIGHT` | 10 | Captures a knight |
| `CB_CAPTURES_ROOK` | 11 | Captures a rook |
| `CB_CAPTURES_QUEEN` | 12 | Captures a queen |
| `CB_CASTLE` | 13 | Kingside castling |
| `CB_LONG_CASTLE` | 14 | Queenside castling |

A few things worth knowing about how these flags work:

Promotions reuse the target piece constant as the flag (0 to 3), so the code detects a promotion by checking `flag < CB_NO_PROM`. There is no separate `CB_PROMOTION` constant.

One known gap: there is no way to tell from the flag alone whether a promotion also involves a capture. A pawn promoting on e8 and a pawn capturing on f8 and promoting both get the same flag (e.g. `CB_QUEEN`). If you need to distinguish them — for move ordering for instance — you have to check manually whether the destination square is occupied by an opponent piece in the board state.

For captures, the flag tells you which piece was taken but not by which piece — that is always in byte 2 of the move. There is no `CB_CAPTURES_KING` because capturing the king is never a legal move.

En passant gets its own flag because the captured pawn is not on the destination square, so the normal capture path does not handle it. `cb_make_move` branches on this case explicitly.

Castling encodes the king's destination square in the dst field (g1/g8 for kingside, c1/c8 for queenside), just like any other king move. The rook teleportation is handled inside `cb_make_move_castle`.

Example: reading the flag from a move

```c
cb_move m = moves.m[i];
u32 flag = cb_get_m_int(m, 3); // byte 3 = flag

if (flag == CB_CAPTURES_QUEEN)
    printf("captures the queen!\n");
else if (flag == CB_CASTLE)
    printf("kingside castle\n");
else if (flag < CB_NO_PROM)
    printf("promotion to piece %d\n", flag);
else if (flag == CB_NO_PROM)
    printf("quiet move\n");
```

---

## Plugging in a Search Function

The `cb_handle_go` function has a placeholder where you put your search. Just replace `cb_move m = 0;` with whatever you have:

```c
void cb_handle_go(cb_board *b, int nb_threads, const char *line) {
    // tc is already parsed from the go command (wtime, btime, depth, etc.)

    cb_move m = my_search(b, &tc, nb_threads); // your function here

    if (m == 0) { printf("bestmove 0000\n"); fflush(stdout); return; }
    char str[6];
    cb_move_to_str(m, str);
    printf("bestmove %s\n", str);
    fflush(stdout);
}
```

The `cb_time_control` struct passed to your search contains `wtime`, `btime`, `winc`, `binc`, `movestogo`, `movetime`, and `depth`, all parsed from the UCI `go` command.

---

## Piece Index Mapping

```
Index   Piece    Color
  0     Rook     Black
  1     Knight   Black
  2     Bishop   Black
  3     Queen    Black
  4     King     Black
  5     Pawn     Black
  6     Rook     White
  7     Knight   White
  8     Bishop   White
  9     Queen    White
 10     King     White
 11     Pawn     White
```

---

## Square Encoding

Squares are numbered 0-63 from A1 (0) to H8 (63). Bit `i` of a bitboard corresponds to square `i`. Named constants like `CB_A1`, `CB_E4`, `CB_H8` are available in the header.

---

## Known Limitations

- Move list capacity is fixed at 240. The theoretical maximum for a legal chess position is 218, so there is some headroom, but keep it in mind if you are generating moves from unusual positions.
- The repetition table is a circular buffer of 150 entries. It resets on captures and pawn moves which is correct, but in extremely long games without either of those events the oldest hashes get silently overwritten.
- No search or evaluation is included. This is intentionally just the game logic layer.

---

## License

MIT License — see [LICENSE](LICENSE)
