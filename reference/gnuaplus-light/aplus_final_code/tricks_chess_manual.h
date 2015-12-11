#ifndef TRICKS_CHESS_MANUAL

#include "board.h"
#include "tricks_chess.h"

#define MANUAL_HASH_TABLE_SIZE 362 //����������ж��ٸ�����
#define BUFFER_STACK_SIZE 10000 //������ʱ�õ���ջ�Ĵ�С


/* �������׵ĸ�֣��ö����ֵ�����ʾ�� */
typedef struct bit_config{

	/* ��ǰ���� */
	bitboard board_bit[MAX_BOARD];

	/* ��һ���ֵ��ڻ��ǰ� */
	intersection color;

	/* �Ѿ����˶��ٿ��� */
	short num_current;//1,2,3...

	/* ���� */
	struct bit_config_edge *edge_son;

	/* ����������ͬ����һ�� */
	struct bit_config *next;

	/* ��������������г��ֵĴ��� */
	int times;

}config;

/* ���׶���֮�������ıߣ��ö����ֵ�����ʾ�� */
typedef struct bit_config_edge{
	/* �����Ŀ��ӣ�����һ����������һ����ֵı仯 */
	int pos;

	/* ������һ��� */
	struct bit_config *config_son;

	/* �ֵ� */
	struct bit_config_edge *brother;

}config_edge;

/* ��ɢ�б� */
typedef struct bit_hash_note{
	/* �ֵ�ʲô��ɫ���� */
	intersection color;

	/* ������Ŀǰ�ж��ٿ��� */
	short num_current;

	/* head */
	struct bit_config *head;
}hash_node;


/* �����׵�ʱ���õ���ջ,�����config��ָ�� */
typedef struct stack_chess_manual_buffer
{
	config *stack[BUFFER_STACK_SIZE];
	int top;
}chess_manual_buffer;

chess_manual_buffer buffer_stack;

hash_node hash_array[MANUAL_HASH_TABLE_SIZE];




/* trick 2: ������������ */
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