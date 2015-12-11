#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "bit_process.h"

/* �Ե�ǰ������� */
void bit_play_move(bitboard *bb, int i, int j, intersection color)
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

/* ��13*13��char����ת��Ϊ��Ӧ��13��int��λ���� */
void bit_convert(intersection *b, bitboard *bb)
{
	int i;
	for (i = 0; i < MAX_BOARDSIZE; ++i)
	{
		bit_play_move(bb, I(i), J(i), b[i]);
	}
}

/* �Ƚ�����λ�����Ƿ���ȫ��ͬ����ͬ����1�����򷵻�0 */
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

/* �������е�λ�õ���ɫ��������Ҳ���� */
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
				if (state != EMPTY && state != temp_board[k][i][j])
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

/* ��ɫ�ոպ��෴ */
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
				if (state != EMPTY && state == temp_board[k][i][j])
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

/* �ж�ĳ��λ�õ�״̬ */
int bit_get_state(bitboard *bb, int i, int j)
{
	int t1 = (bb[i] >> (2 * j)) & 1;
	if (t1==1)
		return WHITE;
	int t2 = (bb[i] >> (2 * j + 1)) & 1;
	if (t2==1)
		return BLACK;
	else
		return EMPTY;
}

/* �ж�ĳ��λ�õ�״̬ */
int bit_get_state_pos(bitboard *bb, int pos)
{
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
