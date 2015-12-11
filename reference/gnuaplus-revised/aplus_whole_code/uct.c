#include "board.h"
#include "aplus.h"
#include "uct.h"
#include "log.h"
#include "zobrist.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

float simulate_game(board_status *bs, intersection color)
{
//	print_line("simulate_game");
    int pass[3];
    intersection color_now;
    int pos, step;
	int score;
	//float score;

    step = 0;
    pass[OTHER_COLOR(color)] = (bs->last_move_pos == -14);
    pass[color] = 0;
    color_now = color;

    while (!(pass[BLACK] && pass[WHITE]) && step <= 200) {
		//当前着手方
		/* Generate a random move. */
		//clock_t start, end;
		//start = clock();

        pos = generate_random_move(bs, color_now);

		//end = clock();
		//printf("Time in gen random move: %d", end - start / CLOCKS_PER_SEC);
        //如果没有可移动的地方 那么就pass
		pass[color_now] = (pos == -14);
        play_move(bs, I(pos), J(pos), color_now);
        color_now = OTHER_COLOR(color_now);
        step++;
    }
    // debug_log_board_status(bs);
	//下面是只看输赢
	score = get_score_uct(bs);
	count++;
	if (score > 0 && color == WHITE)
		return 1;
	if (score < 0 && color == BLACK)
		return 1;
	return 0;

	////下面是分数作为wins
	//score = get_score_uct_white(bs)/169;
	//count++;
	//   if (color == WHITE)
	//       return score;
	//   if (color == BLACK)
	//       return 1-score;
}

static void init_uct_edge(short pos, uct_node *child, uct_edge *ue, short allocate)
{
	ue->pos = pos;
	ue->child = child;
	ue->sibling = NULL;
	ue->allocate = allocate;
}

static int create_uct_children(board_status *bs, intersection color, uct_node *un)
{
    int mi,hit;
	uct_edge *new_edge, *now;
    uct_node *new_node;

    get_legal_moves(bs, color);

    if (bs->legal_moves_num == 0)
        return 0;

	//【①置换表begin】
	new_edge = (uct_edge *)malloc(sizeof(uct_edge));
	hit = query_zobrist_hash(bs,un, color, bs->legal_moves[0],&new_node);
	init_uct_edge(bs->legal_moves[0], new_node, new_edge, hit);
	//【①置换表end】

	////【不用置换表begin】
	//new_edge = (uct_edge *)malloc(sizeof(uct_edge));
	//new_node = (uct_node *)malloc(sizeof(uct_node));
	//new_node->edge = NULL;
	//new_node->wins = -1;
	//new_node->visits = -1;
	////new_node->depth = un->depth + 1;
	//init_uct_edge(bs->legal_moves[0], new_node, new_edge,1);
	////【不用置换表end】

	un->edge = new_edge;
	now = un->edge;

    for (mi = 1; mi < bs->legal_moves_num; mi++) 
	{
		////【不用置换表begin】
		//new_edge = (uct_edge *)malloc(sizeof(uct_edge));
		//new_node = (uct_node *)malloc(sizeof(uct_node));
		//new_node->edge = NULL;
		//new_node->wins = -1;
		//new_node->visits = -1;
		////new_node->depth = un->depth + 1;
		//init_uct_edge(bs->legal_moves[mi], new_node, new_edge,1);
		////【不用置换表end】

		//【②置换表begin】
		new_edge = (uct_edge *)malloc(sizeof(uct_edge));
		hit = query_zobrist_hash(bs, un, color, bs->legal_moves[mi], &new_node);
		init_uct_edge(bs->legal_moves[mi], new_node, new_edge, hit);
		//【②置换表end】

        now->sibling = new_edge;
        now = now->sibling;
    }
    return bs->legal_moves_num;
}

static int first_create_uct_children(board_status *bs, intersection color, uct_node *un, int result[], int result_pos[])
{
	int mi, hit;
	uct_edge *new_edge, *now;
	uct_node *new_node;

	if (result[0] == -1) return 0;

	//【①置换表begin】
	new_edge = (uct_edge *)malloc(sizeof(uct_edge));
	hit = query_zobrist_hash(bs,un, color, result_pos[0],&new_node);
	init_uct_edge(result_pos[0], new_node, new_edge, hit);
	//【①置换表end】

	////【不用置换表begin】
	//new_edge = (uct_edge *)malloc(sizeof(uct_edge));
	//new_node = (uct_node *)malloc(sizeof(uct_node));
	//new_node->edge = NULL;
	//new_node->wins = -1;
	//new_node->visits = -1;
	////new_node->depth = un->depth + 1;
	//init_uct_edge(result_pos[0], new_node, new_edge, 1);
	////【不用置换表end】

	un->edge = new_edge;
	now = un->edge;

	for (mi = 1; mi < MAX_BOARDSIZE; mi++) {
		if (result[mi] == -1) break;

		////【不用置换表begin】
		//new_edge = (uct_edge *)malloc(sizeof(uct_edge));
		//new_node = (uct_node *)malloc(sizeof(uct_node));
		//new_node->edge = NULL;
		//new_node->wins = -1;
		//new_node->visits = -1;
		////new_node->depth = un->depth + 1;
		//init_uct_edge(result_pos[mi], new_node, new_edge, 1);
		////【不用置换表end】

		//【②置换表begin】
		new_edge = (uct_edge *)malloc(sizeof(uct_edge));
		hit = query_zobrist_hash(bs, un, color, result_pos[mi], &new_node);
		init_uct_edge(result_pos[mi], new_node, new_edge, hit);
		//【②置换表end】

		now->sibling = new_edge;
		now = now->sibling;
	}
	return mi;
}

static uct_edge* uct_select(uct_node *un)
{
    uct_edge *now, *tmp;
	float uct_value, max_uct_value, win_rate,visits;

    now = un->edge;
    max_uct_value = -1.0;
    tmp = NULL;
    while (now != NULL) {
		visits = now->child->visits;
		if (visits > 0) {
			win_rate = now->child->wins * 1.0 / visits;
			uct_value = win_rate + UCTK*sqrt(log(un->visits)*1.0 / visits);
        } else
            uct_value = 10000 + (1000 * rand());
        if (uct_value > max_uct_value) {
            max_uct_value = uct_value;
            tmp = now;
        }
        now = now->sibling;
    }
	//printf("max_value: %f\n", max_uct_value);
    return tmp;
}

//static void update_node(float res, uct_node *un) {
//    un->wins += res;
//    un->visits += 1;
//}

static float simulate(board_status *bs, intersection color, uct_node *un) {
	float res;
    uct_edge *next;

    if (un->edge == NULL && un->visits < MAX_VISITS)
        res = simulate_game(bs, color);
    else {
        if (un->edge == NULL)
            create_uct_children(bs, color, un);
        next = uct_select(un);
        if (next == NULL) {
            next = (uct_edge *)malloc(sizeof(uct_edge));
			uct_node *next_un = (uct_node *)malloc(sizeof(uct_node));
			next_un->edge = NULL;
			next_un->wins = -1;
			next_un->visits = -1;
			next_un->key = 0;
            init_uct_edge(POS(-1,-1),next_un,next,1);
        }
        play_move(bs, I(next->pos), J(next->pos), color);
        color = OTHER_COLOR(color);
        res = simulate(bs, color, next->child);
        res = 1 - res;
    }

    //update_node(1 - res, un);
	un->wins += 1 - res;
	un->visits += 1;
    return res;
}

static uct_edge* get_best_child(uct_node *uct_root)
{
    int max_visits = -1;
    uct_edge *un, *bn;
    un = uct_root->edge;
    bn = NULL;
    while (un != NULL) {
        // printf("%d\n", un->visits);
        if (un->child->visits > max_visits) {
            max_visits = un->child->visits;
            bn = un;
        }
        un = un->sibling;
    }
    return bn;
}

unsigned _stdcall sim_thread(void* xi) {
    time_t now = clock();
    int i, j = 0;
    int *hi = (int *)xi;
	//printf("One_thread now...\n");
 //   debug_log_int(*hi);
    while (now - start < 9500 && j < 400) {
        for (i = 0; i <= 100; i++) {
			memcpy(&hboard[*hi], uct_bs, sizeof(hboard[*hi]));
            simulate(&hboard[*hi], uct_color, uct_root);
        }
        j++;
        now = clock();
    }
	printf("Toal Sim: %d\n", count);
	//printf("Hash Hit: %d\n", hash_hit);
	//printf("Query Total: %d\n", query_total);
}

////加权重的函数
//void add_value(board_status *bs, uct_node *uct_root, int result[], int result_pos[])
//{
//	uct_node *un, *bn;
//	int i;
//	un = uct_root->edge->child;
//	while (un != NULL)
//	{
//		for (i = 0; i < sizeof(*result) / 4; ++i)
//		{
//			if (un->edge->pos == result_pos[i] && result[i])
//			{
//				//具体权重再议
//				un->wins = 6;
//				un->visits = 6;
//			}
//		}
//		un = un->edge->sibling;
//	}
//}


int uct_search(board_status *bs, intersection ucolor, int result[], int result_pos[])
{
	//【置换表begin】
	init_zobrist_hash();
	//【置换表end】

    uct_edge *best_child;
    board_status uct_board;
    int i, node_num, pos;

	start = clock();
	count = 0;
	hash_hit = 0;
	query_total = 0;
	top = -1;
	//deep = 0;

    uct_color = ucolor;
    uct_bs = bs;

    memcpy(&uct_board, bs, sizeof(uct_board));
    uct_root = (uct_node *)malloc(sizeof(uct_node));
	uct_root->edge = NULL;
	uct_root->wins = -1;
	uct_root->visits = -1;
	//uct_root->depth = 0;

	//【③置换表begin】
	uct_root->key = get_zobrist_from_board(uct_bs);
	//【③置换表end】

    node_num = create_uct_children(&uct_board, ucolor, uct_root);//旧的
	//node_num = first_create_uct_children(&uct_board, ucolor, uct_root,result,result_pos);
	//if (flag) add_value(bs, uct_root, chess_manual_result);

    for (i = 0; i < MAX_THREAD; i++)
        h[i] = (HANDLE)_beginthreadex(NULL, 0, sim_thread, &f[i], 0, NULL);
    for (i = 0; i < MAX_THREAD; i++)
        WaitForSingleObject(h[i], INFINITE);

    best_child = get_best_child(uct_root);

    if (best_child == NULL)
        pos = POS(-1,-1);
    else
        pos = best_child->pos;

    free_uct_tree(uct_root);
	//printf("Max depth:%d \n", deep);
	//printf("Toal Sim: %d\n", count);
	//因为线程问题，在这里printf不出总count
	//printf("Best move from uct: [%d, %d] \n", I(pos), J(pos));
    return pos;
}

////【不用置换表begin】
//static void free_uct_tree(uct_node *un)
//{
//	if (un == NULL || un == 0xfeeefeee)
//		return;
//
//	//if (un->depth > deep)
//	//	deep = un->depth;
//
//	uct_edge *tmp, *old;
//	tmp = un->edge;
//	while (tmp != NULL)
//	{
//		free_uct_tree(tmp->child);
//		old = tmp;
//		tmp = tmp->sibling;
//		free(old);
//	}
//	free(un);
//}
////【不用置换表end】

/***********************************************************************************
* 模块：置换表begin
***********************************************************************************/

//【置换表】
static void free_uct_tree(uct_node *un)
{
	if (un == NULL || un == 0xfeeefeee)
		return;
	int i,flag=1;

	uct_edge *tmp, *old;
	tmp = un->edge;
	while (tmp != NULL && tmp != 0xfeeefeee)
	{
		//如果这个tmp->child是它分配的，就由它来释放
		if (tmp->allocate)
			free_uct_tree(tmp->child);

		old = tmp;
		tmp = tmp->sibling;
		free(old);
	}
	free(un);	
}

//return 1表示没有查到，所以这个新的uct_node是新分配的，由分配的来删除
static int query_zobrist_hash(board_status *bs, uct_node *un, intersection color, int pos, uct_node **new_node)
{
	query_total++;
	int i;

	//如果有吃子，直接新建一个节点，不要查表，不加进置换表
	if ((pos - 13 >= 0 && bs->board[pos - 13] == OTHER_COLOR(color) && bs->approximate_liberty[bs->string_index[pos - 13]] == 1)
		|| (pos + 13 < MAX_BOARDSIZE &&  bs->board[pos + 13] == OTHER_COLOR(color) && bs->approximate_liberty[bs->string_index[pos + 13]] == 1)
		|| (J(pos) != 0 && bs->board[pos - 1] == OTHER_COLOR(color) && bs->approximate_liberty[bs->string_index[pos - 1]] == 1)
		|| (J(pos) != MAX_BOARD - 1 && bs->board[pos + 1] == OTHER_COLOR(color) && bs->approximate_liberty[bs->string_index[pos + 1] == 1]))
	{
		(*new_node) = (uct_node *)malloc(sizeof(uct_node));
		(*new_node)->wins = -1;
		(*new_node)->visits = -1;
		(*new_node)->edge = NULL;
		(*new_node)->key = 0;
		return 1;
	}
	else
	{
		//新节点的key，看是否在置换表里面已经有了
		U64 key = get_updated_zobrist(un->key, pos, color);
		hash_table_head *head_node = &hash_table[key & ZOBRIST_HASH_TABLE_SIZE_MASK];

		int flag = 1;
		if (head_node->un != NULL && head_node->key == key)
		{
			for (i = 0; i < MAX_BOARDSIZE; ++i)
				if (bs->board[i] != head_node->board[i] && i != pos)
				{
				flag = 0; //printf("conflic\n");
				break;
				}
			if (flag && head_node->board[pos] == color)
			{
				hash_hit++;
				(*new_node) = head_node->un;
				return 0;
			}
		}
		(*new_node) = (uct_node *)malloc(sizeof(uct_node));
		(*new_node)->wins = -1;
		(*new_node)->visits = -1;
		(*new_node)->edge = NULL;
		(*new_node)->key = key;
		head_node->un = (*new_node);
		head_node->key = key;
		for (i = 0; i < MAX_BOARDSIZE; ++i)
			head_node->board[i] = bs->board[i];
		head_node->board[pos] = color;
		return 1;
	}
}


void init_zobrist_hash()
{
	int i, j;
	for (i = 0; i < ZOBRIST_HASH_TABLT_SIZE; ++i)
	{
		hash_table[i].key = 0;
		hash_table[i].un = NULL;
		for (j = 0; j < MAX_BOARDSIZE; ++j)
			hash_table[i].board[j] = 0;
	}
}

/***********************************************************************************
* 模块：置换表end
***********************************************************************************/