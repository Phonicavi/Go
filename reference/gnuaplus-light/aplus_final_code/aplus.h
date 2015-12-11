#ifndef _APLUS_H_
#define _APLUS_H_

#include "board.h"

#define APLUS_NAME "Aplus_final"
#define APLUS_VERSION "12.0"

#define PATH_CHESS_MANUAL "F:\\Course\\2015-2016_Autumn\\Artificial_Intelligence\\Go\\resource\\gnuaplus-revised\\chess_manual\\kogo4.SGF"

//�ֶ�
#define STEPS_START_END 3			//����Ҫ�߶��ٲ��̶�λ��
#define STEPS_MANUAL_END 10			//�����ٲ�Ϊֹ������������
#define STEPS_EAT_SAVE_END 140		//���Ӻ;��ӵĲ�������
#define STEPS_CONNECT_CUT_END 40	//���Ӻ��жϵĲ�������
#define STEPS_MIDDLE_START 50		//�г���ʼ����
#define STEPS_MIDDLE_END 100		//�г���������
#define MAX_LEGGAL_RANDOM 20		//��random���legalmove������� �����Ż�
#define STEPS_BOUNDARY_TWO 10		//���ٲ����ں�����������Ȧ�����
#define STEPS_BOUNDARY_ONE 20		//���ٲ����ں�����������Ȧһ���

//������֮ǰ�ģ�result_to_uct[i]��ʾpos=i��λ�õ�Ȩ��
//���Ϸ���-1
//����ͨ�ĵ㣺0
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
