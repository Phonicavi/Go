#ifndef _APLUS_H_
#define _APLUS_H_

#include "board.h"

//3.0 only pattern
//6.0 ���˾��ӡ������������Ĳ�������20������������
//6.1 ���׵�ƥ������һ��Ľ���������������ɫ�Ƚϣ���ԭ��ռ�ĸ��Ǳ�Ϊռ������
//6.2 ����һ����ȫһ�������kogo���Ա������Խǣ����½ǵ����Ҫ��������
//6.3 ���׸Ľ����������׵Ĳ���ԭ�����ˣ��������׵Ĳ��õ���Ҫ�����������Ĳ���ԭ���Ǹ��ǲ��õ��·�
//6.4 ����һ�����۵�С���裬��ʵ�ͳ��Ӳ��
//6.5 6.4�Ļ������ټ���һ��㣬���ı��ϵ�
//6.6 pull�ˣ���7.1�ϲ�
//6.7 ����ֻ��ǰ10��
//6.8 ����������ƥ���ȫ��λ�ã���uctʹ�ã�Ŀǰ�����������λ��
//6.9 �������Լ����ۣ���uctǰ��ӵģ�������һ�α�������Ϊ���ӵ��µ����ۣ���֪�������ò��ã��о��۵��жϻ�����������
//6.10 ��7.3�ϲ�
//6.11 7.7֮��uct����������Ϊ�������
//6.12 uct�������û���aplus�����uct����ķ�����ע��������

//7 ����һ�����ȳ���
//7.1 �ܱ����Ӽ��������Ե���������2
//7.2 ��uct �����pattern ���ӵ����ȼ���Ϊ���
//7.3 ��6.9�ϲ� ע�͵����� pattern�����̵�ʱ����uct�������� 10-100��֮��������
//7.4 1/3�ĸ�������ǰ���Ӻͳ��ֺ�����
//7.5 ��7.4 �޸�random
//7.6 ��7.5 ������
//7.7 ��7.5 �޸�random 20��

//test 1 �����Ǹ�illegal�����׵Ĺ�ϵ
//test uct ����uct��illegal�Ĺ�ϵ
//test qipu ����qipu��illegal�Ĺ�ϵ

//8 uct+qipu+pattern+random�Ż�

//9 ������8�Ĵ���(pattern,�û���,����)
//9.1 ����İ汾�ϲ������Ǹ�����̫���ˣ�����uct��pattern���û������֡����ס�����trick�ѹأ�
//9.2 �̳�9.1����uct+�û���
//9.3 ����uct���ݽṹ�Ĵ�uct
//9.4 ��uct���ݽṹ֮ǰ�Ĵ�uct
//9.5 ����uct���ݽṹ��uct+�û���
//9.6 ������9.5���û�����󣬻���uct���ݽṹ��uct+�û���uct��΢�Ż���һ��
//9.7 9.6�����ϣ���΢�Ż�����uct��û���û���İ汾
//9.8 ��uct+����4��+������+���Ӿ��ӽ���+����+�����ж�+�����ۡ���ɵ��ź���ĺ�ѡ�㣨û���û��� 
//9.9 �̳�9.8 �޸�illegal
//9.10 �̳�9.9 �޸�trick����ĵ���

//10 �̳�9.1 ��pattern ȫ�̶���pattern
//10.1 �̳�9.1 ��pattern �г�ʹ��pattern
//10.2 �̳�9.1 ֻ��Χ�嶨ʽ

//t2.4 ��uct��score��Ϊwins + ����tricks
//t2.2.4 ��uct��score��Ϊwins + ����tricks + pattern�г�
//1.4 
//1.3.4
//1.1.3.4
//t1 ��uct_pure
//1.3.4-boundary

//11 ȫ��
//11_1 �ص�uct��Ĳ��ԣ���α��
//11_2 �ص�uct��Ĳ��ԣ���α��
//11_3 trickȫ�򿪣���������10������uct�����ģ��Ҫ�ӳ��Ӿ��Ӻ�pattern
//11_4 trickȫ�򿪣���������20������uct�����ģ��Ҫ�ӳ��Ӿ��Ӻ�pattern��10~40���������ܶ�û���ӵĵ�
//11_5 �ص�uct��Ĳ��ԣ���α����10~40���������ܶ�û���ӵĵ�
//11_6 11_4�����ϣ����Ӿ���ȫ��
//11_7 11_5�����ϣ����Ӿ���ȫ��

//Aplus_whole	ȫ������ ���aplus_11_7�Ĺ���

#define APLUS_NAME "Aplus_whole"
#define APLUS_VERSION "13"

#define PATH_CHESS_MANUAL "E:\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "C:\\Users\\qiaomai\\Documents\\gnuaplus_new\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "C:\\Users\\happyqiaomai\\Documents\\GnuAplus\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "W:\\qxt\\computer\\AI\\teamwork\\go\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "E:\\Users\\daniyuu\\Visual Studio 2013\\source\\gnuaplus\\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "F:\\�˹�����\\final_project\\GnuAplus\\bitbucket\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "E:\\Users\\yueying\\visual studio 2013\\Source\\gnuaplus\\chess_manual\\kogo4.SGF"

//�ֶ�
#define STEPS_START_END 3 //����Ҫ�߶��ٲ��̶�λ��
#define STEPS_MANUAL_END 10 //�����ٲ�Ϊֹ������������
#define STEPS_EAT_SAVE_END 140 //���Ӻ;��ӵĲ�������
#define STEPS_CONNECT_CUT_END 40 //���Ӻ��жϵĲ�������
#define STEPS_MIDDLE_START 50	//�г���ʼ
#define STEPS_MIDDLE_END 100	//�г�����
#define MAX_LEGGAL_RANDOM 20	//���random hit���Բ���
#define STEPS_BOUNDARY_TWO 10 //���ٲ����ں�����������Ȧ�����
#define STEPS_BOUNDARY_ONE 20 //���ٲ����ں�����������Ȧһ���

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