#ifndef _APLUS_H_
#define _APLUS_H_

#include "board.h"

#define APLUS_NAME "Aplus_final"
#define APLUS_VERSION "12.0"

#define PATH_CHESS_MANUAL "F:\\Course\\2015-2016_Autumn\\Artificial_Intelligence\\Go\\resource\\gnuaplus-revised\\chess_manual\\kogo4.SGF"

//分段
#define STEPS_START_END 3			//开局要走多少步固定位置
#define STEPS_MANUAL_END 10			//到多少步为止按照棋谱来走
#define STEPS_EAT_SAVE_END 140		//吃子和救子的步数限制
#define STEPS_CONNECT_CUT_END 40	//连接和切断的步数限制
#define STEPS_MIDDLE_START 50		//中场开始步数
#define STEPS_MIDDLE_END 100		//中场结束步数
#define MAX_LEGGAL_RANDOM 20		//靠random获得legalmove的最大数 用于优化
#define STEPS_BOUNDARY_TWO 10		//多少步以内忽略棋牌最外圈两层点
#define STEPS_BOUNDARY_ONE 20		//多少步以内忽略棋牌最外圈一层点

//在排序之前的，result_to_uct[i]表示pos=i的位置的权重
//不合法：-1
//最普通的点：0
int result_to_uct[MAX_BOARDSIZE];
int result_to_uct_pos[MAX_BOARDSIZE];

void init_aplus(void);
int generate_random_move(board_status *bs, intersection color);
void place_free_handicap(board_status *bs, int handicap);
int generate_move(board_status *bs, intersection color);
int sim_for_string(board_status *bs, int pos, intersection color);
int divide_result(int low, int high);
void quick_sort_for_result(int low, int high);

#endif
