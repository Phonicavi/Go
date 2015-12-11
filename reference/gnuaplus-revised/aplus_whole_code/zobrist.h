#ifndef _ZOBRIST_

#include "board.h"
#include "uct.h"

/**********************************zobrist******************************************/

typedef long long U64;

U64 zobrist[2][MAX_BOARDSIZE];

U64 rand64();
void init_zobrist();
U64 get_zobrist_from_bitboard(bitboard bb);
U64 get_zobrist_from_board(board_status *bs);
U64 get_updated_zobrist2(U64 z, int i, int j, intersection color);
U64 get_updated_zobrist(U64 z, int pos, intersection color);



#endif