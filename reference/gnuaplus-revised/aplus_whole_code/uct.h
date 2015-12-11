#ifndef _UCT_H_
#define _UCT_H_

#include "board.h"
#include "zobrist.h"
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

#define MAX_SIM_NUM 20000
#define MAX_THREAD 4
#define MAX_VISITS 10
/* 0.44 = sqrt(1/5) */
#define UCTK 0.44

typedef struct node {
	/* 置换表的键值 */
	U64 key;

    float wins;
    int visits;
	//int depth;
	struct edge *edge;
}uct_node;

typedef struct edge{
	short pos;
	short allocate;
	struct node *child;
	struct edge *sibling;
}uct_edge;

/**********************************置换表******************************************/

/* 哈希表大小 */
#define ZOBRIST_HASH_TABLT_SIZE 65536 //置换表的大小，必须是2的n次方
#define ZOBRIST_HASH_TABLE_SIZE_MASK ZOBRIST_HASH_TABLT_SIZE-1

typedef struct zobrist_hash_table_node{
	U64 key;
	uct_node *un;
	intersection board[MAX_BOARDSIZE];
}hash_table_head;

void init_zobrist_hash();
static int query_zobrist_hash(board_status *bs, uct_node *un, intersection color, int pos, uct_node **new_node);

/**********************************置换表******************************************/

static uct_node *uct_root;
static intersection uct_color;
static board_status *uct_bs;
static HANDLE h[MAX_THREAD];
static board_status hboard[MAX_THREAD];
static time_t start;
static int f[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static int count = 0;
//static int deep = 0;

/* 置换表要用到的 */
static int hash_hit = 0;
static int query_total = 0;
static hash_table_head hash_table[ZOBRIST_HASH_TABLT_SIZE];
static uct_node *already[500000];
static int top = -1;

float simulate_game(board_status *bs, intersection color);
int uct_search(board_status *bs, intersection ucolor, int result[], int result_pos[]);
void add_value(board_status *bs, uct_node *uct_root, int result[], int result_pos[]);
unsigned _stdcall sim_thread(void* xi);

//static函数
//static void init_uct_node(uct_node *un);不用了，换为un->edge=NULL;就可以了
static void init_uct_edge(short pos, uct_node *child, uct_edge *ue, short allocate);
static void free_uct_tree(uct_node *un);
static int create_uct_children(board_status *bs, intersection color, uct_node *un);
static int first_create_uct_children(board_status *bs, intersection color, uct_node *un, int result[], int result_pos[]);
static uct_edge* uct_select(uct_node *un);
//static void update_node(float res, uct_node *un);
static float simulate(board_status *bs, intersection color, uct_node *un);
static uct_edge* get_best_child(uct_node *uct_root);



#endif
