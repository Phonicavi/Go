#ifndef _ZOBRIST_

#include "board.h"

/* Î»´¦Àí */
void bit_play_move(bitboard *bb, int i, int j, intersection color);
void bit_convert(intersection *b, bitboard *bb);
int bit_compare(bitboard *bb1, bitboard *bb2);
int bit_compare_2(bitboard *bb, int temp_board[][7][7]);
int bit_compare_2_conver(bitboard *bb, int temp_board[][7][7]);
int bit_get_state(bitboard *bb, int i, int j);
int bit_get_state_pos(bitboard *bb, int pos);

#endif