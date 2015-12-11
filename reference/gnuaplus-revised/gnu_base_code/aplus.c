#include <stdio.h>
#include <stdlib.h> //���rand��������
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
		return 1; //��
	else if (j == (board_size - 1) && (i) >= 1 && (i) < (board_size - 1))
		return 2; //��
	else if (i == (board_size - 1) && (j) >= 1 && (j) < (board_size - 1))
		return 3; //��
	else if (j == 0 && (i) >= 1 && (i) < (board_size - 1))
		return 4; //��
	else
		return 0;

}


void init_aplus(void){
	clear_board(&main_board);
	init_trick_chess_manual();
	init_zobrist();
}

/*�Ե���ʣһ�������ӣ�ʵ�ַ������ú�ʱ�ģ���Ϊά�����ķ����������⣩*/
static int eat_one_lib(board_status *bs, intersection color)
{

	int ai, aj;
	int k;
	int pos = -1;
	for (ai = 0; ai < board_size; ai++) {
		for (aj = 0; aj < board_size; aj++) {
			//����һ�������崮������Ե�
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


/*�Ե���ʣһ�������� ������1.�ڸ����� 2.�Է����崮��Ŀ���ڵ���3*/
static int eat_one_lib_cyy(board_status *bs, intersection color) {

	int ai, aj;
	int k;
	int pos = -1;
	for (ai = 0; ai < board_size; ai++) {
		for (aj = 0; aj < board_size; aj++) {
			//����һ�������崮������Ե�
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
����Ƿ���Ҫ����*/
//only_lib�������ˣ�Ҫ������
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

/*����Լ���ô����֮��᲻��ֻʣһ����*/
int sim_for_string(board_status *bs, int pos, intersection color){
	int k;
	int sim_approximate_liberty = 0;
	int i = I(pos);
	int j = J(pos);

	// �����Ժ�update��Χ�崮�������Լ�����
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
����Ƿ������������������һ��֮��Է���ֻʣһ������*/
//only_lib��������
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

/*����k�õ�ת�����λ���ϵ���ɫ��
���ַ�ת�ֱ�Ϊ������-�Գƣ�����|�Գƣ��ضԽ���\��ת��
����k��0,1,2λ�õ���ת�������*/
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
	//����Ե
	return bs->board[pos + POS(di, dj)];
}


static int match_border(board_status *bs, intersection color,
	int pos, int dir)
{
	intersection f[3][3];
	int zpos = POS(I(pos) - 1, J(pos) - 1);
	f[0][1] = get_turn(bs, dir, 0, 1, zpos);
	//[0,1][0,0]��ɫ
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


/*����Ƿ���match��pattern,����еĻ����ض�Ӧ�����֣�û�о���0*/
static int match_pattern(board_status *bs, intersection color,
	int pos)
{
	intersection f[3][3];
	int k;
	int i, zpos;//zpos ��pattern�����Ͻ������̵�λ��

	////���������̱�Ե����Ӧpaper�������pattern����ʱ��û��
	i = isLine(I(pos), J(pos));
	switch (i)
	{
	case 0: break;
	case 1:
	{
		if (match_border(bs, color, pos, 1) || match_border(bs, color, pos, 3))//���·�ת �� ���·������ҷ�
			return 7;
		else
			break;
	}
	case 2:
	{
		if (match_border(bs, color, pos, 6) || match_border(bs, color, pos, 4))//�Խ��ߣ�����  ��  �Խ���
			return 8;
		else
			break;
	}
	case 3:
	{
		if (match_border(bs, color, pos, 2) || match_border(bs, color, pos, 0))//����  ��  ��
			return 9;
		else
			break;
	}
	case 4:
	{
		if (match_border(bs, color, pos, 7) || match_border(bs, color, pos, 5))//�Խ��ߣ����£�����  ��  �Խ��ߣ�����
			return 10;
		else
			break;
	}
	}

	//��ת����
	//��pattern�Ƿ񳬹���Ե
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
		//[0,1][1,0]��ɫ
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
		//[0,0]λ���ϲ�Ϊ��
		if (f[0][0] != EMPTY) {
			f[0][1] = get_turn(bs, k, 0, 1, zpos);
			//[0,1][0,0]��ɫ
			if (f[0][1] == OTHER_COLOR(f[0][0])) {
				f[1][0] = get_turn(bs, k, 1, 0, zpos);
				if (f[1][0] == f[0][1]) {
					//[1,0][0,1]ͬɫ
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
				//[1,0][0,1]��ɫ
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
	//	//		pos = get_nakade_pos(bs, color, color);//�Լ�����
	//	//	if (pos == -14 || !is_legal_move(bs, color, pos))
	//	//		pos = get_nakade_pos(bs, OTHER_COLOR(color), color);//��ֹ��������
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

	//	////����
	//	//if (pos == -14 || !is_legal_move(bs, color, pos))
	//	//	pos = get_approach_pos(bs, color);

	//	if (pos >= 0 && is_legal_move(bs, color, pos))
	//		return pos;
	//}

	//random


	int flag = 0;
	int num_of_random = 0;
	//�г�����֮ǰ 
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

	//�г�����֮�� or �г�����֮ǰrandomʧ��
	if (bs->path_top >= STEPS_MIDDLE_END || flag){
		//��������ϵ�ǰ���еĺϷ�λ�� ��������״̬������
		get_legal_moves(bs, color);

		/* Choose one of the considered moves randomly with uniform
		* distribution. (Strictly speaking the moves with smaller 1D
		* coordinates tend to have a very slightly higher probability to be
		* chosen, but for all practical purposes we get a uniform
		* distribution.)
		*/
		//������ںϷ��ƶ� legal_moves����װ���ǺϷ������i*13+j
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

	//���ֲ���
	if (bs->path_top < STEPS_START_END - 1)
		pos = get_fuseki_pos(bs, color);

	//�ж��Ƿ�Ϸ�
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
		get_two_boundary_pos(bs, result_to_uct);//10�����ں���������Ȧ
	else if (pos == -14 && bs->path_top<STEPS_BOUNDARY_ONE - 1)
		get_one_boundary_pos(bs, result_to_uct);//20�����ں�������һȦ

	//�������ܶ��ǿյĵ�
	if (pos == -14 && bs->path_top<40 && bs->path_top>10)
		get_all_fill_board_pos(bs, result_to_uct);

	//���Ӿ��ӽ���
	if (pos == -14)
		get_all_eat_save_lib_pos(bs, color, result_to_uct);

	//�����۾���
	if (pos == -14)
		get_all_alive_pos(bs, color, result_to_uct);

	//���Ӻ��ж�
	if (pos == -14)
		get_all_connect_cut_pos(bs, color, result_to_uct);

	//����
	if (pos == -14)
		get_all_nakade_pos(bs, color, result_to_uct);

	
	//��������
	//chess_manual_result[i]=x ��ʾpos=i��λ��������������˶��ٴΣ�û�г��ֹ�����0
	if (pos == -14 && bs->path_top < STEPS_MANUAL_END - 1)
	{
		get_manual_start_pos(bs, color, result_to_uct);

		//for (i = 0; i < MAX_BOARDSIZE; ++i)
		//{
		//	printf("%d ", chess_manual_result[i]);
		//}
		//printf("\n");

		//��ǰ���ȷ��س��ִ������ĵ㣬��ʱ��Ӧ�ð������ⲿ��ȥ����Ȼ������uct_search(bs, color, chess_manual_result)
		//��Ȼ��������Ρ�������Щѡ�����ĵ�ҲҪ����uct�Ļ���Ҳһ��ӽ�ȥ����
		//��û������ݽṹ�������ö�
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

	//��������
	if (pos == -14)
		get_ban_fill_eye_pos(bs, color, result_to_uct);


	//������uct�Ĳ���begin
	//�ܾ��Ӿ;���
	if (pos == -14 && bs->path_top < STEPS_EAT_SAVE_END)
		pos = get_atari_pos(bs, color);
	if (pos == -14 && bs->path_top < STEPS_EAT_SAVE_END)
		pos = get_eat_pos(bs, color);
	//������uct�Ĳ���end

	//uct
	if (pos == -14 || !is_legal_move(bs, color, pos))
	{
		//��ѡ������򣨼ǵô����
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


/* ���� */
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