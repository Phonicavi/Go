#include <stdio.h>
#include <stdlib.h> //���rand��������
#include <string.h>
#include <time.h>
#include "aplus.h"
#include "uct_pure.h"
#include "board.h"
#include "tricks_chess_manual.h"
#include "tricks_chess.h"
#include "bit_process.h"

int isLine(int i, int j) {
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

//initial
void init_aplus(void){
	clear_board(&main_board);
	init_trick_chess_manual();
}

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



/* Generate a random move. */
int generate_random_move(board_status *bs, intersection color) {
	int move, pos;
	pos = -14;

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

	if (pos == -14 && bs->path_top<STEPS_BOUNDARY_TWO - 1)
		get_two_boundary_pos(bs, result_to_uct);//10�����ں���������Ȧ
	else if (pos == -14 && bs->path_top<STEPS_BOUNDARY_ONE - 1)
		get_one_boundary_pos(bs, result_to_uct);//20�����ں�������һȦ

	//�������ܶ��ǿյĵ�
	if (pos == -14 && bs->path_top>10 && bs->path_top<40)
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
		quick_sort_for_result(0, MAX_BOARDSIZE - 1);
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
int divide_result(int low, int high) {
	int tmp = result_to_uct[low];
	int pos = result_to_uct_pos[low];
	do{
		while (low < high && result_to_uct[high] <= tmp)
			--high;
		if (low < high) {
			result_to_uct[low] = result_to_uct[high];
			result_to_uct_pos[low] = result_to_uct_pos[high];
			++low;
		}
		while (low < high && result_to_uct[low] >= tmp)
			++low;
		if (low < high) {
			result_to_uct[high] = result_to_uct[low];
			result_to_uct_pos[high] = result_to_uct_pos[low];
			--high;
		}
	} while (low != high);
	result_to_uct[low] = tmp;
	result_to_uct_pos[low] = pos;
	return low;
}

void quick_sort_for_result(int low, int high) {
	int mid;
	if (low >= high)
		return;
	mid = divide_result(low, high);
	quick_sort_for_result(low, mid - 1);
	quick_sort_for_result(mid + 1, high);
}