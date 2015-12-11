#ifndef _UCT_PURE_H_
#define _UCT_PURE_H_

#include "board.h"

#define MAX_SIM_NUM 20000
#define MAX_THREAD 4
#define MAX_VISITS 10
/* 0.44 = sqrt(1/5) */
#define UCTK 0.44

typedef struct node_pure {
    double wins;
    int visits;
    int pos;
    struct node_pure *child, *sibling;
}uct_node_pure;



int uct_search_pure(board_status *bs, intersection ucolor, int result[], int result_pos[]);//, int flag, int* chess_manual_result);

#endif
