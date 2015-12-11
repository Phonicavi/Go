#ifndef TRICKS_CHESS_MANUAL

#define SCORE_MANUAL_HIT 0.5  //�������ϳ���һ���ܵõ��ķ���
#define SCORE_EAT_EACH 2  //��һ�����ܵõ��ķ���
#define SCORE_SAVE_EACH 3  //��һ�����ܵõ��ķ���
#define SCORE_APPROACH_EACH 1 //����һ�����ܵõ��ķ���
#define CONNECT_CUT_MIN_NUM 4  //���Ӻ��жϵ�����
#define SCORE_CONNECT_EACH 1  //����һ�����ܵõ��ķ���
#define SCORE_CUT_EACH 1  //�ж�һ�����ܵõ��ķ���
#define SCORE_REAL_TYE_EACH 4 //��һ�������ܵõķ���
#define SCORE_FILL_BOARD -1 //��������һ�����ܶ�û���ӵĵ�
#define SCORE_NAKADE 1 //���öԷ����۵�λ��

/* trick������ */
int get_fuseki_pos(board_status *bs, intersection color);

/* trick������ */
void get_all_alive_pos(board_status *bs, intersection color, int result[]);

/* trick�����Ӿ��ӽ��� */
int get_eat_pos(board_status *bs, intersection color);
int get_atari_pos(board_status *bs, intersection color);
int get_approach_pos(board_status *bs, intersection color);
void get_all_eat_save_lib_pos(board_status *bs, intersection color, int result[]);

/* trick���������� */
void get_ban_fill_eye_pos(board_status *bs, intersection color, int result[]);

/* trick�����Ӻ��ж� */
int get_connect_cut_pos(board_status *bs, intersection color);
void get_all_connect_cut_pos(board_status *bs, intersection color, int result[]);

/* trick��ǰ�漸���������ڱ߽��� */
void get_one_boundary_pos(board_status *bs, int result[]);
void get_two_boundary_pos(board_status *bs, int result[]);

/* trick�������ܶ��ǿյĵ� */
int get_fill_board_pos(board_status *bs, intersection color);
void get_all_fill_board_pos(board_status *bs, int result[]);

/* trick������ */
int get_nakade_pos(board_status *bs, intersection color, intersection color_current);
void get_all_nakade_pos(board_status *bs, intersection color, int result[]);

#endif