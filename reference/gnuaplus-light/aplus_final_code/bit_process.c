#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "bit_process.h"

/* 对当前格局下子 */
void bit_play_move(bitboard *bb, int i, int j, intersection color)
// 用相邻的两位表示一个棋子
// 移动 2*j 		就是...xx01xx...
// 移动 2*j+1	就是...xx10xx...
// 如果是空白 则反转两次可以变回
// 					...xx00xx...
{
	if (color == WHITE)
	{
		bb[i] = bb[i] | (1 << (2 * j));
	}
	else if (color == BLACK)
	{
		bb[i] = bb[i] | (1 << (2 * j + 1));
	}
	else
	{
		bb[i] = bb[i] & ~(1 << (2 * j));
		bb[i] = bb[i] & ~(1 << (2 * j + 1));
	}
}

/* 将13*13的char数组转化为相应的13的int的位数组 */
void bit_convert(intersection *b, bitboard *bb)
{
	int i;
	for (i = 0; i < MAX_BOARDSIZE; ++i) // MAX_BOARDSIZE = 13^2 = 169
	{ // 对位将intersection方法数组的b的每一位按颜色处理到bitboard里
		bit_play_move(bb, I(i), J(i), b[i]);
	}
}

/* 比较两个位数组是否完全相同，相同返回1，否则返回0 */
int bit_compare(bitboard *bb1, bitboard *bb2)
{
	int i;
	for (i = 0; i < MAX_BOARD; ++i)
	{
		if (bb1[i] != bb2[i])
		{
			return 0;
		}
	}
	return 1;
}

// 这两个bit_compare*函数 用于匹配当前局 bitboard和temp_board 可能是棋谱
// 是否有重合的图样

/* 棋谱上有的位置的颜色，棋盘上也会有 */
int bit_compare_2(bitboard *bb, int temp_board[][7][7])
{
	int i, j, k,result;
	for (k = 0; k < 8; ++k)
	{
		result = 1;
		for (i = 0; i < 7; ++i)
		{
			for (j = 0; j < 7; ++j)
			{
				int state = bit_get_state(bb, i, j);
				if (state != EMPTY && state != temp_board[k][i][j]) // 区别 bit_compare_2 要求bit和棋谱temp上相同
				{
					result = 0;
					break;
				}
			}
			if (result == 0)
				break;
		}
		if (result == 1)
		{
			return 1;
		}
	}
	return 0;
}

/* 颜色刚刚好相反 */
int bit_compare_2_conver(bitboard *bb, int temp_board[][7][7])
{
	int i, j, k, result;
	for (k = 0; k < 8; ++k)
	{
		result = 1;
		for (i = 0; i < 7; ++i)
		{
			for (j = 0; j < 7; ++j)
			{
				int state = bit_get_state(bb, i, j);
				if (state != EMPTY && state == temp_board[k][i][j]) // 区别 bit_compare_2 要求bit和棋谱temp上相反
				{
					result = 0;
					break;
				}
			}
			if (result == 0)
				break;
		}
		if (result == 1)
		{
			return 1;
		}
	}
	return 0;
}

/* 判断某个位置的状态 */
int bit_get_state(bitboard *bb, int i, int j)
{
	// 取出指定的位
	// 判断指定位的值 ....x2x1....
	// x1是1则是01			WHITE
	// x2是1则是10			BLACK
	// 否则x1 x2都是0 则是00	EMPTY
	int t1 = (bb[i] >> (2 * j)) & 1;
	if (t1==1)
		return WHITE;
	int t2 = (bb[i] >> (2 * j + 1)) & 1;
	if (t2==1)
		return BLACK;
	else
		return EMPTY;
}

/* 判断某个位置的状态 */
int bit_get_state_pos(bitboard *bb, int pos)
{
	// 先将线性位置pos转成坐标[I, J]
	// 之后的动作和 bit_get_state() 一样

	// 取出指定的位
	// 判断指定位的值 ....x2x1....
	// x1是1则是01			WHITE
	// x2是1则是10			BLACK
	// 否则x1 x2都是0 则是00	EMPTY
	int i = I(pos);
	int j = J(pos);
	int t1 = (bb[i] >> (2 * j)) & 1;
	if (t1==1)
		return WHITE;
	int t2 = (bb[i] >> (2 * j + 1)) & 1;
	if (t2==1)
		return BLACK;
	else
		return EMPTY;
}

