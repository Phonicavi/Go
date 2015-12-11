#ifndef _GOTODIE_H_
#define _GOTODIE_H_

#include "board.h"

#define GOTODIE_NAME "GoToDie"
#define GOTODIE_VERSION "1.0"

void init_gotodie(void);
int generate_move(board_status *bs, intersection color);
void place_free_handicap(board_status *bs, int handicap);


#endif