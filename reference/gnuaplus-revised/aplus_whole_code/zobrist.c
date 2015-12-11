#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "zobrist.h"

/***********************************************************************************
* ģ�飺Zobrist��ֵ
*
* ���ߣ�
* *U64 rand64()
*	*����������64λ�������
*
* *void init_zobrist()
*	*��ʼ��zobrist���飬����������
*
* *U64 get_zobrist_from_bitboard(bitboard bb)
*	*����λ�����ʾ������
*
* *U64 get_updated_zobrist(U64 z, int i, int j, intersection color)
* *U64 get_updated_zobrist(U64 z, int pos, intersection color)
*	*��������һ���ӻ�������������zobristֵ
*
*
***********************************************************************************/
U64 rand64()
{
	//srand((unsigned)time(NULL));
	return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

void init_zobrist()
{
	int i, j;
	for (i = 0; i < MAX_BOARDSIZE; ++i)
	{
		zobrist[0][i] = rand64();
		zobrist[1][i] = rand64();
	}
}


U64 get_zobrist_from_bitboard(bitboard bb)
{
	int i;
	U64 z = 0;
	intersection color;
	for (i = 0; i<MAX_BOARDSIZE; ++i)
	{
		color = bit_get_state(bb, I(i), J(i));
		z = z^zobrist[color - 1][i];
	}
	return z;
}

U64 get_zobrist_from_board(board_status *bs)
{
	int i;
	U64 z = 0;
	for (i = 0; i < MAX_BOARDSIZE; ++i)
	{
		z = z^zobrist[bs->board[i] - 1][i];
	}
	return z;
}

U64 get_updated_zobrist2(U64 z, int i, int j, intersection color)
{
	return z^zobrist[color - 1][POS(i,j)];
}

U64 get_updated_zobrist(U64 z, int pos, intersection color)
{
	return z^zobrist[color - 1][pos];
}
