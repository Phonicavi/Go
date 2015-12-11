/*********************************************************************************
* Copyright(C),2014-2015,SJTU
* Program Assignment:	board
* FileName:	board.c
* Source code in：	gnu_base_code/board.c
* Author:	Cyy/ljj/Qxt/Zzh
* Version:	1.0
* Date:	2014/12/28
* Description:	Functions of board. 用于维护棋盘的操作
* Others:
* Function List:
*	void clear_board(board_status *bs);
*	int board_empty(board_status *bs);
*	int is_stone(board_status *bs, int pos);
*	int get_string(board_status *bs, int i, int j, int *stonei, int *stonej);
*	int legal_move(board_status *bs, int i, int j, intersection color);
*	int only_lib(board_status *bs, int string_idx);
*	int suicide(board_status *bs, int i, int j, intersection color);
*	void get_legal_moves(board_status *bs, intersection color);
*	int get_father(board_status *bs, int pos);
*	void play_move(board_status *bs, int i, int j, intersection color);
*	void compute_final_status(board_status *bs);
*	int get_final_status(board_status *bs, int i, int j);
*	void set_final_status(board_status *bs, int i, int j, int status);
*	double get_score(board_status *bs);
*	int valid_fixed_handicap(int handicap);
*	void place_fixed_handicap(board_status *bs, int handicap);
* History:
*	1.	Date:	2014/12/28
*		Author:	cyy
*		Modification:	基础代码
*	2.	Date:
*		Author:
*		Modification:
**********************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

/* The GTP specification leaves the initial board size and komi to the
 * discretion of the engine. We make the uncommon choices of 13x13 board
 * and komi -3.14.
 */
/* board size */
int board_size = 13;
/* number of board block board size * board size*/
int board_array_size = 169;
/* komi about score*/
float komi = -3.14;

/* Global variable: board status. */
board_status main_board;

/* Offsets for the four directly adjacent neighbors. Used for looping. */
int deltai[4] = { -1, 1, 0, 0 };
int deltaj[4] = { 0, 0, -1, 1 };


/*******************************************************
* Function Name:	clear_board
* Purpose:			Clean the board.
* Params :
*	@board_status(*)	bs		board status
* Return:				void
* Limitation:
*******************************************************/
void clear_board(board_status *bs) {
	int pos;
	memset(bs, 0, sizeof(*bs));

	//没必要的循环 cyy
	//for (pos = 0; pos < board_array_size; pos++){
	//	bs->father[pos] = -1;
	//	bs->legal_moves[pos] = -1;
	//}

	memset(bs->father, -1, board_array_size * sizeof(int));
	memset(bs->legal_moves, -1, board_array_size * sizeof(int));

	bs->last_move_pos = -1;
	bs->ko_pos = POS(-1, -1);

	bs->path_top = -1;
	bs->legal_moves_num = 0;
}

/*******************************************************
* Function Name:	board_empty
* Purpose:			Is the board empty?
* Params :
*	@board_status(*)	bs		board status
* Return:				int		0：not empty		1：empty
* Limitation:
*******************************************************/
int board_empty(board_status *bs) {
	int i;
	for (i = 0; i < MAX_BOARDSIZE; i++){
		if (bs->board[i] != EMPTY){
			return 0;
		}
	}
	return 1;
}

/*******************************************************
* Function Name:	is_stone
* Purpose:			Is there a stone in position (pos)?
* Params :
*	@board_status(*)	bs		board status
*	@int				pos		the index of a position
* Return:				int		0：false		1：true
* Limitation:
*******************************************************/
int is_stone(board_status *bs, int pos) {
	return ((bs->board[pos] == BLACK) || (bs->board[pos] == WHITE));
}

/* Get the stones of a string. stonei and stonej must point to arrays
 * sufficiently large to hold any string on the board. The number of
 * stones in the string is returned.
 */

/*******************************************************
* Function Name:	get_string
* Purpose:			Get the stones of a string.
* Params :
*	@board_status(*)	bs		board status
*	@int				i		起始stone position i
*	@int				j		起始stone position j
*	@int(*)				stonei	stonei[x] 棋串第x个棋子的i坐标
*	@int(*)				stonej	stonej[x] 棋串第x个棋子的j坐标
* Return:				int		The number of stones in the string
* Limitation:	根据查询起始stone的不同 stonei stonej会有不同，
*				但是依旧满足一一对应关系。
*******************************************************/
int get_string(board_status *bs, int i, int j, int *stonei, int *stonej) {
	//该棋串的棋子数目
	int num_stones = 0;
	//查询起始棋子的pos
	int pos = POS(i, j);
	do {
		stonei[num_stones] = I(pos);
		stonej[num_stones] = J(pos);
		num_stones++;
		//选取当前棋子的相连的下一个同色棋子
		pos = bs->next_stone[pos];
	} while (pos != POS(i, j));

	return num_stones;
}

/*******************************************************
* Function Name:	legal_move
* Purpose:			Is a move at (i, j) legal?
* Params :
*	@board_status(*)	bs		board status
*	@int				i		position i
*	@int				j		position j
*	@intersection		color	player's color
* Return:				int		1:legal	0:not legal
* Limitation:
*******************************************************/
int legal_move(board_status *bs, int i, int j, intersection color) {
	//对方棋子颜色
	intersection other = OTHER_COLOR(color);

	/* Pass is always legal. */
	if (PASS_MOVE(i, j)){
		return 1;
	}

	/* Already occupied. */
	if (bs->board[POS(i, j)] != EMPTY){
		return 0;
	}

	/* Illegal ko recapture. It is not illegal to fill the ko so we must
	 * check the color of at least one neighbor.
	 */
	if (i == bs->ko_i && j == bs->ko_j
		//这是要判断什么？？没懂 是规则？cyy
		&& ((ON_BOARD(i - 1, j) && bs->board[POS(i - 1, j)] == other) || (ON_BOARD(i + 1, j) && bs->board[POS(i + 1, j)] == other))){
		return 0;
	}

	return 1;
}

/*******************************************************
* Function Name:	has_additional_liberty
* Purpose:			Does the string at (i, j) have
*					any more liberty than the one
*					at (libi, libj)?
* Params :
*	@board_status(*)	bs		board status
*	@int				i		查询stone position i
*	@int				j		查询stone position j
*	@int				libi	源头stone position libi
*	@int				libj	源头stone position libj
* Return:				int		1:ture	0:false
* Limitation：	1. 源头stone用来查重，源头贡献的气不算
*				2. 需要查询 查询点所在的棋串内的全部棋子
*******************************************************/
int has_additional_liberty(board_status *bs, int i, int j, int libi, int libj) {

	//先假设在libi，libj位置上已经放置了棋子


	//查询stone的pos
	int lib;
	int pos = POS(i, j);
	int pos2;
	int k, bi, bj;
	for (k = 0; k < 4; ++k)
	{
		bi = libi + deltai[k];
		bj = libj + deltaj[k];
		pos2 = POS(bi, bj);
		if (ON_BOARD(bi, bj) && bs->board[pos2] != EMPTY){
			bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]] --;
		}
	}

	if (bs->approximate_liberty[bs->string_index[get_father(bs, pos)]] == 0)
		lib = 0;
	else
		lib = 1;

	for (k = 0; k < 4; ++k)
	{
		bi = libi + deltai[k];
		bj = libj + deltaj[k];
		pos2 = POS(bi, bj);
		if (ON_BOARD(bi, bj) && bs->board[pos2] != EMPTY){
			bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]] ++;
		}
	}
	return lib;

}



int has_additional_liberty_game(board_status *bs, int i, int j, int libi, int libj) {

	//先假设在libi，libj位置上已经放置了棋子


	//查询stone的pos
	int lib;
	int pos = POS(i, j);
	int pos2;
	int k, bi, bj;
	for (k = 0; k < 4; ++k)
	{
		bi = libi + deltai[k];
		bj = libj + deltaj[k];
		pos2 = POS(bi, bj);
		if (ON_BOARD(bi, bj) && bs->board[pos2] != EMPTY){
			bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]]--;
			bs->approximate_liberty_real[bs->string_index[get_father(bs, pos2)]]--;
		}

	}

	if (bs->approximate_liberty[bs->string_index[get_father(bs, pos)]] == 0){
		lib = 0;
	}
	else{
		lib = 1;
	}


	for (k = 0; k < 4; ++k)
	{
		bi = libi + deltai[k];
		bj = libj + deltaj[k];
		pos2 = POS(bi, bj);
		if (ON_BOARD(bi, bj) && bs->board[pos2] != EMPTY){
			bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]] ++;
			bs->approximate_liberty_real[bs->string_index[get_father(bs, pos2)]] ++;
		}

	}
	return lib;

}

/*******************************************************
* Function Name:	provides_liberty
* Purpose:			Does (ai, aj) provide a liberty
*					for a stone at (i, j)?
* Params :
*	@board_status(*)	bs		board status
*	@int				ai		around block position ai
*	@int				aj		around block position aj
*	@int				i		查询block position i
*	@int				j		查询block position j
*	@intersection		color	player's color
* Return:				int		1:ture	0:false
* Limitation: 判断(i, j)周边的位置(ai, aj)是否有气：
*				1. （ai, aj）上无棋子
*				2. （ai, aj）上是同色棋子 继续判断
*******************************************************/
int provides_liberty(board_status *bs, int ai, int aj, int i, int j, intersection color) {
	/* A vertex off the board does not provide a liberty. */
	if (!ON_BOARD(ai, aj)) {
		return 0;
	}

	/* An empty vertex IS a liberty. */
	if (bs->board[POS(ai, aj)] == EMPTY) {
		return 1;
	}

	/* A friendly string provides a liberty to (i, j) if it currently
	 * has more liberties than the one at (i, j).
	 */
	if (bs->board[POS(ai, aj)] == color) {
		//继续判断（ai, aj）所连得气数目
		return has_additional_liberty(bs, ai, aj, i, j);
	}

	/* An unfriendly string provides a liberty if and only if it is
	 * captured, i.e. if it currently only has the liberty at (i, j).
	 */
	return !has_additional_liberty(bs, ai, aj, i, j);
}


/*******************************************************
* Function Name:	provides_liberty_sim
* Purpose:			Does (ai, aj) provide a liberty
*					for a stone at (i, j)?
* Params :
*	@board_status(*)	bs		board status
*	@int				ai		around block position ai
*	@int				aj		around block position aj
*	@int				i		查询block position i
*	@int				j		查询block position j
*	@intersection		color	player's color
* Return:				int		提供气的数目
* Limitation: 判断(i, j)周边的位置(ai, aj)是否有气：
*				1. （ai, aj）上无棋子
*				2. （ai, aj）上是同色棋子 返回这一部分的气数
*				用来防止出现自己送子的情况 cyy
*******************************************************/
int provides_liberty_sim(board_status *bs, int ai, int aj, int i, int j, intersection color) {
	/* A vertex off the board does not provide a liberty. */
	if (!ON_BOARD(ai, aj)) {
		return 0;
	}

	/* An empty vertex IS a liberty. */
	if (bs->board[POS(ai, aj)] == EMPTY) {
		return 1;
	}

	/* A friendly string provides a liberty to (i, j) if it currently
	* has more liberties than the one at (i, j).
	*/
	if (bs->board[POS(ai, aj)] == color) {
		//继续判断（ai, aj）所连得真气数目
		return bs->approximate_liberty_real[bs->string_index[get_father(bs, POS(ai, aj))]] - 1;
	}

	/* An unfriendly string provides a liberty if and only if it is
	* captured, i.e. if it currently only has the liberty at (i, j).
	*/
	return 0;
}

/*******************************************************
* Function Name:	only_lib
* Purpose:			Get the position of the only liberty
*					for the stone string containing stone
at (i,j).
* Params :
*	@board_status(*)	bs				board status
*	@int				string_idx		position ai
* Return:				int				the position of
*										the only liberty
* Limitation:			-1 for more then 1 lib
*******************************************************/
//int only_lib(board_status *bs, int string_idx) {
//	int fpos, pos, lpos = -1;
//	int i, ai, aj, bi, bj;
//	int appr_lib, find_lib = 0;
//	appr_lib = bs->approximate_liberty_real[string_idx];
//	if (appr_lib > 4)
//		return -1;
//	fpos = bs->strings[string_idx];
//	pos = fpos;
//	do {
//		ai = I(pos);
//		aj = J(pos);
//		for (i = 0; i < 4; i++) {
//			bi = ai + deltai[i];
//			bj = aj + deltaj[i];
//			if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] == EMPTY) {
//				find_lib++;
//				if (lpos == -1)
//					lpos = POS(bi, bj);
//				else
//					if (lpos != POS(bi, bj))
//						return -1;
//				if (find_lib == appr_lib)
//					return lpos;
//			}
//		}
//		pos = bs->next_stone[pos];
//	} while (pos != fpos);
//
//	return lpos;
//}

/*******************************************************
* Function Name:	suicide
* Purpose:			Is a move at (i, j) suicide for color?
* Params :
*	@board_status(*)	bs		board status
*	@int				i		查询block position i
*	@int				j		查询block position j
*	@intersection		color	player's color
* Return:				int		0:false 1:true
* Limitation:
*******************************************************/
int suicide(board_status *bs, int i, int j, intersection color) {
	int k;
	//查询点的四周是否有气
	for (k = 0; k < 4; k++){
		//如果有气
		if (provides_liberty(bs, i + deltai[k], j + deltaj[k], i, j, color)){
			return 0;
		}
	}

	return 1;
}

/*******************************************************
* Function Name:	is_legal_move
* Purpose:			Is a move at pos legal?
* Params :
*	@board_status(*)	bs		board status
*	@intersection		color	player's color
*	@int				pos		查询block position index
* Return:				int		1:legal	0:not legal
* Limitation:
*******************************************************/
int is_legal_move(board_status *bs, intersection color, int pos) {
	int ai, aj, bi, bj, k;
	ai = I(pos);
	aj = J(pos);
	// 合理移动且不是自杀行为
	if (legal_move(bs, ai, aj, color)
		&& !suicide(bs, ai, aj, color)) {
		/* Further require the move not to be suicide for the
		 * opponent...
		 */
		//为什么需要判断对方下在(ai, aj)处是不是自杀？ cyy
		if (!suicide(bs, ai, aj, OTHER_COLOR(color))){
			return 1;
		}
		else {
			/* ...however, if the move captures at least one stone,
			 * consider it anyway.
			 */
			for (k = 0; k < 4; k++) {
				bi = ai + deltai[k];
				bj = aj + deltaj[k];
				if (ON_BOARD(bi, bj)
					&& bs->board[POS(bi, bj)] == OTHER_COLOR(color)) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/*******************************************************
* Function Name:	get_legal_moves2
* Purpose:			Get the list and the number of the
*					legal moves
* Params :
*	@board_status(*)	bs		board status
*	@intersection		color	player's color
* Return:				void	Update the bs->legal_moves
*								&& bs->legal_moves_num
* Limitation:		version2 cyy
*******************************************************/
//void get_legal_moves2(board_status *bs, intersection color) {
//	int num_moves = 0;
//	int i;
//	//遍历整张棋盘 着手方在棋盘上一点如果合法 应该记录在合法移动里面 并且记录合法移动位置数目 注：legal[][] == 1 是不合法
//	for (i = 0; i < board_array_size; i++) {
//		//这个legal数组没懂 cyy 
//		if (!bs->legal[color - 1][i]){
//			//记录合法移动的pos legal_moves[x] = i -> 第x个合法移动位置的pos是i  
//			bs->legal_moves[num_moves++] = i;
//		}
//	}
//	//更新当前棋盘状态下合法移动的数目
//	bs->legal_moves_num = num_moves;
//}

/*******************************************************
* Function Name:	get_legal_moves
* Purpose:			Get the list and the number of the
*					legal moves
* Params :
*	@board_status(*)	bs		board status
*	@intersection		color	player's color
* Return:				void	Update the bs->legal_moves
*								&& bs->legal_moves_num
* Limitation:
*******************************************************/
void get_legal_moves(board_status *bs, intersection color) {
	int ai, aj, bi, bj, k;
	int num_moves = 0;

	memset(bs->legal_moves, 0, sizeof(bs->legal_moves));
	for (ai = 0; ai < board_size; ai++){
		for (aj = 0; aj < board_size; aj++) {
			/* Consider moving at (ai, aj) if it is legal and not suicide. */
			if (legal_move(bs, ai, aj, color)
				&& !suicide(bs, ai, aj, color)) {
				/* Further require the move not to be suicide for the
				 * opponent...
				 */
				if (!suicide(bs, ai, aj, OTHER_COLOR(color))){
					bs->legal_moves[num_moves++] = POS(ai, aj);
				}
				else {
					/* ...however, if the move captures at least one stone,
					 * consider it anyway.
					 */
					for (k = 0; k < 4; k++) {
						bi = ai + deltai[k];
						bj = aj + deltaj[k];
						if (ON_BOARD(bi, bj)
							&& bs->board[POS(bi, bj)] == OTHER_COLOR(color)) {
							bs->legal_moves[num_moves++] = POS(ai, aj);
							break;
						}
					}
				}
			}
		}
	}
	bs->legal_moves_num = num_moves;
}

//void update_string_legal1(board_status *bs, int si) {
//	int lpos = only_lib(bs, si);
//	intersection color = bs->string_color[si];
//	if (lpos != -1) {
//		bs->update_pos[0]++;
//		bs->update_pos[bs->update_pos[0]] = lpos;
//	}
//}

/*******************************************************
* Function Name:	update_string_legal2
* Purpose:			Update the string si's legal
* Params :
*	@board_status(*)	bs		board status
*	@int				si  	string index
* Return:				void	Update the bs->legal
* Limitation:
*******************************************************/
//void update_string_legal2(board_status *bs, int si) {
//	int lpos = only_lib(bs, si);
//	if (lpos != -1) {
//		intersection color = bs->string_color[si];
//		//当前棋串只有一个气，位置为lpos
//		//对于两种颜色的棋子，如果该位置是合法的，那么legal对应位置设为0
//		//只剩一口气的位置对于自己和对方的合法性
//		bs->legal[color - 1][lpos] = 1 - is_legal_move(bs, color, lpos);
//		bs->legal[OTHER_COLOR(color) - 1][lpos] = 1 - is_legal_move(bs, OTHER_COLOR(color), lpos);
//	}
//}

/*******************************************************
* Function Name:	get_father
* Purpose:			Disjoint Set->get_father, get the
*					pos' father's position.
* Params :
*	@board_status(*)	bs		board status
*	@int				pos		child's position
* Return:				int		the father's position
* Limitation:
*******************************************************/
int get_father(board_status *bs, int pos) {
	if (bs->father[pos] == pos){
		return pos;
	}

	bs->father[pos] = get_father(bs, bs->father[pos]);
	return bs->father[pos];
}


/*******************************************************
* Function Name:	remove_string_from_strings
* Purpose:			remove a string from the board array
*					string的数量为num_of_strings，把当前要去掉的string
*					的所有信息设为第num_of_strings个string的信息，然后
*					num_of_strings减1
* Params :
*	@board_status(*)	bs		board status
*	@int				fa		要移除的string的father
* Return:				void    Update the status of
*								strings.
* Limitation:
*******************************************************/
static void remove_string_from_strings(board_status *bs, int fa) {
	//获取要移除的棋串的index
	int str_idx = bs->string_index[fa];

	if (str_idx != bs->num_of_strings) {
		//把要移除的string相关的信息用最后一组数据替代
		bs->strings[str_idx] = bs->strings[bs->num_of_strings];
		bs->string_color[str_idx] = bs->string_color[bs->num_of_strings];
		bs->string_stones[str_idx] = bs->string_stones[bs->num_of_strings];
		bs->approximate_liberty[str_idx] = bs->approximate_liberty[bs->num_of_strings];

		bs->string_index[bs->strings[str_idx]] = str_idx;
	}
	bs->num_of_strings--;
}

static void remove_string_from_strings_game(board_status *bs, int fa) {
	//获取要移除的棋串的index
	int str_idx = bs->string_index[fa], i;

	if (str_idx != bs->num_of_strings) {
		//把要移除的string相关的信息用最后一组数据替代
		bs->strings[str_idx] = bs->strings[bs->num_of_strings];
		bs->string_color[str_idx] = bs->string_color[bs->num_of_strings];
		bs->string_stones[str_idx] = bs->string_stones[bs->num_of_strings];
		bs->approximate_liberty[str_idx] = bs->approximate_liberty[bs->num_of_strings];

		//真气
		bs->approximate_liberty_real[str_idx] = bs->approximate_liberty_real[bs->num_of_strings];

		//更新被删除和最后一个棋串的provide_lib数组
		for (i = 0; i < MAX_BOARDSIZE; ++i)
		{
			if (bs->provide_lib[i][str_idx] == true)
				bs->provide_lib[i][str_idx] = false;
			if (bs->provide_lib[i][bs->num_of_strings] == true)
			{
				bs->provide_lib[i][str_idx] = true;
				bs->provide_lib[i][bs->num_of_strings] = false;
			}

		}
		bs->string_index[bs->strings[str_idx]] = str_idx;
	}
	bs->num_of_strings--;
}


/*******************************************************
* Function Name:	remove_string
* Purpose:			Remove a string from the board array.
*					There is no need to modify the
*					next_stone array since this only
*					matters where there are stones
*					present and the entire string is
*					removed.
* Params :
*	@board_status(*)	bs		board status
*	@int				i		要删除棋串中的某个stone的 i
*	@int				j		要删除棋串中的某个stone的 j
* Return:				int		The number of the removed
*								stones.
* Limitation: 只在simulate的时候使用
*******************************************************/
static int remove_string(board_status *bs, int i, int j) {
	//要删除棋串中的某个stone的 pos
	int pos = POS(i, j);

	int lpos;

	//棋串共有father
	int fa = get_father(bs, pos);
	//删除的棋子个数
	int removed = 0;

	int k, pos2, ai, aj, f2;

	//当前被删除棋串的颜色
	intersection color = bs->board[pos];

	//update_pos记录了等会儿需要用于更新legal[][]的pos
	//bs->update_pos[0] = 0;

	//以一点stone遍历全部棋串棋子
	pos = POS(i, j);
	do {
		// 遍历四周棋子
		for (k = 0; k < 4; k++) {
			ai = I(pos) + deltai[k];
			aj = J(pos) + deltaj[k];
			pos2 = POS(ai, aj);
			//如果四周有棋子是对方棋子
			if (ON_BOARD(ai, aj)
				&& bs->board[pos2] == OTHER_COLOR(color)) {
				//获取对方棋子所在棋串的father
				f2 = get_father(bs, pos2);

				//伪气
				bs->approximate_liberty[bs->string_index[f2]]++;
			}
		}
		pos = bs->next_stone[pos];
	} while (pos != POS(i, j));


	//删除棋串
	/* 1. board状态为empty
	 * 2. 更新father */
	pos = POS(i, j);
	do {
		bs->board[pos] = EMPTY;
		removed++;

		pos = bs->next_stone[pos];
		bs->father[pos] = -1;


	} while (pos != POS(i, j));


	// 删除strings集合里面的棋串
	remove_string_from_strings(bs, fa);

	// 没懂 cyy
	//之前有遍历要删除的棋串周围的点，如果遇到的是对方的点，会用update_string_legal1来判断对方的棋串是不是只有一口气了，把只有一口气的位置保存下来。
	//然后把这些位置存在legal[][]中 qxt
	//for (k = 1; k <= bs->update_pos[0]; k++) {
	//	lpos = bs->update_pos[k];
	//	// if (bs->string_stones[bs->string_index[fa]] == 8 && lpos == POS(2, 7))
	//	//     debug_log_int(5555);
	//	bs->legal[color - 1][lpos] = 1 - is_legal_move(bs, color, lpos);
	//	bs->legal[OTHER_COLOR(color) - 1][lpos] = 1 - is_legal_move(bs, OTHER_COLOR(color), lpos);
	//}

	return removed;
}
static int remove_string_game(board_status *bs, int i, int j) {
	//要删除棋串中的某个stone的 pos
	int pos = POS(i, j);

	int lpos;

	//棋串共有father
	int fa = get_father(bs, pos);
	//删除的棋子个数
	int removed = 0;

	int k, pos2, ai, aj, f2;

	//当前被删除棋串的颜色
	intersection color = bs->board[pos];

	//update_pos记录了等会儿需要用于更新legal[][]的pos
	//bs->update_pos[0] = 0;

	//以一点stone遍历全部棋串棋子
	pos = POS(i, j);
	do {
		// 遍历四周棋子
		for (k = 0; k < 4; k++) {
			ai = I(pos) + deltai[k];
			aj = J(pos) + deltaj[k];
			pos2 = POS(ai, aj);
			//如果四周有棋子是对方棋子
			if (ON_BOARD(ai, aj)
				&& bs->board[pos2] == OTHER_COLOR(color)) {
				//获取对方棋子所在棋串的father
				f2 = get_father(bs, pos2);
				//注释气
				//真气
				if (!bs->provide_lib[pos][bs->string_index[f2]])
				{
					bs->provide_lib[pos][bs->string_index[f2]] = true;
					bs->approximate_liberty_real[bs->string_index[f2]]++;
				}
				//伪气
				bs->approximate_liberty[bs->string_index[f2]]++;
			}
		}
		pos = bs->next_stone[pos];
	} while (pos != POS(i, j));


	//删除棋串
	/* 1. board状态为empty
	* 2. 更新father */
	pos = POS(i, j);
	do {
		bs->board[pos] = EMPTY;
		removed++;
		pos = bs->next_stone[pos];
		bs->father[pos] = -1;


	} while (pos != POS(i, j));

	// 删除strings集合里面的棋串
	remove_string_from_strings_game(bs, fa);

	return removed;
}
/*******************************************************
* Function Name:	same_string
* Purpose:			Disjoint Set->same_string, Do two
*					vertices belong to the same string.
*					It is required that both pos1 and
*					pos2 point to vertices with stones.
* Params :
*	@board_status(*)	bs		board status
*	@int				pos1	stone1 position
*	@int				pos2	stone2 position
* Return:				int		1:true	0:false
* Limitation:
*******************************************************/
static int same_string(board_status *bs, int pos1, int pos2) {
	return (get_father(bs, pos1) == get_father(bs, pos2));
}

//注释气
//只有在用真气的时候才用
static int union_lib(board_status *bs, int st1, int st2)
{
	//st1是小串
	int lib, f1, f2, bi, bj, k, t, pos, pos2;
	lib = bs->approximate_liberty_real[st1] + bs->approximate_liberty_real[st2];
	pos = f1 = bs->strings[st1];
	f2 = bs->strings[st2];
	do{
		for (k = 0; k < 4; ++k)
		{
			bi = I(pos) + deltai[k];
			bj = J(pos) + deltaj[k];
			pos2 = POS(bi, bj);
			if (ON_BOARD(bi, bj) && bs->board[pos2] == EMPTY)
			{
				if (bs->provide_lib[pos2][bs->string_index[f1]] && bs->provide_lib[pos2][bs->string_index[f2]])
					lib--;
			}
		}
		pos = bs->next_stone[pos];
	} while (pos != f1);

	return lib;
}

/*******************************************************
* Function Name:	union_string
* Purpose:			Disjoint Set->union_string, union two
*					strings. It is required that both pos1
*					and pos2 point to vertices with stones.
* Params :
*	@board_status(*)	bs		board status
*	@int				pos1	stone1 position in string1
*	@int				pos2	stone2 position in string2
* Return:				void	Update the status of strings
* Limitation:
*******************************************************/
static void union_string(board_status *bs, int pos1, int pos2) {
	int tmp = bs->next_stone[pos2], k;
	int f1 = get_father(bs, pos1);
	int f2 = get_father(bs, pos2);

	if (f1 != f2) {
		int i1 = bs->string_index[f1];
		int i2 = bs->string_index[f2];
		//保证把小的串并到大的串
		if (bs->string_stones[i1] < bs->string_stones[i2])
		{
			k = i1;
			i1 = i2;
			i2 = k;

			k = f1;
			f1 = f2;
			f2 = k;
		}
		//前面是i2小串，i1是大串
		bs->approximate_liberty[i1] += bs->approximate_liberty[i2];
		bs->father[f2] = f1;
		bs->string_stones[i1] += bs->string_stones[i2];
		remove_string_from_strings(bs, f2);
	}
	bs->next_stone[pos2] = bs->next_stone[pos1];
	bs->next_stone[pos1] = tmp;
}

static void union_string_game(board_status *bs, int pos1, int pos2) {
	int tmp = bs->next_stone[pos2], k, i;
	int f1 = get_father(bs, pos1);
	int f2 = get_father(bs, pos2);

	if (f1 != f2) {
		int i1 = bs->string_index[f1];
		int i2 = bs->string_index[f2];
		//保证把小的串并到大的串
		if (bs->string_stones[i1] < bs->string_stones[i2])
		{
			k = i1;
			i1 = i2;
			i2 = k;

			k = f1;
			f1 = f2;
			f2 = k;
		}
		//前面是i2小串，i1是大串
		//注释气
		bs->approximate_liberty_real[i1] = union_lib(bs, i2, i1);

		//更新provide_lib数组，st2会被删掉
		for (i = 0; i < MAX_BOARDSIZE; ++i)
		{
			if (bs->provide_lib[i][i2] == true)
			{
				bs->provide_lib[i][i2] = false;
				bs->provide_lib[i][i1] = true;
			}
		}
		bs->approximate_liberty[i1] += bs->approximate_liberty[i2];
		bs->father[f2] = f1;
		bs->string_stones[i1] += bs->string_stones[i2];

		remove_string_from_strings_game(bs, f2);
	}
	bs->next_stone[pos2] = bs->next_stone[pos1];
	bs->next_stone[pos1] = tmp;
}



/*******************************************************
* Function Name:	play_move
* Purpose:			Play at (i, j) for color. 确认下子
* Params :
*	@board_status(*)	bs		board status
*	@int				i		position i
*	@int				j		position j
*	@intersection		color	player's color
* Return:				void	Update the board array,
*								the next_stone array, and
*								the ko point.
* Limitation: 只在simulate的时候使用 不需要对真气进行维护
*******************************************************/
void play_move(board_status *bs, int i, int j, intersection color) {
	// 下子pos
	int pos = POS(i, j);
	// 被吃掉的棋子的数目
	int captured_stones = 0;
	int k;
	int ko_pos = POS(bs->ko_i, bs->ko_j);

	//这个操作必须在前面，因为无论是否pass是否自杀等提前返回的情况，这些信息都必须更新
	/* Reset the ko point. */
	bs->ko_i = -1;
	bs->ko_j = -1;

	/* Reset the last_move_pos */
	bs->last_move_pos = pos;

	/* Nothing more happens if the move was a pass. */
	if (PASS_MOVE(i, j)){
		return;
	}


	/* If the move is a suicide we only need to remove the adjacent
	 * friendly stones.
	 */
	if (suicide(bs, i, j, color)) {
		for (k = 0; k < 4; k++) {
			int ai = i + deltai[k];
			int aj = j + deltaj[k];
			if (ON_BOARD(ai, aj)
				&& bs->board[POS(ai, aj)] == color){
				remove_string(bs, ai, aj);
			}
		}
		return;
	}

	/* Not suicide. Remove captured opponent strings. */
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (ON_BOARD(ai, aj)
			&& bs->board[POS(ai, aj)] == OTHER_COLOR(color)
			&& !has_additional_liberty(bs, ai, aj, i, j)) {
			captured_stones += remove_string(bs, ai, aj);
		}
	}

	/* Update board status */

	/* Update block[pos] status */
	bs->board[pos] = color;

	/* Put down the new stone. Initially build a single stone string by
	 * setting next_stone[pos] pointing to itself.更新棋盘
	 */

	/* Create a new string */
	bs->next_stone[pos] = pos;
	bs->father[pos] = pos;
	bs->num_of_strings++;
	bs->string_index[pos] = bs->num_of_strings;

	// 落子位置连起来的string
	/* Create a new string */
	bs->strings[bs->num_of_strings] = pos;
	bs->string_stones[bs->num_of_strings] = 1;
	bs->approximate_liberty[bs->num_of_strings] = 0;
	bs->string_color[bs->num_of_strings] = color;

	// 落子以后update周围棋串的气和自己的气
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);
		if (ON_BOARD(ai, aj)) {
			// 影响了别的棋串的气 堵掉别人一口气 
			// 有问题吧 如果是和自己同色用减掉一口气的做法不一定对
			if (is_stone(bs, pos2)){
				bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]]--;
			}

			//自己作为一个棋串可以获得的气口
			if (bs->board[pos2] == EMPTY){
				bs->approximate_liberty[bs->num_of_strings]++;
			}

		}
	}

	/* If we have friendly neighbor strings we need to link the strings
	 * together.
	 */
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);

		/* Make sure that the stones are not already linked together. This
		 * may happen if the same string neighbors the new stone in more
		 * than one direction.
		 */
		if (ON_BOARD(ai, aj) && bs->board[pos2] == color
			&& !same_string(bs, pos, pos2)) {
			union_string(bs, pos, pos2);
		}
	}

	/* If we have captured exactly one stone and the new string is a
	 * single stone it may have been a ko capture.
	 */
	if (captured_stones == 1 && bs->next_stone[pos] == pos) {
		int ai, aj;
		/* Check whether the new string has exactly one liberty. If so it
		 * would be an illegal ko capture to play there immediately. We
		 * know that there must be a liberty immediately adjacent to the
		 * new stone since we captured one stone.
		 */
		for (k = 0; k < 4; k++) {
			ai = i + deltai[k];
			aj = j + deltaj[k];
			if (ON_BOARD(ai, aj) && bs->board[POS(ai, aj)] == EMPTY){
				break;
			}
		}

		if (!has_additional_liberty(bs, i, j, ai, aj)) {
			bs->ko_i = ai;
			bs->ko_j = aj;
			bs->ko_pos = POS(bs->ko_i, bs->ko_j);
			//bs->legal[OTHER_COLOR(color) - 1][POS(ai, aj)] = 1;
		}
	}
}

void play_move_game(board_status *bs, int i, int j, intersection color) {
	// 下子pos
	int pos = POS(i, j);
	// 被吃掉的棋子的数目
	int captured_stones = 0;
	int k;
	int ko_pos = POS(bs->ko_i, bs->ko_j);

	//这个操作必须在前面，因为无论是否pass是否自杀等提前返回的情况，这些信息都必须更新
	/* Reset the ko point. */
	bs->ko_i = -1;
	bs->ko_j = -1;

	/* Reset the last_move_pos */
	bs->last_move_pos = pos;

	/* Nothing more happens if the move was a pass. */
	if (PASS_MOVE(i, j)){
		return;
	}


	/* If the move is a suicide we only need to remove the adjacent
	* friendly stones.
	*/
	if (suicide(bs, i, j, color)) {
		for (k = 0; k < 4; k++) {
			int ai = i + deltai[k];
			int aj = j + deltaj[k];
			if (ON_BOARD(ai, aj)
				&& bs->board[POS(ai, aj)] == color){
				remove_string_game(bs, ai, aj);
			}
		}
		return;
	}

	/* Not suicide. Remove captured opponent strings. */
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (ON_BOARD(ai, aj)
			&& bs->board[POS(ai, aj)] == OTHER_COLOR(color)
			&& !has_additional_liberty_game(bs, ai, aj, i, j)) {
			captured_stones += remove_string_game(bs, ai, aj);
		}
	}

	/* Update board status */

	/* Update block[pos] status */
	bs->board[pos] = color;

	/* Put down the new stone. Initially build a single stone string by
	* setting next_stone[pos] pointing to itself.更新棋盘
	*/

	/* Create a new string */
	bs->next_stone[pos] = pos;
	bs->father[pos] = pos;
	bs->num_of_strings++;
	bs->string_index[pos] = bs->num_of_strings;

	// 落子位置连起来的string
	/* Create a new string */
	bs->strings[bs->num_of_strings] = pos;
	bs->string_stones[bs->num_of_strings] = 1;
	bs->approximate_liberty[bs->num_of_strings] = 0;
	bs->approximate_liberty_real[bs->num_of_strings] = 0;
	bs->string_color[bs->num_of_strings] = color;
	//注释气
	memset(bs->provide_lib[pos], false, MAX_BOARDSIZE);

	// 落子以后update周围棋串的气和自己的气
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);
		if (ON_BOARD(ai, aj)) {
			// 影响了别的棋串的气 堵掉别人一口气 
			// 有问题吧 如果是和自己同色用减掉一口气的做法不一定对
			if (is_stone(bs, pos2)){
				bs->approximate_liberty[bs->string_index[get_father(bs, pos2)]]--;
				bs->approximate_liberty_real[bs->string_index[get_father(bs, pos2)]]--;
			}

			//自己作为一个棋串可以获得的气口
			if (bs->board[pos2] == EMPTY){
				//注释气
				bs->provide_lib[pos2][bs->num_of_strings] = true;
				bs->approximate_liberty[bs->num_of_strings]++;
				bs->approximate_liberty_real[bs->num_of_strings]++;
			}

		}
	}

	/* If we have friendly neighbor strings we need to link the strings
	* together.
	*/
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);

		/* Make sure that the stones are not already linked together. This
		* may happen if the same string neighbors the new stone in more
		* than one direction.
		*/
		if (ON_BOARD(ai, aj) && bs->board[pos2] == color
			&& !same_string(bs, pos, pos2)) {
			union_string_game(bs, pos, pos2);
		}
	}

	/* If we have captured exactly one stone and the new string is a
	* single stone it may have been a ko capture.
	*/
	if (captured_stones == 1 && bs->next_stone[pos] == pos) {
		int ai, aj;
		/* Check whether the new string has exactly one liberty. If so it
		* would be an illegal ko capture to play there immediately. We
		* know that there must be a liberty immediately adjacent to the
		* new stone since we captured one stone.
		*/
		for (k = 0; k < 4; k++) {
			ai = i + deltai[k];
			aj = j + deltaj[k];
			if (ON_BOARD(ai, aj) && bs->board[POS(ai, aj)] == EMPTY){
				break;
			}
		}

		if (!has_additional_liberty_game(bs, i, j, ai, aj)) {
			bs->ko_i = ai;
			bs->ko_j = aj;
			bs->ko_pos = POS(bs->ko_i, bs->ko_j);
		}
	}
}
/*******************************************************
* Function Name:	set_final_status_string
* Purpose:			Set a final status value for an entire string.
* Params :
*	@board_status(*)	bs		board status
*	@int				pos		不懂待补充
*	@int				status	不懂待补充
* Return:				void	不懂待补充
* Limitation:
*******************************************************/
static void set_final_status_string(board_status *bs, int pos, int status) {
	int pos2 = pos;
	do {
		bs->final_status[pos2] = status;
		pos2 = bs->next_stone[pos2];
	} while (pos2 != pos);
}

/* Compute final status. This function is only valid to call in a
 * position where generate_move() would return pass for at least one
 * color.
 *
 * Due to the nature of the move generation algorithm, the final
 * status of stones can be determined by a very simple algorithm:
 *
 * 1. Stones with two or more liberties are alive with territory.
 * 2. Stones in atari are dead.
 *
 * Moreover alive stones are unconditionally alive even if the
 * opponent is allowed an arbitrary number of consecutive moves.
 * Similarly dead stones cannot be brought alive even by an arbitrary
 * number of consecutive moves.
 *
 * Seki is not an option. The move generation algorithm would never
 * leave a seki on the board.
 *
 * Comment: This algorithm doesn't work properly if the game ends with
 *          an unfilled ko. If three passes are required for game end,
 *          that will not happen.
 */

/*******************************************************
* Function Name:	compute_final_status
* Purpose:			Compute final status.
* Params :
*	@board_status(*)	bs		board status
* Return:				void	Update bs->final_status[]
* Limitation:This function is only valid to call in a
* position where generate_move() would return pass for
* at least one color.
*******************************************************/
void compute_final_status(board_status *bs) {
	int i, j;
	int pos;
	int k;

	for (pos = 0; pos < board_array_size; pos++) {
		bs->final_status[pos] = UNKNOWN;
	}

	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			if (bs->board[POS(i, j)] == EMPTY) {
				for (k = 0; k < 4; k++) {
					int ai = i + deltai[k];
					int aj = j + deltaj[k];
					if (!ON_BOARD(ai, aj)){
						continue;
					}

					/* When the game is finished, we know for sure that (ai, aj)
					* contains a stone. The move generation algorithm would
					* never leave two adjacent empty vertices. Check the number
					* of liberties to decide its status, unless it's known
					* already.
					*
					* If we should be called in a non-final position, just make
					* sure we don't call set_final_status_string() on an empty
					* vertex.
					*/
					pos = POS(ai, aj);
					if (bs->final_status[pos] == UNKNOWN) {
						//printf("\nbs->board[POS(ai, aj)] == %d \n", bs->board[POS(ai, aj)]);
						if (bs->board[POS(ai, aj)] != EMPTY) {
							if (has_additional_liberty(bs, ai, aj, i, j)){
								set_final_status_string(bs, pos, ALIVE);
							}
							else{
								set_final_status_string(bs, pos, DEAD);
							}
						}
					}
					/* Set the final status of the (i, j) vertex to either black
					* or white territory.
					*/
					if (bs->final_status[POS(i, j)] == UNKNOWN) {
						if ((bs->final_status[pos] == ALIVE) ^ (bs->board[POS(ai, aj)] == WHITE))
							bs->final_status[POS(i, j)] = BLACK_TERRITORY;
						else
							bs->final_status[POS(i, j)] = WHITE_TERRITORY;
					}
				}
			}
		}
	}

}


void compute_final_status_game(board_status *bs) {
	int i, j;
	int pos;
	int k;

	for (pos = 0; pos < board_array_size; pos++) {
		bs->final_status[pos] = UNKNOWN;
	}

	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			if (bs->board[POS(i, j)] == EMPTY) {
				for (k = 0; k < 4; k++) {
					int ai = i + deltai[k];
					int aj = j + deltaj[k];
					if (!ON_BOARD(ai, aj)){
						continue;
					}

					/* When the game is finished, we know for sure that (ai, aj)
					* contains a stone. The move generation algorithm would
					* never leave two adjacent empty vertices. Check the number
					* of liberties to decide its status, unless it's known
					* already.
					*
					* If we should be called in a non-final position, just make
					* sure we don't call set_final_status_string() on an empty
					* vertex.
					*/
					pos = POS(ai, aj);
					if (bs->final_status[pos] == UNKNOWN) {
						//printf("\nbs->board[POS(ai, aj)] == %d \n", bs->board[POS(ai, aj)]);
						if (bs->board[POS(ai, aj)] != EMPTY) {
							if (has_additional_liberty_game(bs, ai, aj, i, j)){
								set_final_status_string(bs, pos, ALIVE);
							}
							else{
								set_final_status_string(bs, pos, DEAD);
							}
						}
					}
					/* Set the final status of the (i, j) vertex to either black
					* or white territory.
					*/
					if (bs->final_status[POS(i, j)] == UNKNOWN) {
						if ((bs->final_status[pos] == ALIVE) ^ (bs->board[POS(ai, aj)] == WHITE))
							bs->final_status[POS(i, j)] = BLACK_TERRITORY;
						else
							bs->final_status[POS(i, j)] = WHITE_TERRITORY;
					}
				}
			}
		}
	}

}

/*******************************************************
* Function Name:	get_final_status
* Purpose:			Get final status at (i , j)
* Params :
*	@board_status(*)	bs		board status
*	@int				i		position i
*	@int				j		position j
* Return:				int		the status of (i, j)
* Limitation:
*******************************************************/
int get_final_status(board_status *bs, int i, int j) {
	return bs->final_status[POS(i, j)];
}

int get_final_status2(board_status *bs, int pos) {
	return bs->final_status[pos];
}

/*******************************************************
* Function Name:	set_final_status
* Purpose:			set final status at (i , j) to status
* Params :
*	@board_status(*)	bs		board status
*	@int				i		position i
*	@int				j		position j
*	@int				status	position status
* Return:				void	Update bs->final_status[]
* Limitation:
*******************************************************/
void set_final_status(board_status *bs, int i, int j, int status) {
	bs->final_status[POS(i, j)] = status;
}


/*******************************************************
* Function Name:	get_point_score
* Purpose:			accluate sum
* Params :
* Return:
* Limitation:  循环加法优化 获得单点score值 已废弃 cyy
*******************************************************/
int get_point_score(board_status *bs, int i, int j){
	int status;
	status = get_final_status(bs, i, j);
	if (status == BLACK_TERRITORY)
		return -1;
	else if (status == WHITE_TERRITORY)
		return 1;
	else if ((status == ALIVE) ^ (bs->board[POS(i, j)] == WHITE))
		return -1;
	else
		return 1;

}

/*******************************************************
* Function Name:	quick_score_sum
* Purpose:			accluate sum
* Params :
* Return:
* Limitation:  循环加法优化 已废弃 没用 cyy
*******************************************************/
void quick_score_sum(board_status *bs, int length, double *sum, int i) {
	//for (j = 0; j < board_size; j++) {
	//	status = get_final_status(bs, i, j);
	//	if (status == BLACK_TERRITORY)
	//		score--;
	//	else if (status == WHITE_TERRITORY)
	//		score++;
	//	else if ((status == ALIVE) ^ (bs->board[POS(i, j)] == WHITE))
	//		score--;
	//	else
	//		score++;
	//}

	double value = 0.0;
	int j = 0;
	int num = length - (length % 4);

	double value1 = 1;
	double value2 = 1;

	for (; j < num; j += 4)
	{
		//value1 = array[i] + array[i + 1];
		//value2 = array[i + 2] + array[i + 3];
		//value = value + value1 + value2;
		value1 = get_point_score(bs, i, j) + get_point_score(bs, i, j + 1);
		value2 = get_point_score(bs, i, j + 2) + get_point_score(bs, i, j + 3);
		value = value + value1 + value2;
	}

	for (; j < length; ++j) {
		value = (value + get_point_score(bs, i, j));
	}

	*sum = *sum + value;
}


/*******************************************************
* Function Name:	get_score
* Purpose:			Get score at this board stauts
* Params :
*	@board_status(*)	bs		board status
* Return:				double	Score
* Limitation:
*******************************************************/
double get_score(board_status *bs) {
	//print_line("get_score");
	double score = komi;
	int i, j, status;

	compute_final_status(bs);
	for (i = 0; i < board_size; i++){
		//quick_score_sum(bs, board_size, &score, i);
		for (j = 0; j < board_size; j++) {
			status = get_final_status(bs, i, j);
			if (status == BLACK_TERRITORY)
				score--;
			else if (status == WHITE_TERRITORY)
				score++;
			else if ((status == ALIVE) ^ (bs->board[POS(i, j)] == WHITE))
				score--;
			else
				score++;
		}
	}

	//printf("score = %f", score);
	return score;
}

//想让它更快，只计算输赢，不计算分数
//-1是黑胜，1是白胜
int get_score_uct(board_status *bs) {
	//print_line("get_score");
	double score = (board_array_size - komi) / 2;
	int pos, status;

	compute_final_status(bs);
	for (pos = 0; pos < board_array_size; ++pos)
	{
		status = get_final_status2(bs, pos);
		if (status == WHITE_TERRITORY || ((status == ALIVE) ^ (bs->board[pos] == BLACK)))
		{
			score--;
		}
		if (score < 0)
			return 1;
	}
	return -1;
}

//想让它更快，计算白子总共有多少颗
int get_score_uct_white(board_status *bs) {
	int score = 0;// komi;
	int pos, status;

	compute_final_status(bs);
	for (pos = 0; pos < board_array_size; ++pos)
	{
		status = get_final_status2(bs, pos);
		if (status == WHITE_TERRITORY || ((status == ALIVE) ^ (bs->board[pos] == BLACK)))
		{
			score++;
		}
	}
	return score;
}


/*******************************************************
* Function Name:	valid_fixed_handicap
* Purpose:			Valid number of stones for fixed placement
*					handicaps.These are compatible with the
*					GTP fixed handicap placement rules.
* Params :
*	@int	handicap	不懂待补充
* Return:	int			不懂待补充
* Limitation:
*******************************************************/
int valid_fixed_handicap(int handicap) {
	if (handicap < 2 || handicap > 9)
		return 0;
	if (board_size % 2 == 0 && handicap > 4)
		return 0;
	if (board_size == 7 && handicap > 4)
		return 0;
	if (board_size < 7 && handicap > 0)
		return 0;

	return 1;
}

/* Put fixed placement handicap stones on the board. The placement is
 * compatible with the GTP fixed handicap placement rules.
 */

/*******************************************************
* Function Name:	place_fixed_handicap
* Purpose:			Put fixed placement handicap stones
*					on the board. The placement is compatible
*					with the GTP fixed handicap placement rules.
* Params :
*	@board_status(*)	bs		board status
*	@int				handicap	不懂待补充
* Return:				void		不懂待补充
* Limitation:
*******************************************************/
void place_fixed_handicap(board_status *bs, int handicap) {
	int low = board_size >= 13 ? 3 : 2;
	int mid = board_size / 2;
	int high = board_size - 1 - low;

	if (handicap >= 2) {
		play_move_game(bs, high, low, BLACK);     /* bottom left corner */
		play_move_game(bs, low, high, BLACK);     /* top right corner */
	}

	if (handicap >= 3)
		play_move_game(bs, low, low, BLACK);        /* top left corner */

	if (handicap >= 4)
		play_move_game(bs, high, high, BLACK);    /* bottom right corner */

	if (handicap >= 5 && handicap % 2 == 1)
		play_move_game(bs, mid, mid, BLACK);        /* tengen */

	if (handicap >= 6) {
		play_move_game(bs, mid, low, BLACK);        /* left edge */
		play_move_game(bs, mid, high, BLACK);     /* right edge */
	}

	if (handicap >= 8) {
		play_move_game(bs, low, mid, BLACK);        /* top edge */
		play_move_game(bs, high, mid, BLACK);     /* bottom edge */
	}
}


