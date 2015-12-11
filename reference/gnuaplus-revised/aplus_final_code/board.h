/*********************************************************************************
* Copyright(C),2014-2015,SJTU
* Program Assignment:	board
* FileName:	board.h
* Source code in��	gnu_base_code/board.h
* Author:	Cyy/ljj/Qxt/Zzh
* Version:	1.0
* Date:	2014/12/28
* Description:	���̶����弰ͷ�ļ�
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
/* �������� board[i] */
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
	/* board[i] ��������pos == i��������״̬ */
	/* board[i] -> EMPTY or WHITE or BLACK */
	/* default value:
	 *	board[i] = EMPTY */
	intersection board[MAX_BOARDSIZE];

	/* Position of the last move */
	/* default value��
	 *	last_move_pos = -1 */
	int last_move_pos;

	/* Point which would be an illegal ko recapture. */
	/* default value��
	 *	ko_i	=	-1
	 *	ko_i	=	-1
	 *	ko_pos	=	POS(-1, -1) */
	int ko_i, ko_j, ko_pos;

	/* Stones are linked together in a circular list for each string. */
	int next_stone[MAX_BOARDSIZE];

	/* ���鼯 ������ */
	/* father[i] -> stonei's father is father[i]
	 * i -> the index of block
	 * father[i] == i -> single stone
	 * father[i] == -1 -> EMPTY
	 *
	 * default value ��
	 *	father[i] = -1
	 */
	int father[MAX_BOARDSIZE];

	/* ��¼�����ϵ��崮 */
	/* strings[i] = x:
	 *	i-> string_index(���崮��Ŀ����)
	 *	x->	���崮��father */
	/* all strings in the board */
	int strings[MAX_BOARDSIZE];

	/* ��¼�����ϵ��崮����ɫ */
	/* string_color[i] = x:
	*	i-> string_index(���崮��Ŀ����)
	*	x->	���崮��color */
	/* all strings in the board */
	intersection string_color[MAX_BOARDSIZE];

	/* ��¼�����ϵ��崮�����Ӹ��� */
	/* string_stones[i] = x:
	*	i-> string_index(���崮��Ŀ����)
	*	x->	���崮�����Ӹ��� */
	/* all strings in the board */
	int string_stones[MAX_BOARDSIZE];

	/* ��¼�����ϵ��崮������ */
	/* approximate_liberty[i] = x:
	*	i-> string_index(���崮��Ŀ����)
	*	x->	���崮������ */
	/* all strings in the board */
	int approximate_liberty[MAX_BOARDSIZE];


	/* ��¼�����ϵ��崮���������� */
	/* approximate_liberty_real[i] = x:
	*	i-> string_index(���崮��Ŀ����)
	*	x->	���崮���������� */
	/* all strings in the board */
	int approximate_liberty_real[MAX_BOARDSIZE];

	/* the number of strings */
	/* default value :
	 *	num_of_strings = 0 */
	int num_of_strings;

	//Ŀǰû�г�ʼ��ֵ cyy
	/* ��¼�������崮��index �����崮��father pos����¼ */
	/* string_index[i] = x�� �崮fatherΪi��indexΪx */
	/* strings[string_index[get_father(pos)]] = get_father(pos) */
	int string_index[MAX_BOARDSIZE];

	/* ������¼��ǰ����״̬�µĺϷ��ƶ���pos */
	/* legal_moves[i] = x: ��i���Ϸ��ƶ�λ�õ�pos��x */
	/* defalut value:
	 *	legal_moves[i] = -1 */
	int legal_moves[MAX_BOARDSIZE];

	/* ������¼��ǰ����״̬�ºϷ��ƶ���λ����Ŀ */
	/* defalut value:
	*	legal_moves_num = 0 */
	int legal_moves_num;

	/* Storage for final status computations. */
	int final_status[MAX_BOARDSIZE];
	int update_pos[MAX_BOARDSIZE];

	/* record the history */
	int path[MAX_BOARDSIZE];
	int path_top;//-1,0,1,2...


	/*�ж�һ����λ�Ƿ��һ���崮�ṩ��
	provide_lib[��λpos][�崮index]*/
	//ע����
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
/* �ж� (i, j) ���λ���ǲ��������̷�Χ�� */
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
