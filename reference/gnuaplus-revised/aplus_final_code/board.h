/*********************************************************************************
* Copyright(C),2014-2015,SJTU
* Program Assignment:	board
* FileName:	board.h
* Source code in：	gnu_base_code/board.h
* Author:	Cyy/ljj/Qxt/Zzh
* Version:	1.0
* Date:	2014/12/28
* Description:	棋盘对象定义及头文件
* Others:
* Function List:
* History:
*	1.	Date:	2014/12/28
*		Author:
*		Modification:	base code
*	2.	Date:
*		Author:
*		Modification:
**********************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>
/* board size */
#define MIN_BOARD 13
#define MAX_BOARD 13
#define MAX_BOARDSIZE 169

/* board block status */
/* 用于描述 board[i] */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

/* board block status */
#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define UNKNOWN 5

/* color */
typedef unsigned char intersection;

typedef unsigned int bitboard;

extern float komi;
extern int board_size;
extern int board_array_size;

typedef struct sbs {
	/* board status */
	/* board[i] ：棋盘上pos == i处的棋盘状态 */
	/* board[i] -> EMPTY or WHITE or BLACK */
	/* default value:
	 *	board[i] = EMPTY */
	intersection board[MAX_BOARDSIZE];

	/* Position of the last move */
	/* default value：
	 *	last_move_pos = -1 */
	int last_move_pos;

	/* Point which would be an illegal ko recapture. */
	/* default value：
	 *	ko_i	=	-1
	 *	ko_i	=	-1
	 *	ko_pos	=	POS(-1, -1) */
	int ko_i, ko_j, ko_pos;

	/* Stones are linked together in a circular list for each string. */
	int next_stone[MAX_BOARDSIZE];

	/* 并查集 加速用 */
	/* father[i] -> stonei's father is father[i]
	 * i -> the index of block
	 * father[i] == i -> single stone
	 * father[i] == -1 -> EMPTY
	 *
	 * default value ：
	 *	father[i] = -1
	 */
	int father[MAX_BOARDSIZE];

	/* 记录棋盘上的棋串 */
	/* strings[i] = x:
	 *	i-> string_index(由棋串数目决定)
	 *	x->	该棋串的father */
	/* all strings in the board */
	int strings[MAX_BOARDSIZE];

	/* 记录棋盘上的棋串的颜色 */
	/* string_color[i] = x:
	*	i-> string_index(由棋串数目决定)
	*	x->	该棋串的color */
	/* all strings in the board */
	intersection string_color[MAX_BOARDSIZE];

	/* 记录棋盘上的棋串的棋子个数 */
	/* string_stones[i] = x:
	*	i-> string_index(由棋串数目决定)
	*	x->	该棋串的棋子个数 */
	/* all strings in the board */
	int string_stones[MAX_BOARDSIZE];

	/* 记录棋盘上的棋串的气口 */
	/* approximate_liberty[i] = x:
	*	i-> string_index(由棋串数目决定)
	*	x->	该棋串的气口 */
	/* all strings in the board */
	int approximate_liberty[MAX_BOARDSIZE];


	/* 记录棋盘上的棋串的真气气口 */
	/* approximate_liberty_real[i] = x:
	*	i-> string_index(由棋串数目决定)
	*	x->	该棋串的真气气口 */
	/* all strings in the board */
	int approximate_liberty_real[MAX_BOARDSIZE];

	/* the number of strings */
	/* default value :
	 *	num_of_strings = 0 */
	int num_of_strings;

	//目前没有初始化值 cyy
	/* 记录棋盘上棋串的index 按照棋串的father pos来记录 */
	/* string_index[i] = x： 棋串father为i的index为x */
	/* strings[string_index[get_father(pos)]] = get_father(pos) */
	int string_index[MAX_BOARDSIZE];

	/* 用来记录当前棋盘状态下的合法移动的pos */
	/* legal_moves[i] = x: 第i个合法移动位置的pos是x */
	/* defalut value:
	 *	legal_moves[i] = -1 */
	int legal_moves[MAX_BOARDSIZE];

	/* 用来记录当前棋盘状态下合法移动的位置数目 */
	/* defalut value:
	*	legal_moves_num = 0 */
	int legal_moves_num;

	/* Storage for final status computations. */
	int final_status[MAX_BOARDSIZE];
	int update_pos[MAX_BOARDSIZE];

	/* record the history */
	int path[MAX_BOARDSIZE];
	int path_top;//-1,0,1,2...


	/*判断一个空位是否给一个棋串提供气
	provide_lib[空位pos][棋串index]*/
	//注释气
	bool provide_lib[MAX_BOARDSIZE][MAX_BOARDSIZE];
} board_status;


extern board_status main_board;

/* Offsets for the four directly adjacent neighbors. Used for looping. */
extern int deltai[4];
extern int deltaj[4];

/* calculate the index of (i, j) */
#define POS(i, j) ((i) * board_size + (j))
/* calculate the i and j of pos */
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

/* calculate the color of the stone */
#define OTHER_COLOR(color) (WHITE + BLACK - (color))

/* pass or not */
#define PASS_MOVE(i, j) ((i) == -1 && (j) == -1)
/* 判断 (i, j) 这个位置是不是在棋盘范围内 */
#define ON_BOARD(i, j) \
    ((i) >= 0 && (i) < board_size && (j) >= 0 && (j) < board_size)


void clear_board(board_status *bs);
int board_empty(board_status *bs);
int is_stone(board_status *bs, int pos);
int get_string(board_status *bs, int i, int j, int *stonei, int *stonej);
int legal_move(board_status *bs, int i, int j, intersection color);
int has_additional_liberty(board_status *bs, int i, int j, int libi, int libj);
int has_additional_liberty_game(board_status *bs, int i, int j, int libi, int libj);
int provides_liberty(board_status *bs, int ai, int aj, int i, int j, intersection color);
int provides_liberty_sim(board_status *bs, int ai, int aj, int i, int j, intersection color);
int suicide(board_status *bs, int i, int j, intersection color);
int is_legal_move(board_status *bs, intersection color, int pos);
void get_legal_moves(board_status *bs, intersection color);
int get_father(board_status *bs, int pos);
static void remove_string_from_strings(board_status *bs, int fa);
static void remove_string_from_strings_game(board_status *bs, int fa);
static int remove_string(board_status *bs, int i, int j);
static int remove_string_game(board_status *bs, int i, int j);
static int same_string(board_status *bs, int pos1, int pos2);
static void union_string(board_status *bs, int pos1, int pos2);
static void union_string_game(board_status *bs, int pos1, int pos2);
void play_move(board_status *bs, int i, int j, intersection color);
void play_move_game(board_status *bs, int i, int j, intersection color);
void compute_final_status(board_status *bs);
void compute_final_status_game(board_status *bs);
int get_final_status(board_status *bs, int i, int j);
int get_final_status2(board_status *bs, int pos);
void set_final_status(board_status *bs, int i, int j, int status);
double get_score(board_status *bs);
int get_score_uct(board_status *bs);
int get_score_uct_white(board_status *bs);
int valid_fixed_handicap(int handicap);
void place_fixed_handicap(board_status *bs, int handicap);

#endif
