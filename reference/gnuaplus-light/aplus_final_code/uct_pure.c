#include "board.h"
#include "aplus.h"
#include "uct_pure.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

int MAX_SIM = 20000;

static time_t start;
static HANDLE h[MAX_THREAD];
static board_status *gbs;
static intersection gcolor;
static board_status hboard[MAX_THREAD];
static uct_node_pure *groot;
static int f[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int count = 0;

static double simulate_game(board_status *bs, intersection color)
{
    int pass[3];
    intersection color_now;
    int pos, step;
    double score;

    step = 0;
    pass[OTHER_COLOR(color)] = (bs->last_move_pos == -14);
    pass[color] = 0;
    color_now = color;

    while (!(pass[BLACK] && pass[WHITE]) && step <= 200) {
		//当前着手方
		/* Generate a random move. */
        pos = generate_random_move(bs, color_now);
        //如果没有可移动的地方 那么就pass
		pass[color_now] = (pos == -14);
        play_move(bs, I(pos), J(pos), color_now);
        color_now = OTHER_COLOR(color_now);
        step++;
    }

	//下面是只看输赢
	score = get_score_uct(bs);
	count++;
	if (score > 0 && color == WHITE)
		return 1;
	if (score < 0 && color == BLACK)
		return 1;
	return 0;

}

static void free_uct_tree(uct_node_pure *un)
{
    if (un != NULL) {
        free_uct_tree(un->child);
        free_uct_tree(un->sibling);
        int k = un->pos;
        free(un);
    }
}

static int create_uct_children(board_status *bs, intersection color, uct_node_pure *un)
{
    int mi;
	uct_node_pure *now, *new_node;

    get_legal_moves(bs, color);

    if (bs->legal_moves_num == 0)
        return 0;

	new_node = (uct_node_pure *)malloc(sizeof(uct_node_pure));
	new_node->visits = -1;
	new_node->wins = -1;
	new_node->child = NULL;
	new_node->sibling = NULL;
    new_node->pos = bs->legal_moves[0];

    un->child = new_node;
    now = un->child;

    for (mi = 1; mi < bs->legal_moves_num; mi++) {
		new_node = (uct_node_pure *)malloc(sizeof(uct_node_pure));
		new_node->visits = -1;
		new_node->wins = -1;
		new_node->child = NULL;
		new_node->sibling = NULL;
        new_node->pos = bs->legal_moves[mi];

        now->sibling = new_node;
        now = now->sibling;
    }
    return bs->legal_moves_num;
}


static int first_create_uct_children(board_status *bs, intersection color, uct_node_pure *un, int result[], int result_pos[])
{
	int mi;
	uct_node_pure *now, *new_node;

	if (result[0] == -1) return 0;

	new_node = (uct_node_pure *)malloc(sizeof(uct_node_pure));
	new_node->visits = -1;
	new_node->wins = -1;
	new_node->child = NULL;
	new_node->sibling = NULL;
	new_node->pos = result_pos[0];

	un->child = new_node;
	now = un->child;

	for (mi = 1; mi < MAX_BOARDSIZE; mi++) {
		if (result[mi] == -1) break;

		new_node = (uct_node_pure *)malloc(sizeof(uct_node_pure));
		new_node->visits = -1;
		new_node->wins = -1;
		new_node->child = NULL;
		new_node->sibling = NULL;
		new_node->pos = result_pos[mi];

		now->sibling = new_node;
		now = now->sibling;
	}
	return mi;
}

static uct_node_pure* uct_select(uct_node_pure *un)
{
	uct_node_pure *now, *tmp;
    double uct_value, max_uct_value, win_rate,visits;

    now = un->child;
    max_uct_value = -1.0;
    tmp = NULL;
    while (now != NULL) {
		visits = now->visits;
        if (visits > 0) {
            win_rate = now->wins / visits;
			uct_value = win_rate + max(win_rate * (1 - win_rate), 0.001) * sqrt(log(un->visits) / visits) + log(un->visits) / visits;
        } else
            uct_value = 10000 + (1000 * rand());
        if (uct_value > max_uct_value) {
            max_uct_value = uct_value;
            tmp = now;
        }
        now = now->sibling;
    }
    return tmp;
}

static double simulate(board_status *bs, intersection color, uct_node_pure *un) {
    double res;
	uct_node_pure *next;

    if (un->child == NULL && un->visits < MAX_VISITS)
        res = simulate_game(bs, color);
    else {
        if (un->child == NULL)
            create_uct_children(bs, color, un);
        next = uct_select(un);
        if (next == NULL) {
			next = (uct_node_pure *)malloc(sizeof(uct_node_pure));
			next->visits = -1;
			next->wins = -1;
			next->pos = -14;
			next->child = NULL;
			next->sibling = NULL;
        }
        play_move(bs, I(next->pos), J(next->pos), color);
        color = OTHER_COLOR(color);
		res = simulate(bs, color, next);

	//用输赢作为wins要用下面这些
        res = 1 - res;
    }
	un->wins += 1 - res;
	un->visits++;

    return res;
}

static uct_node_pure* get_best_child(uct_node_pure *uct_root)
{
    int max_visits = -1;
	uct_node_pure *un, *bn;
    un = uct_root->child;
    bn = NULL;
    while (un != NULL) {
		double win_rate, n_visit, c_visit;
		win_rate = (un->wins / un->visits);

		n_visit = uct_root->visits;
		c_visit = un->visits;
		double va;
		va = win_rate + max(win_rate * (1 - win_rate), 0.001) * sqrt(log(c_visit) / n_visit) + log(c_visit) / n_visit;
		if (un->visits > max_visits) {
            max_visits = un->visits;
            bn = un;
        }
        un = un->sibling;
    }
    return bn;
}

unsigned _stdcall sim_thread_pure(void* xi) {
    time_t now = clock();
    int i, j = 0;
    int *hi = (int *)xi;
	uct_node_pure *best_child;
    while (now - start < 9500 && j < 400 && (count < 1000000)) {
        for (i = 0; i <= 100; i++) {
        	memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
            simulate(&hboard[*hi], gcolor, groot);
        }
        j++;
        now = clock();
    }
}

int uct_search_pure(board_status *bs, intersection ucolor, int result[], int result_pos[])
{
	uct_node_pure *uct_root;
	uct_node_pure *best_child;
    board_status uct_board;
    int i, node_num, pos;

	start = clock();
	count = 0;

    memcpy(&uct_board, bs, sizeof(uct_board));
	uct_root = (uct_node_pure *)malloc(sizeof(uct_node_pure));
	uct_root->visits = -1;
	uct_root->wins = -1;
	uct_root->pos = -14;
	uct_root->child = NULL;
	uct_root->sibling = NULL;
	node_num = first_create_uct_children(&uct_board, ucolor, uct_root,result,result_pos);

    groot = uct_root;
	gcolor = ucolor;
    gbs = bs;
    for (i = 0; i < MAX_THREAD; i++)
		h[i] = (HANDLE)_beginthreadex(NULL, 0, sim_thread_pure, &f[i], 0, NULL);
    for (i = 0; i < MAX_THREAD; i++)
        WaitForSingleObject(h[i], INFINITE);

	double win_rate, n_visit, c_visit;
    best_child = get_best_child(uct_root);
	win_rate = (best_child->wins / best_child->visits);

	n_visit = uct_root->visits;
	c_visit = best_child->visits;
	double va;
	va = win_rate + max(win_rate * (1 - win_rate), 0.001) * sqrt(log(c_visit) / n_visit) + log(c_visit) / n_visit;

    if (best_child == NULL)
        pos = POS(-1,-1);
    else
        pos = best_child->pos;

    free_uct_tree(uct_root);

	return pos;
}