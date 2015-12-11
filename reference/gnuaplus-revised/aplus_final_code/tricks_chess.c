#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "aplus.h"
#include "tricks_chess.h"
#include "bit_process.h"

/***********************************************************************************
*��������õ���һЩС����
*
***********************************************************************************/



/***********************************************************************************
* ģ�飺ǰ��������
*
* ��������
* int get_fuseki_pos(board_status *bs, intersection color)
*
* ���ߣ�
*
*
***********************************************************************************/
int get_fuseki_pos(board_status *bs, intersection color)
{
	int i, j;
	switch (bs->path_top)
	{
	case -1:
		i = 3; j = 3;
		if (bs->board[POS(i,j)] == EMPTY)
		{
			return POS(i, j);
		}
		else if (bs->board[POS(i+1, j-1)] == EMPTY)
		{
			return POS(i + 1, j - 1);
		}
		return -14;
	case 0:
		i = 3; j = 9;
		if (bs->board[POS(i, j)] == EMPTY)
		{
			return POS(i, j);
		}
		else if (bs->board[POS(i+1, j+1)] == EMPTY)
		{
			return POS(i + 1, j + 1);
		}
		return -14;
	case 1:
		i = 9; j = 3;
		if (bs->board[POS(i, j)] == EMPTY)
		{
			return POS(i, j);
		}
		else if (bs->board[POS(i+1, j+1)] == EMPTY)
		{
			return POS(i + 1, j + 1);
		}
		return -14;
	case 2:
		i = 9; j = 9;
		if (bs->board[POS(i, j)] == EMPTY)
		{
			return POS(i, j);
		}
		else if (bs->board[POS(i-1, j-1)] == EMPTY)
		{
			return POS(i - 1, j - 1);
		}
		return -14;
	case 3:

		return -14;
	case 4:

		return -14;
	case 5:

		return -14;
	case 6:

		return -14;
	}
	return -14;
}

/***********************************************************************************
* ģ�飺������������ĵط������Ҫ�����ۣ�����һ��
*
* ��������
* *int get_all_alive_pos(board_status *bs, intersection color)
*
* ���ߣ�
*
*
***********************************************************************************/

//δ����
void get_all_alive_pos(board_status *bs, intersection color, int result[])
{
	int i, j, ai, aj, k, flag=0;
	int fill[2];

	//���������ڲ���һ������Ҫ������
	for (i = 1; i < MAX_BOARD-1; ++i)
		for (j = 1; j < MAX_BOARD-1; ++j)
	{
		if (bs->board[POS(i,j)] != EMPTY)
			continue;

		flag = 0;
		for (ai = -1; ai <= 1; ++ai)
		{
			for (aj = -1; aj <= 1; ++aj)
			{
				if (ai != 0 || aj != 0)
				{
					if (is_legal_move(bs, OTHER_COLOR(color), POS(i + ai, j + aj)))
					{
						fill[flag] = POS(i + ai, j + aj);
						flag++;
						if (flag > 2)//�Ѿ��������ˣ����ǵ�����
						{
							break;
						}
					}
				}
			}
			if (flag > 2)
				break;
		}
		if (flag <= 2)
		{
			for (k = 0; k < flag; ++k)
				if (result[fill[k]] >= 0)
					result[fill[k]] += SCORE_REAL_TYE_EACH;
		}
	}

	int di[5], dj[5];

	//�������̱��ϣ�һ������Ҫ������
	for (i = 1; i < MAX_BOARD-1; ++i)
	{
		if (bs->board[POS(0, i)] == EMPTY)
		{
			int pos[5] = { POS(0,i-1), POS(1,i-1), POS(1,i), POS(1,i+1), POS(0,i+1) };
			flag = 0;
			for (k = 0; k < 5; ++k)
			{
				if (is_legal_move(bs, OTHER_COLOR(color), pos[k]))
				{
					fill[flag] = pos[k];
					flag ++;
					if (flag >1)
						break;
				}
			}
			if (flag == 1 && result[fill[0]] >= 0)
				result[fill[0]] += SCORE_REAL_TYE_EACH;
		}

		if (bs->board[POS(MAX_BOARD-1, i)] == EMPTY)
		{
			int pos[5] = { POS(MAX_BOARD - 1, i - 1), POS(MAX_BOARD - 2, i - 1), POS(MAX_BOARD - 2, i), POS(MAX_BOARD - 2, i + 1), POS(MAX_BOARD - 1, i + 1) };
			flag = 0;
			for (k = 0; k < 5; ++k)
			{
				if (is_legal_move(bs, OTHER_COLOR(color), pos[k]))
				{
					fill[flag] = pos[k];
					flag++;
					if (flag >1)
						break;
				}
			}
			if (flag == 1 && result[fill[0]] >= 0)
				result[fill[0]] += SCORE_REAL_TYE_EACH;
		}

		if (bs->board[POS(i, 0)] == EMPTY)
		{
			int pos[5] = { POS(i-1,0), POS(i - 1,1), POS(i,1), POS(i + 1,1), POS(i + 1,0) };
			flag = 0;
			for (k = 0; k < 5; ++k)
			{
				if (is_legal_move(bs, OTHER_COLOR(color), pos[k]))
				{
					fill[flag] = pos[k];
					flag++;
					if (flag >1)
						break;
				}
			}
			if (flag == 1 && result[fill[0]] >= 0)
				result[fill[0]] += SCORE_REAL_TYE_EACH;
		}

		if (bs->board[POS(i,MAX_BOARD - 1)] == EMPTY)
		{
			int pos[5] = { POS(i - 1,MAX_BOARD - 1), POS(i - 1,MAX_BOARD - 2), POS(i,MAX_BOARD - 2), POS(i+1,MAX_BOARD - 2), POS(i+1,MAX_BOARD - 1) };
			flag = 0;
			for (k = 0; k < 5; ++k)
			{
				if (is_legal_move(bs, OTHER_COLOR(color), pos[k]))
				{
					fill[flag] = pos[k];
					flag++;
					if (flag >1)
						break;
				}
			}
			if (flag == 1 && result[fill[0]] >= 0)
				result[fill[0]] += SCORE_REAL_TYE_EACH;
		}
	}


	//һ�����̽��䣬һ������Ҫһ����
	//��ʱ��̫�б�Ҫ

}

/***********************************************************************************
* ģ�飺���ӣ����ӣ��;��ӣ��гԣ������ӡ�ǰһ�֡��ڶԷ�����һ��֮ǰ���Ͽ������ӣ��ͻᱻ�Է���Ե������
*		�ͽ������ڳԶԷ���֮ǰ�����ٶԷ�������Ŀ��
* ��������
* *void get_all_eat_save_lib_pos(board_status *bs, intersection color, int result[])
*	*����������ĳ��λ��pos�ܳԵ��Է����ٿ��ӻ�Ȼ��Լ����ٿ��ӻ��ܽ���������
*	*���pos���������ң����ܲ��ܳԻ��߾�
*
***********************************************************************************/

void get_all_eat_save_lib_pos(board_status *bs, intersection color, int result[])
{
	int ai, aj, bi, bj;
	int pos,k,t;
	int father[4];
	for (pos = 0; pos < MAX_BOARDSIZE; ++pos)
	{
		if (result[pos] < 0)
			continue;

		ai = I(pos);
		aj = J(pos);

		for (k = 0; k < 4; ++k)
		{
			father[k] = -1;
			bi = ai + deltai[k];
			bj = aj + deltaj[k];
			if (!ON_BOARD(bi, bj) || bs->board[POS(bi, bj)] == EMPTY)
				continue;
			if (!has_additional_liberty_game(bs, bi, bj, ai, aj))
			{
				//���᲻���֮ǰ�ĵ���ͬһ���崮
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
					if (father[t] != -1 && father[k] == father[t])
					{
					father[k] = -1;
						break;
					}
				if (father[k] == -1)
					continue;

				//�Ա���
				if (bs->board[POS(bi, bj)] == OTHER_COLOR(color))
				{
					result[pos] += SCORE_EAT_EACH * (bs->string_stones[bs->string_index[father[k]]]);
				}
				//���Լ�
				else if (bs->board[POS(bi, bj)] == color)
				{
					if (sim_for_string(bs, pos, color))
					{
						result[pos] += SCORE_SAVE_EACH * (bs->string_stones[bs->string_index[father[k]]]);
					}
					else
					{
						//�������֮��Ҳ����һ����
					}
					
				}
			}
			//������⻹�������ж��ǲ������������֮������ֻʣһ�����ˣ�����Ǿͽ���
			else if (bs->board[POS(bi, bj)] == OTHER_COLOR(color) && sim_for_string(bs, pos, color) == 0)
			{
				//���᲻���֮ǰ�ĵ���ͬһ���崮
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
					if (father[t] != -1 && father[k] == father[t])
					{
					father[k] = -1;
					break;
					}
				if (father[k] == -1)
					continue;

				//����
				result[pos] += SCORE_APPROACH_EACH * (bs->string_stones[bs->string_index[father[k]]]);
			}
		}
	}
}

//���ӣ����ӣ�
int get_eat_pos(board_status *bs, intersection color)
{
	int i,j;
	for (i = 0; i < bs->num_of_strings; ++i)
	{
		if (bs->approximate_liberty_real[i] != 1 || bs->string_color[i]==color)
			continue;
		for (j = 0; j < bs->legal_moves_num; ++j)
		{
			if (bs->provide_lib[bs->legal_moves[j]][i])
				return bs->legal_moves[j];
		}
	}
	return -14;
}

//�гԣ����ӣ�
int get_atari_pos(board_status *bs, intersection color)
{
	int i, j;
	int pos,k;
	int bi, bj;
	int cnt;
	for (i = 0; i < bs->num_of_strings; ++i)
	{
		if (bs->approximate_liberty_real[i] != 1 || bs->string_color[i] == OTHER_COLOR(color))
			continue;
		for (j = 0; j < bs->legal_moves_num; ++j)
		{
			pos = bs->legal_moves[j];
			if (bs->provide_lib[pos][i])
			{
				cnt = 0;
				for (k = 0; k < 4; ++k)
				{
					bi = I(pos) + deltai[k];
					bj = J(pos) + deltaj[k];
					if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] == EMPTY)
					{
						cnt++;
						if (cnt >= 2)
						{
							return pos;
						}
					}
				}
			}
		}
	}
	return -14;
}

//����
int get_approach_pos(board_status *bs, intersection color)
{
	int i, j;
	for (i = 0; i < bs->num_of_strings; ++i)
	{
		if (bs->approximate_liberty_real[i] != 2 || bs->string_color[i] == color)
			continue;
		for (j = 0; j < bs->legal_moves_num; ++j)
		{
			if (bs->provide_lib[bs->legal_moves[j]][i])
				return bs->legal_moves[j];
		}
	}
	return -14;
}

/***********************************************************************************
* ģ�飺��������
*
* ��������
* *void get_ban_fill_eye_pos(board_status *bs, intersection color, int result[])
*
*
***********************************************************************************/

void get_ban_fill_eye_pos(board_status *bs, intersection color, int result[])
{
	int pos,i,j,ai,aj,cnt;
	for (i = 0; i < MAX_BOARD; ++i)
		for (j = 0; j < MAX_BOARD; ++j)
	{
		pos = POS(i,j);
		if (result[pos] < 0)
			continue;

		cnt = 0;
		for (ai = -1; ai <= 1; ++ai)
			for (aj = -1; aj <= 1; ++aj)
		{
			if (ai == 0 && aj == 0)
				continue;
			if (!ON_BOARD(i + ai, j + aj) || bs->board[POS(i + ai, j + aj)] == color)
				cnt++;
		}
		if (cnt == 8)
			result[POS(i, j)] = -1;
	}
}

/***********************************************************************************
* ģ�飺���Ӻ��ж�
*
* ��������
* *void get_all_connect_cut_pos(board_status *bs, intersection color, int result[])
* *int get_connect_cut_pos(board_status *bs, intersection color)
*
***********************************************************************************/

void get_all_connect_cut_pos(board_status *bs, intersection color, int result[])
{
	int ai, aj, bi, bj;
	int pos, k, t,tmp;
	int father[4];
	for (pos = 0; pos < MAX_BOARDSIZE; ++pos)
	{
		if (result[pos] < 0)
			continue;

		ai = I(pos);
		aj = J(pos);

		//����Է�������������ǾͿ��������ܲ������������ӣ�����Է������������û��Ҫ�����ˣ�Ҳ˵���Ѿ����ж���
		if (is_legal_move(bs, OTHER_COLOR(color), pos))
		{
			for (k = 0; k < 4; ++k)
			{
				father[k] = -1;
				bi = ai + deltai[k];
				bj = aj + deltaj[k];
				if (!ON_BOARD(bi, bj) || bs->board[POS(bi, bj)] == EMPTY)
					continue;
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
				{
					if (father[t] != -1 && father[t] == father[k])//ͬһ����
					{
						father[k] = -1;
						break;
					}
					else if (father[t] != -1 
						&& bs->board[POS(ai+deltai[t],aj+deltaj[t])]==bs->board[POS(bi,bj)])//Ҳ����father[t] != father[k]
					{
						tmp = bs->string_stones[bs->string_index[father[k]]] + bs->string_stones[bs->string_index[father[t]]];
						if (tmp >= CONNECT_CUT_MIN_NUM)
						{
							if (bs->board[POS(bi, bj)] == color)
								result[pos] += SCORE_CONNECT_EACH * (tmp);
							else
								result[pos] += SCORE_CUT_EACH * (tmp);
							break;
						}
					}
				}
			}
		}
	}
}

int get_connect_cut_pos(board_status *bs, intersection color)
{
	int ai, aj, bi, bj;
	int i,pos, k, t, tmp;
	int father[4];
	for (i = 0; i < bs->legal_moves_num; ++i)
	{
		pos = bs->legal_moves[i];
		ai = I(pos);
		aj = J(pos);

		//����Է�������������ǾͿ��������ܲ������������ӣ�����Է������������û��Ҫ�����ˣ�Ҳ˵���Ѿ����ж���
		if (is_legal_move(bs, OTHER_COLOR(color), pos))
		{
			for (k = 0; k < 4; ++k)
			{
				father[k] = -1;
				bi = ai + deltai[k];
				bj = aj + deltaj[k];
				if (!ON_BOARD(bi, bj) || bs->board[POS(bi, bj)] == EMPTY)
					continue;
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
				{
					if (father[t] != -1 && father[t] == father[k])//ͬһ����
					{
						father[k] = -1;
						break;
					}
					else if (father[t] != -1
						&& bs->board[POS(ai + deltai[t], aj + deltaj[t])] == bs->board[POS(bi, bj)])//Ҳ����father[t] != father[k]
					{
						tmp = bs->string_stones[bs->string_index[father[k]]] + bs->string_stones[bs->string_index[father[t]]];
						if (tmp >= CONNECT_CUT_MIN_NUM)
						{
							return pos;
						}
					}
				}
			}
		}
	}
	return -14;
}

/***********************************************************************************
* ģ�飺ǰ�漸���������ڱ߽���
*
* ��������
* *void get_one_boundary_pos(board_status *bs, int result[])
* *void get_two_boundary_pos(board_status *bs, int result[])
*
***********************************************************************************/

//��������ڶ����ǿյģ����һ�㲻����
void get_one_boundary_pos(board_status *bs, int result[])
{
	int i,empty;
	empty = 1;
	for (i = 0;i<MAX_BOARD;++i)
		if (bs->board[POS(1, i)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
			result[POS(0, i)] = -1;
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(i, 1)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
			result[POS(i, 0)] = -1;
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(MAX_BOARD-2, i)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
			result[POS(MAX_BOARD-1, i)] = -1;
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(i, MAX_BOARD-2)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
			result[POS(i,MAX_BOARD-1)] = -1;
	}
}

//��������������ǿյģ�������㲻����
void get_two_boundary_pos(board_status *bs, int result[])
{
	int i, empty;
	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(2, i)] != EMPTY || bs->board[POS(1, i)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
		{
			result[POS(0, i)] = -1;
			result[POS(1, i)] = -1;
		}
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(i, 2)] != EMPTY || bs->board[POS(i, 1)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
		{
			result[POS(i, 0)] = -1;
			result[POS(i, 1)] = -1;
		}
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(MAX_BOARD - 3, i)] != EMPTY || bs->board[POS(MAX_BOARD - 2, i)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
		{
			result[POS(MAX_BOARD - 1, i)] = -1;
			result[POS(MAX_BOARD - 2, i)] = -1;
		}
	}

	empty = 1;
	for (i = 0; i<MAX_BOARD; ++i)
		if (bs->board[POS(i, MAX_BOARD - 3)] != EMPTY || bs->board[POS(i, MAX_BOARD - 2)] != EMPTY)
		{
		empty = 0;
		break;
		}
	if (empty)
	{
		for (i = 0; i < MAX_BOARD; ++i)
		{
			result[POS(i, MAX_BOARD - 1)] = -1;
			result[POS(i, MAX_BOARD - 2)] = -1;
		}
	}
}

/***********************************************************************************
* ģ�飺�����ܶ��ǿյĵ�
*
* ��������
* *void get_all_fill_board_pos(board_status *bs, int result[])
* *int get_fill_board_pos(board_status *bs, intersection color)
*
***********************************************************************************/

void get_all_fill_board_pos(board_status *bs, int result[])
{
	int i,j;
	int bi, bj;
	int ai[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int aj[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int flag;

	for (i = 0; i < MAX_BOARDSIZE; ++i)
	{
		if (result[i] <0)
			continue;

		flag = 1;
		for (j = 0; j < 8; ++j)
		{
			bi = I(i) + ai[j];
			bj = J(i) + aj[j];
			if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] != EMPTY)
			{
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			result[i] += SCORE_FILL_BOARD;
		}
	}
}

int get_fill_board_pos(board_status *bs, intersection color)
{
	int i, j;
	int ai,aj,bi, bj;
	int flag;

	for (i = 0; i < bs->legal_moves_num; ++i)
	{
		flag = 1;
		ai = I(bs->legal_moves[i]);
		aj = J(bs->legal_moves[i]);
		for (j = 0; j < 4; ++j)
		{
			bi = ai + deltai[j];
			bj = aj + deltaj[j];
			if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] != EMPTY)
			{
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			return i;
		}
	}
	return -14;
}

/***********************************************************************************
* ģ�飺����
*
* ��������
* *void get_all_nakade_pos(board_status *bs, intersection color, int result[])
* *int get_nakade_pos(board_status *bs, intersection color, intersection color_current)
*
***********************************************************************************/

//���öԷ����ۣ������Լ�����
void get_all_nakade_pos(board_status *bs, intersection color, int result[])
{
	int pos,i;
	int bi, bj;
	int empty_num, other_num, our_num;
	int flag;
	int emptyi, emptyj;
	int otheri[3], otherj[3];
	int ouri[3], ourj[3];

	for (pos = 0; pos < MAX_BOARDSIZE; ++pos)
	{
		if (bs->board[pos]!=EMPTY)
			continue;
		empty_num = 0;
		other_num = 0;
		our_num = 0;
		flag = 1;
		for (i = 0; i < 4; ++i)
		{
			bi = I(pos) + deltai[i];
			bj = J(pos) + deltaj[i];
			if (!ON_BOARD(bi, bj))
			{
				other_num++;
				our_num++;
				continue;
			}
			else if (bs->board[POS(bi, bj)] == EMPTY)
			{
				if (empty_num > 0 || !is_legal_move(bs,color,POS(bi,bj)))
				{
					flag = 0;
					break;
				}
				else
				{
					empty_num++;
					emptyi = bi;
					emptyj = bj;
				}
			}
			else if (bs->board[POS(bi, bj)] == OTHER_COLOR(color))
			{
				if (other_num >= 3)
				{
					flag = 0;
					break;
				}
				else
				{
					otheri[other_num] = bi;
					otherj[other_num] = bj;
					other_num++;
				}
			}
			else if (bs->board[POS(bi, bj)] == color)
			{
				if (our_num >= 3)
				{
					flag = 0;
					break;
				}
				else
				{
					ouri[our_num] = bi;
					ourj[our_num] = bj;
					our_num++;
				}
			}
		}
		if (flag && (other_num == 3 || our_num == 3) && result[POS(emptyi, emptyj)]>=0)
		{
			result[POS(emptyi, emptyj)] += SCORE_NAKADE;
		}
	}
}

//����(��color���ۣ���ǰ��color_current������)
int get_nakade_pos(board_status *bs, intersection color, intersection color_current)
{
	int pos, i;
	int bi, bj;
	int empty_num, our_num;
	int emptyi, emptyj;

	for (pos = 0; pos < MAX_BOARDSIZE; ++pos)
	{
		if (bs->board[pos] != EMPTY)
			continue;
		empty_num = 0;
		our_num = 0;
		for (i = 0; i < 4; ++i)
		{
			bi = I(pos) + deltai[i];
			bj = J(pos) + deltaj[i];
			if (!ON_BOARD(bi, bj))
			{
				our_num++;
				continue;
			}
			if (bs->board[POS(bi, bj)] == EMPTY)
			{
				if (empty_num > 0)
				{
					empty_num = 0;
					break;
				}
				else
				{
					empty_num++;
					emptyi = bi;
					emptyj = bj;
				}
			}
			else if (bs->board[POS(bi, bj)] == OTHER_COLOR(color))
				break;
			else // if (bs->board[POS(bi, bj)] == color)
				our_num++;
		}
		if (empty_num==1 && our_num == 3)
		{
			return POS(emptyi, emptyj);
		}
	}
	return -14;
}