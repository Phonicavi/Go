/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is Brown, a simple go program.                           *
 *                                                               *
 * Copyright 2003 and 2004 by Gunnar Farnebäck.                  *
 *                                                               *
 * Permission is hereby granted, free of charge, to any person   *
 * obtaining a copy of this file gtp.c, to deal in the Software  *
 * without restriction, including without limitation the rights  *
 * to use, copy, modify, merge, publish, distribute, and/or      *
 * sell copies of the Software, and to permit persons to whom    *
 * the Software is furnished to do so, provided that the above   *
 * copyright notice(s) and this permission notice appear in all  *
 * copies of the Software and that both the above copyright      *
 * notice(s) and this permission notice appear in supporting     *
 * documentation.                                                *
 *                                                               *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY     *
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE    *
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR       *
 * PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO      *
 * EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS  *
 * NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR    *
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING    *
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF    *
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT    *
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS       *
 * SOFTWARE.                                                     *
 *                                                               *
 * Except as contained in this notice, the name of a copyright   *
 * holder shall not be used in advertising or otherwise to       *
 * promote the sale, use or other dealings in this Software      *
 * without prior written authorization of the copyright holder.  *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "uctNode.h"



#define MIN_BOARD 2
#define MAX_BOARD 23

/* These must agree with the corresponding defines in gtp.c. */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

/* Used in the final_status[] array. */
#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define UNKNOWN 5

extern float komi;
extern int board_size;
extern int lastMovei;
extern int lastMovej;
extern int rivalMovei;
extern int rivalMovej;
extern int step;

int deltai[4] = { -1, 1, 0, 0 };
int deltaj[4] = { 0, 0, -1, 1 };

//below defined by us

#define IMPACT 32
#define IMPACTDIS 5
#define AIMOVEMAX 10
#define SAMECOLOR 4
#define DIFFERENTCOLOR 2
#define STARTMEDIAN 5
#define CORNERSIZE 5
#define MINMAXRANGE 3
#define BLACKEDGE 4
#define WHITEEDGE -BLACKEDGE
#define MEDIANMINMAX 50
#define TRYTIME 10
#define MAXSTEP 60
#define TIMELIMIT 2
#define MAXGAMES 5000
#define MONTECARLORANGE 13
#define CLEARTIME 10



void init_brown(void);
void clear_board(void);
int board_empty(void);
int get_board(int i, int j);
int get_string(int i, int j, int *stonei, int *stonej);
int legal_move(int i, int j, int color);

void generate_move(int *i, int *j, int color);
void compute_final_status(void);
int get_final_status(int i, int j);
void set_final_status(int i, int j, int status);
int valid_fixed_handicap(int handicap);
void place_fixed_handicap(int handicap);
void place_free_handicap(int handicap);

//////////////////////
int calcDisImpact(int d);
int minmin(int a, int b);
int maxmax(int a, int b);
void calcGame(int *b, int *w, int *bScore, int *wScore);
int checkDistance(int x0, int y0, int x1, int y1);
void aiMoveGreedy2(int *pos, int color,int *moves,int num_moves);
void aiMoveMonteCarlo(int *pos, int color,int *moves,int num_moves);
void aiMovePreCheck(int *pos, int color, int *moves, int num_moves);
void aiMove(int *pos, int color,int *moves,int num_moves);

int autoRun(int color);
int autoRun2(int color);
bool available(int i, int j, int color);
void getAvailableMonteCarloMove(uctNode *root, int *games);

bool cmpLess(const uctNode *a, const uctNode *b);
bool cmpMore(const uctNode* a, const uctNode *b);

void mylog(const char str[]);
void mylog(int num);
void mylog(int num1, int num2);
void mylog(const char str[], int num);

void show_game();
/*
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */
void calScore(uctNode* tmp, int c);
void uctSearch(int *pos, int color, int *moves, int num_moves);
void backup(uctNode* v, int reward);
int defaultPolicy(int color);
uctNode* treePolicy(uctNode* v);
uctNode* bestchild(uctNode* curNode, int c);
uctNode* expand(uctNode* curNode, int* moves, int num_moves);
int generate_legal_moves(int* moves, int color);

int checkLiberty(int i, int j);
int findALiberty(int i, int j);