#ifndef TRICKS_CHESS_MANUAL

#include "board.h"
#include "tricks_chess.h"

#define MANUAL_HASH_TABLE_SIZE 362 //棋谱上最多有多少个棋子
#define BUFFER_STACK_SIZE 10000 //读棋谱时用到的栈的大小


/* 保存棋谱的格局，用儿子兄弟链表示法 */
typedef struct bit_config{

	/* 当前局面 */
	bitboard board_bit[MAX_BOARD];

	/* 下一步轮到黑还是白 */
	intersection color;

	/* 已经下了多少颗子 */
	short num_current;//1,2,3...

	/* 儿子 */
	struct bit_config_edge *edge_son;

	/* 和它层数相同的下一个 */
	struct bit_config *next;

	/* 这个局面在棋谱中出现的次数 */
	int times;

}config;

/* 父亲儿子之间相连的边，用儿子兄弟链表示法 */
typedef struct bit_config_edge{
	/* 下了哪颗子，即下一个格局相对上一个格局的变化 */
	int pos;

	/* 连到下一格局 */
	struct bit_config *config_son;

	/* 兄弟 */
	struct bit_config_edge *brother;

}config_edge;

/* 开散列表 */
typedef struct bit_hash_note{
	/* 轮到什么颜色下了 */
	intersection color;

	/* 棋盘上目前有多少颗子 */
	short num_current;

	/* head */
	struct bit_config *head;
}hash_node;


/* 读棋谱的时候用到的栈,存的是config的指针 */
typedef struct stack_chess_manual_buffer
{
	config *stack[BUFFER_STACK_SIZE];
	int top;
}chess_manual_buffer;

chess_manual_buffer buffer_stack;

hash_node hash_array[MANUAL_HASH_TABLE_SIZE];




/* trick 2: 按照棋谱来走 */
void get_manual_start_pos(board_status *bs, intersection color, int result[]);
void get_manual_start_pos_sub(board_status *bs, intersection color, int offset_x, int offset_y, int is_oppsite, int result[]);
void init_trick_chess_manual();
void init_hash_array();
void init_config(config *con, bitboard *bb, intersection c, short nc);
void init_config_edge(config_edge *edge, int p, config *con, config_edge *brother);
void refresh_config(config **con, int i, int j, intersection color);
void make_chess_manual_tree();
void chess_manual_buffer_push(config *push);
config *chess_manual_buffer_pop();
config *chess_manual_buffer_top();
int is_fine_move(int pos);

#endif