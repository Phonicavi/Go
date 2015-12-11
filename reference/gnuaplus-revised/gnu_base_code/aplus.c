#include <stdio.h>
#include <stdlib.h> //解决rand（）警告
#include <string.h>
#include <time.h>
#include "aplus.h"
#include "uct.h"
#include "uct_pure.h"
#include "board.h"
#include "tricks_chess_manual.h"
#include "tricks_chess.h"
#include "log.h"
#include "bit_process.h"

int isLine(int i, int j)
{
	if (i == 0 && (j) >= 1 && (j) < (board_size - 1))
		return 1; //上
	else if (j == (board_size - 1) && (i) >= 1 && (i) < (board_size - 1))
		return 2; //右
	else if (i == (board_size - 1) && (j) >= 1 && (j) < (board_size - 1))
		return 3; //下
	else if (j == 0 && (i) >= 1 && (i) < (board_size - 1))
		return 4; //左
	else
		return 0;

}


void init_aplus(void){
	clear_board(&main_board);
	init_trick_chess_manual();
	init_zobrist();
}

/*吃掉还剩一口气的子（实现方法先用耗时的，因为维护气的方法还有问题）*/
static int eat_one_lib(board_status *bs, intersection color)
{

	int ai, aj;
	int k;
	int pos = -1;
	for (ai = 0; ai < board_size; ai++) {
		for (aj = 0; aj < board_size; aj++) {
			//仅有一口气的棋串，必须吃掉
			if (legal_move(bs, ai, aj, color)
				&& !suicide(bs, ai, aj, color)) {
				for (k = 0; k < 4; k++) {
					int bi = ai + deltai[k];
					int bj = aj + deltaj[k];
					if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] == OTHER_COLOR(color))
						if (!has_additional_liberty(bs, bi, bj, ai, aj)) {
						pos = POS(ai, aj);
						return pos;
						}

				}
			}
		}
	}
	return pos;
}


/*吃掉还剩一口气的子 条件：1.在腹地中 2.对方总棋串数目大于等于3*/
static int eat_one_lib_cyy(board_status *bs, intersection color) {

	int ai, aj;
	int k;
	int pos = -1;
	for (ai = 0; ai < board_size; ai++) {
		for (aj = 0; aj < board_size; aj++) {
			//仅有一口气的棋串，必须吃掉
			if (legal_move(bs, ai, aj, color) && !suicide(bs, ai, aj, color)) {
				int sum = 0;
				for (k = 0; k < 4; k++) {
					int bi = ai + deltai[k];
					int bj = aj + deltaj[k];
					int pos = POS(ai, aj);

					if (ON_BOARD(bi, bj)
						&& bs->board[POS(bi, bj)] == OTHER_COLOR(color)
						&& !has_additional_liberty(bs, bi, bj, ai, aj)
						&& sim_for_string(bs, pos, color)){
						sum = sum + bs->string_stones[bs->string_index[get_father(bs, POS(bi, bj))]];
						if (sum >= 2){
							return pos;
						}

					}
				}
			}
		}
	}

	return pos;
}

/*duif*/


/* -1 for there is no stone to save
检查是否需要救子*/
//only_lib不能用了，要换方法
//static int save_stone(board_status *bs, intersection color) {
//	int si;
//	int lpos;
//
//	for (si = 1; si <= bs->num_of_strings; si++) {
//		if (bs->string_color[si] == color) {
//			lpos = only_lib(bs, si);
//			if (lpos != -1 && is_legal_move(bs, color, lpos) && sim_for_string(bs, lpos, color)){
//				return lpos;
//			}
//		}
//	}
//	return -1;
//}

/*检查自己这么下了之后会不会只剩一口气*/
int sim_for_string(board_status *bs, int pos, intersection color){
	int k;
	int sim_approximate_liberty = 0;
	int i = I(pos);
	int j = J(pos);

	// 落子以后update周围棋串的气和自己的气
	for (k = 0; k < 4; k++) {
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);
		sim_approximate_liberty += provides_liberty_sim(bs, ai, aj, i, j, color);
	}

	if (sim_approximate_liberty <= 1){
		return 0;
	}
	else{
		return 1;
	}
}


/* -1 for there is no stone to capture
检查是否可以提气，就是下了一步之后对方就只剩一个气了*/
//only_lib不能用了
//static int atari_move(board_status *bs, intersection color) {
//	int si;
//	int lpos;
//
//	for (si = 1; si <= bs->num_of_strings; si++) {
//		if (bs->string_color[si] == OTHER_COLOR(color)) {
//			lpos = only_lib(bs, si);
//			if (lpos != -1 && is_legal_move(bs, color, lpos))
//				return lpos;
//		}
//	}
//	return -1;
//}

/*根据k得到转换后的位置上的颜色。
三种翻转分别为，上下-对称；左右|对称；沿对角线\翻转。
根据k的0,1,2位得到翻转后的坐标*/
static intersection get_turn(board_status *bs, int k,
	int di, int dj, int pos)
{
	int tmp;
	if (k & 1)
		di = 2 - di;
	if ((k >> 1) & 1)
		dj = 2 - dj;
	if ((k >> 2) & 1) {
		tmp = di;
		di = dj;
		dj = tmp;
	}
	//检测边缘
	return bs->board[pos + POS(di, dj)];
}


static int match_border(board_status *bs, intersection color,
	int pos, int dir)
{
	intersection f[3][3];
	int zpos = POS(I(pos) - 1, J(pos) - 1);
	f[0][1] = get_turn(bs, dir, 0, 1, zpos);
	//[0,1][0,0]反色
	if (f[0][1] == EMPTY)
	{
		f[0][0] = get_turn(bs, dir, 0, 0, zpos);
		f[1][0] = get_turn(bs, dir, 1, 0, zpos);
		if (f[0][1] == OTHER_COLOR(f[1][0]))
		{
			//printf("try border1...succeed\n");
			return 1;//border1
		}
	}
	else{
		f[1][2] = get_turn(bs, dir, 1, 2, zpos);

		if (f[0][1] == OTHER_COLOR(f[1][2]))
		{
			f[1][0] = get_turn(bs, dir, 1, 0, zpos);
			if (f[1][0] != f[0][1])
			{
				//printf("try border2...succeed\n");
				return 1;//border2
			}
		}
		f[0][2] = get_turn(bs, dir, 0, 2, zpos);
		if (f[0][2] == OTHER_COLOR(f[0][1]))
		{
			if (color == f[0][1])
			{
				//printf("try border3...succeed\n");
				return 1;//border3
			}
			else if (f[1][2] != OTHER_COLOR(color))
			{
				//printf("try border4...succeed\n");
				return 1;//border4
			}
			else if (f[1][2] == f[0][1])
			{
				f[1][0] = get_turn(bs, dir, 1, 0, zpos);
				if (color == f[1][0])
				{
					//printf("try border5...succeed\n");
					return 1;//border5
				}

			}
		}
	}
	return 0;
}


/*检查是否有match的pattern,如果有的话返回对应的数字，没有就是0*/
static int match_pattern(board_status *bs, intersection color,
	int pos)
{
	intersection f[3][3];
	int k;
	int i, zpos;//zpos 是pattern中左上角在棋盘的位置

	////考虑在棋盘边缘，对应paper里第四类pattern，暂时还没加
	i = isLine(I(pos), J(pos));
	switch (i)
	{
	case 0: break;
	case 1:
	{
		if (match_border(bs, color, pos, 1) || match_border(bs, color, pos, 3))//上下翻转 或 上下翻，左右翻
			return 7;
		else
			break;
	}
	case 2:
	{
		if (match_border(bs, color, pos, 6) || match_border(bs, color, pos, 4))//对角线，左右  或  对角线
			return 8;
		else
			break;
	}
	case 3:
	{
		if (match_border(bs, color, pos, 2) || match_border(bs, color, pos, 0))//左右  或  无
			return 9;
		else
			break;
	}
	case 4:
	{
		if (match_border(bs, color, pos, 7) || match_border(bs, color, pos, 5))//对角线，上下，左右  或  对角线，上下
			return 10;
		else
			break;
	}
	}

	//旋转棋盘
	//看pattern是否超过边缘
	f[1][1] = bs->board[pos];
	if (!ON_BOARD(I(pos) - 1, J(pos) - 1) || !ON_BOARD(I(pos) + 1, J(pos) + 1))
	{
		return 0;
	}
	zpos = POS(I(pos) - 1, J(pos) - 1);
	for (k = 0; k <= 7; k++) {

		// FILE *debug_file;
		// debug_file = fopen("debug.log", "a");
		// fprintf(debug_file, "%d\n", k);
		// int ii, jj;
		// for (ii = 0; ii <= 2; ii++) {
		//     for (jj = 0; jj <= 2; jj++) {
		//         f[ii][jj] = get_turn(bs, k, ii, jj, zpos);
		//         fprintf(debug_file, "%3d ", f[ii][jj]);
		//     }
		//     fprintf(debug_file, "\n");
		// }
		// fclose(debug_file);

		f[0][1] = get_turn(bs, k, 0, 1, zpos);
		f[1][0] = get_turn(bs, k, 1, 0, zpos);
		//[0,1][1,0]异色
		if (f[0][1] == OTHER_COLOR(f[1][0])){
			f[1][2] = get_turn(bs, k, 1, 2, zpos);
			if (f[0][1] == OTHER_COLOR(f[1][2])){
				f[2][0] = get_turn(bs, k, 2, 0, zpos);
				f[2][1] = get_turn(bs, k, 2, 1, zpos);
				f[2][2] = get_turn(bs, k, 2, 2, zpos);
				if (f[2][0] != f[1][0] && f[2][1] != f[1][0] && f[2][2] != f[1][0])
					return 2; //CUT2
			}
		}

		f[0][0] = get_turn(bs, k, 0, 0, zpos);
		//[0,0]位置上不为空
		if (f[0][0] != EMPTY) {
			f[0][1] = get_turn(bs, k, 0, 1, zpos);
			//[0,1][0,0]反色
			if (f[0][1] == OTHER_COLOR(f[0][0])) {
				f[1][0] = get_turn(bs, k, 1, 0, zpos);
				if (f[1][0] == f[0][1]) {
					//[1,0][0,1]同色
					f[1][2] = get_turn(bs, k, 1, 2, zpos);
					f[2][1] = get_turn(bs, k, 2, 1, zpos);
					if ((f[1][2] == f[0][1] && f[2][1] == EMPTY) || (f[2][1] == f[0][1] && f[1][2] == EMPTY))
						continue;
					else
						return 1; // Cut1
				}
				else if (f[1][0] == EMPTY) {
					f[1][2] = get_turn(bs, k, 1, 2, zpos);
					if (f[1][2] != EMPTY)
						continue;
					f[0][2] = get_turn(bs, k, 0, 2, zpos);
					if (f[0][2] == f[0][0])
						return 3; // Hane1
					f[2][1] = get_turn(bs, k, 2, 1, zpos);
					if (f[2][1] != EMPTY)
						continue;
					if (f[0][2] == EMPTY)
						return 4; // Hane2
					if (f[0][2] == f[0][1] && color == f[0][0])
						return 6; // Hane4
				}
				//[1,0][0,1]反色
				else if (f[0][1] == OTHER_COLOR(f[1][0])){
					f[1][2] = get_turn(bs, k, 1, 2, zpos);
					f[2][1] = get_turn(bs, k, 2, 1, zpos);
					if (f[1][2] == EMPTY && f[2][1] == EMPTY)
						return 5; // Hane3
				}
			}
		}
	}
	return 0;
}

static int pattern(board_status *bs, intersection color)
{
	int lpos, res;
	int li, lj, di, dj;
	lpos = bs->last_move_pos;
	li = I(lpos);
	lj = J(lpos);
	for (di = -2; di <= 0; di++)
		for (dj = -2; dj <= 0; dj++) {
		if (ON_BOARD(li + di + 1, lj + dj + 1) && bs->board[POS(li + di + 1, lj + dj + 1)] == EMPTY)
		{
			res = match_pattern(bs, color, POS(li + di + 1, lj + dj + 1));

			if (res)
			{
				//printf("try pattern...%d succeed\n", res);
				return POS(li + di + 1, lj + dj + 1);
			}

		}
		}
	return -1;
}

/* Generate a random move. */
int generate_random_move(board_status *bs, intersection color) {
	int move, pos;
	pos = -14;

	//if (bs->path_top > 20)
	//{
	//	//save
	//	pos = get_atari_pos(bs, color);

	//	////nakada
	//	//if (bs->path_top > 40)
	//	//{
	//	//	if (pos == -14 || !is_legal_move(bs, color, pos))
	//	//		pos = get_nakade_pos(bs, color, color);//自己做眼
	//	//	if (pos == -14 || !is_legal_move(bs, color, pos))
	//	//		pos = get_nakade_pos(bs, OTHER_COLOR(color), color);//防止别人做眼
	//	//}

	//	////fillboard
	//	//if (pos == -14 && bs->path_top > 50)
	//	//	pos = get_fill_board_pos(bs, color);

	//	//pattern
	//	if (pos == -14 || !is_legal_move(bs, color, pos))
	//		pos = pattern(bs, color);

	//	//capture
	//	if (pos == -14 || !is_legal_move(bs, color, pos))
	//		pos = get_eat_pos(bs, color);

	//	////紧气
	//	//if (pos == -14 || !is_legal_move(bs, color, pos))
	//	//	pos = get_approach_pos(bs, color);

	//	if (pos >= 0 && is_legal_move(bs, color, pos))
	//		return pos;
	//}

	//random


	int flag = 0;
	int num_of_random = 0;
	//中场结束之前 
	//printf("bs->path_top == %d", bs->path_top);
	if (bs->path_top < STEPS_MIDDLE_END){
		while (num_of_random < MAX_LEGGAL_RANDOM)
		{
			move = rand() % MAX_BOARDSIZE;
			if (is_legal_move(bs, color, move)){
				return move;
			}
			num_of_random++;
		}

		flag = 1;
	}

	//中场结束之后 or 中场结束之前random失败
	if (bs->path_top >= STEPS_MIDDLE_END || flag){
		//获得棋盘上当前所有的合法位置 利用棋盘状态来传递
		get_legal_moves(bs, color);

		/* Choose one of the considered moves randomly with uniform
		* distribution. (Strictly speaking the moves with smaller 1D
		* coordinates tend to have a very slightly higher probability to be
		* chosen, but for all practical purposes we get a uniform
		* distribution.)
		*/
		//如果存在合法移动 legal_moves里面装的是合法步骤的i*13+j
		if (bs->legal_moves_num > 0) {
			move = bs->legal_moves[rand() % bs->legal_moves_num];
			return move;
		}
		else {
			/* But pass if no move was considered. */
			return -14;
		}

	}

}

/* Generate a move. */
int generate_move(board_status *bs, intersection color) {
	int pos = -14, i;

	//开局布局
	if (bs->path_top < STEPS_START_END - 1)
		pos = get_fuseki_pos(bs, color);

	//判断是否合法
	get_legal_moves(bs, color);
	for (i = 0; i < MAX_BOARDSIZE; ++i)
	{
		result_to_uct[i] = -1;
		result_to_uct_pos[i] = i;
	}
	for (i = 0; i < bs->legal_moves_num; ++i)
	{
		result_to_uct[bs->legal_moves[i]] = 0;
	}

	if (pos==-14 && bs->path_top<STEPS_BOUNDARY_TWO-1)
		get_two_boundary_pos(bs, result_to_uct);//10步以内忽略最外两圈
	else if (pos == -14 && bs->path_top<STEPS_BOUNDARY_ONE - 1)
		get_one_boundary_pos(bs, result_to_uct);//20步以内忽略最外一圈

	//忽略四周都是空的点
	if (pos == -14 && bs->path_top<40 && bs->path_top>10)
		get_all_fill_board_pos(bs, result_to_uct);

	//吃子救子紧气
	if (pos == -14)
		get_all_eat_save_lib_pos(bs, color, result_to_uct);

	//有真眼就做
	if (pos == -14)
		get_all_alive_pos(bs, color, result_to_uct);

	//连接和切断
	if (pos == -14)
		get_all_connect_cut_pos(bs, color, result_to_uct);

	//做眼
	if (pos == -14)
		get_all_nakade_pos(bs, color, result_to_uct);

	
	//开局棋谱
	//chess_manual_result[i]=x 表示pos=i的位置在棋谱里出现了多少次，没有出现过就是0
	if (pos == -14 && bs->path_top < STEPS_MANUAL_END - 1)
	{
		get_manual_start_pos(bs, color, result_to_uct);

		//for (i = 0; i < MAX_BOARDSIZE; ++i)
		//{
		//	printf("%d ", chess_manual_result[i]);
		//}
		//printf("\n");

		//当前我先返回出现次数最多的点，到时候应该把下面这部分去掉，然后下面uct_search(bs, color, chess_manual_result)
		//当然如果走菱形、吃子这些选出来的点也要传到uct的话，也一起加进去好了
		//最好换个数据结构，比如用堆
		//int max_chess = 0;
		//for (i = 0; i < MAX_BOARDSIZE; ++i)
		//{
		//	if (result_to_uct[i]>max_chess)
		//	{
		//		pos = i;
		//		max_chess = result_to_uct[i];
		//	}
		//}
	}

	//避免填眼
	if (pos == -14)
		get_ban_fill_eye_pos(bs, color, result_to_uct);


	//不进入uct的操作begin
	//能救子就救子
	if (pos == -14 && bs->path_top < STEPS_EAT_SAVE_END)
		pos = get_atari_pos(bs, color);
	if (pos == -14 && bs->path_top < STEPS_EAT_SAVE_END)
		pos = get_eat_pos(bs, color);
	//不进入uct的操作end

	//uct
	if (pos == -14 || !is_legal_move(bs, color, pos))
	{
		//候选点的排序（记得打开这里）
		quick_sort_for_result(0,MAX_BOARDSIZE-1);

		/*for (int i = 0; i < MAX_BOARDSIZE; ++i)
			printf("%d ", result_to_uct[i]);
		printf("\n");
		for (int i = 0; i < MAX_BOARDSIZE; ++i)
			printf("%d ", result_to_uct_pos[i]);
		printf("\n");*/

		//pos = uct_search(bs, color, flag, chess_manual_result);
		pos = uct_search_pure(bs, color, result_to_uct, result_to_uct_pos);
	}
	bs->path_top++;
	bs->path[bs->path_top] = pos;
	return pos;
}


/* Put free placement handicap stones on the board. We do this simply
* by generating successive black moves.
*/
void place_free_handicap(board_status *bs, int handicap) {
	int k;
	int pos;

	for (k = 0; k < handicap; k++) {
		pos = generate_move(bs, BLACK);
		play_move_game(bs, I(pos), J(pos), BLACK);
	}
}


/* 快排 */
int divide_result(int low,int high)
{
	int tmp = result_to_uct[low];
	int pos = result_to_uct_pos[low];
	do{
		while (low<high && result_to_uct[high] <= tmp)
			--high;
		if (low < high)
		{
			result_to_uct[low] = result_to_uct[high];
			result_to_uct_pos[low] = result_to_uct_pos[high];
			++low;
		}
		while (low < high && result_to_uct[low] >= tmp)
			++low;
		if (low < high)
		{
			result_to_uct[high] = result_to_uct[low];
			result_to_uct_pos[high] = result_to_uct_pos[low];
			--high;
		}
	} while (low!=high);
	result_to_uct[low] = tmp;
	result_to_uct_pos[low] = pos;
	return low;
}

void quick_sort_for_result(int low, int high)
{
	int mid;
	if (low >= high)
		return;
	mid = divide_result(low, high);
	quick_sort_for_result(low, mid - 1);
	quick_sort_for_result(mid + 1, high);
}