#ifndef TRICKS_CHESS_MANUAL

#define SCORE_MANUAL_HIT 0.5  //在棋盘上出现一次能得到的分数
#define SCORE_EAT_EACH 2  //吃一颗子能得到的分数
#define SCORE_SAVE_EACH 3  //救一颗子能得到的分数
#define SCORE_APPROACH_EACH 1 //紧气一颗子能得到的分数
#define CONNECT_CUT_MIN_NUM 4  //连接和切断的下限
#define SCORE_CONNECT_EACH 1  //连接一颗子能得到的分数
#define SCORE_CUT_EACH 1  //切断一颗子能得到的分数
#define SCORE_REAL_TYE_EACH 4 //做一个真眼能得的分数
#define SCORE_FILL_BOARD -1 //填棋盘上一个四周都没有子的点
#define SCORE_NAKADE 1 //不让对方做眼的位置

/* trick：布局 */
int get_fuseki_pos(board_status *bs, intersection color);

/* trick：做活 */
void get_all_alive_pos(board_status *bs, intersection color, int result[]);

/* trick：吃子救子紧气 */
int get_eat_pos(board_status *bs, intersection color);
int get_atari_pos(board_status *bs, intersection color);
int get_approach_pos(board_status *bs, intersection color);
void get_all_eat_save_lib_pos(board_status *bs, intersection color, int result[]);

/* trick：避免填眼 */
void get_ban_fill_eye_pos(board_status *bs, intersection color, int result[]);

/* trick：连接和切断 */
int get_connect_cut_pos(board_status *bs, intersection color);
void get_all_connect_cut_pos(board_status *bs, intersection color, int result[]);

/* trick：前面几步不能下在边界上 */
void get_one_boundary_pos(board_status *bs, int result[]);
void get_two_boundary_pos(board_status *bs, int result[]);

/* trick：找四周都是空的点 */
int get_fill_board_pos(board_status *bs, intersection color);
void get_all_fill_board_pos(board_status *bs, int result[]);

/* trick：做眼 */
int get_nakade_pos(board_status *bs, intersection color, intersection color_current);
void get_all_nakade_pos(board_status *bs, intersection color, int result[]);

#endif