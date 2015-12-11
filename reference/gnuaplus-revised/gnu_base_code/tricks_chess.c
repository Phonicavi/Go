#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "aplus.h"
#include "tricks_chess.h"
#include "bit_process.h"

/***********************************************************************************
*提高棋力用到的一些小技巧
*
***********************************************************************************/



/***********************************************************************************
* 模块：前几步布局
*
* 主函数：
* int get_fuseki_pos(board_status *bs, intersection color)
*
* 工具：
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
* 模块：优先找能做活的地方做活（需要是真眼，三二一）
*
* 主函数：
* *int get_all_alive_pos(board_status *bs, intersection color)
*
* 工具：
*
*
***********************************************************************************/

//未加速
void get_all_alive_pos(board_status *bs, intersection color, int result[])
{
	int i, j, ai, aj, k, flag=0;
	int fill[2];

	//三：棋盘内部，一个眼需要三个角
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
						if (flag > 2)//已经有两个了，这是第三个
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

	//二：棋盘边上，一个眼需要两个角
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


	//一：棋盘角落，一个眼需要一个角
	//暂时不太有必要

}

/***********************************************************************************
* 模块：吃子（提子）和救子（叫吃：“提子”前一手。在对方落下一手之前不赶快往外逃，就会被对方提吃的情况）
*		和紧气（在吃对方子之前，减少对方气的数目）
* 主函数：
* *void get_all_eat_save_lib_pos(board_status *bs, intersection color, int result[])
*	*描述：看下某个位置pos能吃掉对方多少颗子或救活自己多少颗子或能紧气多少子
*	*检查pos的上下左右，看能不能吃或者救
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
				//检查会不会和之前的点在同一个棋串
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
					if (father[t] != -1 && father[k] == father[t])
					{
					father[k] = -1;
						break;
					}
				if (father[k] == -1)
					continue;

				//吃别人
				if (bs->board[POS(bi, bj)] == OTHER_COLOR(color))
				{
					result[pos] += SCORE_EAT_EACH * (bs->string_stones[bs->string_index[father[k]]]);
				}
				//救自己
				else if (bs->board[POS(bi, bj)] == color)
				{
					if (sim_for_string(bs, pos, color))
					{
						result[pos] += SCORE_SAVE_EACH * (bs->string_stones[bs->string_index[father[k]]]);
					}
					else
					{
						//如果救了之后也还是一口气
					}
					
				}
			}
			//如果另外还有气，判断是不是下了这颗子之后它就只剩一口气了，如果是就紧气
			else if (bs->board[POS(bi, bj)] == OTHER_COLOR(color) && sim_for_string(bs, pos, color) == 0)
			{
				//检查会不会和之前的点在同一个棋串
				father[k] = get_father(bs, POS(bi, bj));
				for (t = 0; t < k; ++t)
					if (father[t] != -1 && father[k] == father[t])
					{
					father[k] = -1;
					break;
					}
				if (father[k] == -1)
					continue;

				//紧气
				result[pos] += SCORE_APPROACH_EACH * (bs->string_stones[bs->string_index[father[k]]]);
			}
		}
	}
}

//提子（吃子）
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

//叫吃（救子）
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

//紧气
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
* 模块：避免填眼
*
* 主函数：
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
* 模块：连接和切断
*
* 主函数：
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

		//如果对方可以下这里，我们就看看这里能不能让我们连接，如果对方不能下这里就没必要接上了，也说明已经被切断了
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
					if (father[t] != -1 && father[t] == father[k])//同一个串
					{
						father[k] = -1;
						break;
					}
					else if (father[t] != -1 
						&& bs->board[POS(ai+deltai[t],aj+deltaj[t])]==bs->board[POS(bi,bj)])//也就是father[t] != father[k]
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

		//如果对方可以下这里，我们就看看这里能不能让我们连接，如果对方不能下这里就没必要接上了，也说明已经被切断了
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
					if (father[t] != -1 && father[t] == father[k])//同一个串
					{
						father[k] = -1;
						break;
					}
					else if (father[t] != -1
						&& bs->board[POS(ai + deltai[t], aj + deltaj[t])] == bs->board[POS(bi, bj)])//也就是father[t] != father[k]
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
* 模块：前面几步不能下在边界上
*
* 主函数：
* *void get_one_boundary_pos(board_status *bs, int result[])
* *void get_two_boundary_pos(board_status *bs, int result[])
*
***********************************************************************************/

//如果倒数第二层是空的，最后一层不能下
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

//如果倒数第三层是空的，最后两层不能下
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
* 模块：找四周都是空的点
*
* 主函数：
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
* 模块：做眼
*
* 主函数：
* *void get_all_nakade_pos(board_status *bs, intersection color, int result[])
* *int get_nakade_pos(board_status *bs, intersection color, intersection color_current)
*
***********************************************************************************/

//不让对方做眼，或者自己做眼
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

//做眼(做color的眼，当前是color_current在下子)
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