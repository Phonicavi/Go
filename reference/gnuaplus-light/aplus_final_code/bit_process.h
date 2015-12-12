#ifndef _ZOBRIST_

#include "board.h"
/* Zobrist 佐布里斯特哈希 */
// 由board.h中的定义
// bitboard 是 unsigned int 类型的数组 board_size
// 数组中的每一位 存的是 board_size*2 位的二进制数 表示该行或列的情况

// intersection 类型的棋盘的存储方法 是开一个长度169的数组 记作*b
// 第i个位置的char表示的就是坐标为[I(i), J(i)]的棋子的颜色
// 其中I(pos) J(pos) 是board.h中宏定义里 将线性的第pos个位置转成I, J坐标的方法
// b[pos]的值为 0 1 或 2 分别对应board.h中宏定义里 EMPTY WHITE 或 BLACK

/* 位处理 */
void bit_play_move(bitboard *bb, int i, int j, intersection color);
void bit_convert(intersection *b, bitboard *bb);
int bit_compare(bitboard *bb1, bitboard *bb2);
int bit_compare_2(bitboard *bb, int temp_board[][7][7]);
int bit_compare_2_conver(bitboard *bb, int temp_board[][7][7]);
int bit_get_state(bitboard *bb, int i, int j);
int bit_get_state_pos(bitboard *bb, int pos);

#endif