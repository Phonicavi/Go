#include "board.h"
#include "aplus.h"
#include "log.h"
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
// static int hi;


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
	//score = get_score_uct_white(bs);
	//count++;
 //   if (color == WHITE)
 //       return score;
 //   if (color == BLACK)
 //       return 169-score;
}
//
//static void init_uct_node(int wins, int visits, uct_node_pure *un)
//{
//    un->wins = wins;
//    un->visits = visits;
//	un->pos = -14;//POS(-1, -1);
//    un->child = NULL;
//    un->sibling = NULL;
//}

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
            //uct_value = win_rate + UCTK*sqrt(log(un->visits)/visits); //原来的，第二项参数是根号（1/5）
			//uct_value = win_rate + max(win_rate * (1 - win_rate), 0.001) * sqrt(log(un->visits) / visits);
			uct_value = win_rate + max(win_rate * (1 - win_rate), 0.001) * sqrt(log(un->visits) / visits) + log(un->visits) / visits;
			//if (now->visits > 0) {
			//	win_rate = now->wins * 1.0 / now->visits;
			//	uct_value = win_rate + UCTK*sqrt(log(un->visits)*1.0 / now->visits);
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

//static void update_node(double res, uct_node_pure *un) {
//    un->wins += res;
//    un->visits += 1;
//}

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

	////用分数作为wins要用下面这些
	//	res = 169 - res;
	//}
	//un->wins += 169 - res;
	//un->visits += 169;

	//用输赢作为wins要用下面这些
        res = 1 - res;
    }
    //update_node(1 - res, un);
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
		//printf("pos：（%d, %d）  winrate: %f  value: %f  visits: %d\n", I(un->pos), J(un->pos), win_rate, va, un->visits);
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
	//printf("One_thread now...\n");
 //   debug_log_int(*hi);
    while (now - start < 9500 && j < 400 && (count < 1000000)) {
        for (i = 0; i <= 100; i++) {
        	memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
            simulate(&hboard[*hi], gcolor, groot);
        }
        j++;
        now = clock();
    }
	//best_child = get_best_child(groot);
	//printf("pos：（%d, %d）  winrate: %f  win： %f    visits: %d\n", I(best_child->pos), J(best_child->pos), (best_child->wins / best_child->visits), best_child->wins, best_child->visits);
	//printf("20000: \n");
	//while (now - start < 9500 && j < 400 && (count < 20000)) {
	//	for (i = 0; i <= 100; i++) {
	//		memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
	//		simulate(&hboard[*hi], gcolor, groot);
	//	}
	//	j++;
	//	now = clock();
	//}
	//best_child = get_best_child(groot);

	//printf("pos：（%d, %d）  winrate: %f  win： %f    visits: %d\n", I(best_child->pos), J(best_child->pos), (best_child->wins / best_child->visits), best_child->wins, best_child->visits);
	//printf("30000: \n");
	//while (now - start < 9500 && j < 400 && (count < 30000)) {
	//	for (i = 0; i <= 100; i++) {
	//		memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
	//		simulate(&hboard[*hi], gcolor, groot);
	//	}
	//	j++;
	//	now = clock();
	//}
	//best_child = get_best_child(groot);

	//printf("pos：（%d, %d）  winrate: %f  win： %f    visits: %d\n", I(best_child->pos), J(best_child->pos), (best_child->wins / best_child->visits), best_child->wins, best_child->visits);	
	//printf("40000: \n");
	//while (now - start < 9500 && j < 400 && (count < 40000)) {
	//	for (i = 0; i <= 100; i++) {
	//		memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
	//		simulate(&hboard[*hi], gcolor, groot);
	//	}
	//	j++;
	//	now = clock();
	//}
	//best_child = get_best_child(groot);

	//printf("pos：（%d, %d）  winrate: %f  win： %f    visits: %d\n", I(best_child->pos), J(best_child->pos), (best_child->wins / best_child->visits), best_child->wins, best_child->visits);	
	//printf("50000: \n");
	//while (now - start < 9500 && j < 400 && (count < 50000)) {
	//	for (i = 0; i <= 100; i++) {
	//		memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
	//		simulate(&hboard[*hi], gcolor, groot);
	//	}
	//	j++;
	//	now = clock();
	//}
	//best_child = get_best_child(groot);

	//printf("pos：（%d, %d）  winrate: %f  win： %f    visits: %d\n", I(best_child->pos), J(best_child->pos), (best_child->wins / best_child->visits), best_child->wins, best_child->visits);
	//printf("60000: \n");
	//while (now - start < 9500 && j < 400 && (count < 60000)) {
	//	for (i = 0; i <= 100; i++) {
	//		memcpy(&hboard[*hi], gbs, sizeof(hboard[*hi]));
	//		simulate(&hboard[*hi], gcolor, groot);
	//	}
	//	j++;
	//	now = clock();
	//}
	//printf("40000: \n");

	printf("Toal Sim: %d\n", count);
//    debug_log_int(100*j);
}

int uct_search_pure(board_status *bs, intersection ucolor, int result[], int result_pos[])
{
	uct_node_pure *uct_root;
	uct_node_pure *best_child;
    board_status uct_board;
    int i, node_num, pos;

	start = clock();
	count = 0;

	/*printf("Input max sim: ");
	scanf(&MAX_SIM);*/

    memcpy(&uct_board, bs, sizeof(uct_board));
	uct_root = (uct_node_pure *)malloc(sizeof(uct_node_pure));
	uct_root->visits = -1;
	uct_root->wins = -1;
	uct_root->pos = -14;
	uct_root->child = NULL;
	uct_root->sibling = NULL;
	//node_num = create_uct_children(&uct_board, ucolor, uct_root);//旧的
	node_num = first_create_uct_children(&uct_board, ucolor, uct_root,result,result_pos);

	//if (flag) add_value(bs, uct_root, chess_manual_result);

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
	//printf("pos：（%d, %d）  winrate: %f  win： %f    value: %f  visits: %d\n", I(best_child->pos), J(best_child->pos), win_rate, va , best_child->visits);

    if (best_child == NULL)
        pos = POS(-1,-1);
    else
        pos = best_child->pos;

    free_uct_tree(uct_root);
	//printf("Toal Sim: %d\n", count);
	//因为线程问题，在这里printf不出总count
	//printf("Best move from uct: [%d, %d] \n", I(pos), J(pos));
    return pos;
}