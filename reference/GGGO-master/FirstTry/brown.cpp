/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is Brown, a simple go program.                           *
 *                                                               *
 * Copyright 2003 and 2004 by Gunnar Farneb鋍k.                  *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "brown.h"
#include <fstream>
#include <time.h>// for clock()
#include "algorithm"
#include "vector"
#include "uctNode.h"
#include "windows.h"
#include "string"
#include <time.h>
#include <process.h>
#include "GoBoard.h"
using namespace std;
/* The GTP specification leaves the initial board size and komi to the
 * discretion of the engine. We make the uncommon choices of 6x6 board
 * and komi -3.14.
 */
int board_size = 13;
float komi = -3.14;

#define THREAD_NUM 4

/* Board represented by a 1D array. The first board_size*board_size
 * elements are used. Vertices are indexed row by row, starting with 0
 * in the upper left corner.
 */
static int board[MAX_BOARD * MAX_BOARD];

/* Stones are linked together in a circular list for each string. */
static int next_stone[MAX_BOARD * MAX_BOARD];

/* Storage for final status computations. */
static int final_status[MAX_BOARD * MAX_BOARD];

/* Point which would be an illegal ko recapture. */
static int ko_i, ko_j;

/* Offsets for the four directly adjacent neighbors. Used for looping. */

/* Macros to convert between 1D and 2D coordinates. The 2D coordinate
 * (i, j) points to row i and column j, starting with (0,0) in the
 * upper left corner.
 */
#define POS(i, j) ((i) * board_size + (j))
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

/* Macro to find the opposite color. */
#define OTHER_COLOR(color) (WHITE + BLACK - (color))






/* Get the stones of a string. stonei and stonej must point to arrays
 * sufficiently large to hold any string on the board. The number of
 * stones in the string is returned.
 */







/* Is a move at (i, j) suicide for color? */






/* Play at (i, j) for color. No legality check is done here. We need
 * to properly update the board array, the next_stone array, and the
 * ko point.
 */









/////////////////////










void aiMoveGreedy2(int *pos, int color,int * moves,int num_moves)
{
  int * store_board = new int [board_size*board_size];
  int * store_next_stone = new int [board_size*board_size];
  for (int i = 0;i<board_size*board_size;++i)
  {
      store_board[i] = board[i];
      store_next_stone[i] = next_stone[i];
  }
  int storeko_i = ko_i;
  int storeko_j = ko_j;
  int storeStep = step;

	int b = 0, w = 0, bScore = 0, wScore = 0;
	calcGame(&b, &w, &bScore, &wScore);
	int tmpScore = b - w;
	int storePos = -1;
	for (int i = 0;i<num_moves;++i)
  {
      play_move(I(moves[i]),J(moves[i]),color);

      bScore = 0;
      wScore = 0;
      b = 0;
      w = 0;
      calcGame(&b, &w, &bScore, &wScore);
      if (color == BLACK && b - w>tmpScore)
      {
          tmpScore = b - w;
          storePos = moves[i];
      }
  		if (color == WHITE && b-w<tmpScore)
  		{
  			tmpScore = b-w;
  			storePos = moves[i];
  		}
      for(int ii = 0;ii<board_size*board_size;++ii)
      {
        board[ii] = store_board[ii];
        next_stone[ii] = store_next_stone[ii];
      }
      step = storeStep;
      ko_i = storeko_i;
      ko_j = storeko_j;
  }
	(*pos) = storePos;
  if ((*pos) == POS(ko_i,ko_j))
  {
    *pos = -1;
  }
  delete []store_board;
  delete []store_next_stone;
}
/*
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */

int autoRun2(int color)
{
  if (color!=BLACK && color!=WHITE) return 0;

  int moves1[MAX_BOARD * MAX_BOARD];
  int num_moves1 = 0;
  int moves2[MAX_BOARD * MAX_BOARD];
  int num_moves2 = 0;
  int ai, aj;
  int k;

  memset(moves1, 0, sizeof(moves1));
  memset(moves2, 0, sizeof(moves2));
  for (ai = 0; ai < board_size; ai++){
    for (aj = 0; aj < board_size; aj++) {
      /* Consider moving at (ai, aj) if it is legal and not suicide. */
      if (legal_move(ai, aj, color) && !suicide(ai, aj, color)) {
        /* Further require the move not to be suicide for the opponent... */
        if (!suicide(ai, aj, OTHER_COLOR(color)))
          moves1[num_moves1++] = POS(ai, aj);
        else {
        /* ...however, if the move captures at least one stone,
               * consider it anyway.
         */
          for (k = 0; k < 4; k++) {
            int bi = ai + deltai[k];
            int bj = aj + deltaj[k];
            if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color)) {
              moves1[num_moves1++] = POS(ai, aj);
              break;
            }
          }
        }
      }
    }
  }

  for (ai = 0; ai < board_size; ai++){
    for (aj = 0; aj < board_size; aj++) {
      /* Consider moving at (ai, aj) if it is legal and not suicide. */
      if (legal_move(ai, aj, OTHER_COLOR(color)) && !suicide(ai, aj, OTHER_COLOR(color))) {
        /* Further require the move not to be suicide for the opponent.color.. */
        if (!suicide(ai, aj, color))
          moves2[num_moves2++] = POS(ai, aj);
        else {
        /* ...however, if the move captures at least one stone,
               * consider it anyway.
         */
          for (k = 0; k < 4; k++) {
            int bi = ai + deltai[k];
            int bj = aj + deltaj[k];
            if (on_board(bi, bj) && get_board(bi, bj) == color) {
              moves2[num_moves2++] = POS(ai, aj);
              break;
            }
          }
        }
      }
    }
  }

  bool pass1 = false;
  bool pass2 = false;
  int maxStep = min(num_moves1,num_moves2)/2;
  int iterstep = 0;
  while ((!pass1 || !pass2) && iterstep<maxStep)
  {
    ++iterstep;
    int ppos;
    bool flag1 = true;
    bool flag2 = true;
    for (int i=0; i<TRYTIME; ++i)
    {
      ppos = rand()%num_moves1;
      flag1 = available(I(ppos), J(ppos), color);
      if (flag1)
      {
        play_move(I(ppos), J(ppos), color);
          break;
      }
    }
    pass1 = !flag1;

    for (int i=0; i<TRYTIME; ++i)
    {
      ppos = rand()%num_moves2;
      flag2 = available(I(ppos), J(ppos), color);
      if (flag2)
      {
        play_move(I(ppos), J(ppos), color);
          break;
      }
    }
    pass2 = !flag2;

    if (iterstep%CLEARTIME == 0)
    {
      memset(moves1, 0, sizeof(moves1));
      memset(moves2, 0, sizeof(moves2));
      for (ai = 0; ai < board_size; ai++){
        for (aj = 0; aj < board_size; aj++) {
          /* Consider moving at (ai, aj) if it is legal and not suicide. */
          if (legal_move(ai, aj, color) && !suicide(ai, aj, color)) {
            /* Further require the move not to be suicide for the opponent... */
            if (!suicide(ai, aj, OTHER_COLOR(color)))
              moves1[num_moves1++] = POS(ai, aj);
            else {
            /* ...however, if the move captures at least one stone,
                   * consider it anyway.
             */
              for (k = 0; k < 4; k++) {
                int bi = ai + deltai[k];
                int bj = aj + deltaj[k];
                if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color)) {
                  moves1[num_moves1++] = POS(ai, aj);
                  break;
                }
              }
            }
          }
        }
      }

      for (ai = 0; ai < board_size; ai++){
        for (aj = 0; aj < board_size; aj++) {
          /* Consider moving at (ai, aj) if it is legal and not suicide. */
          if (legal_move(ai, aj, OTHER_COLOR(color)) && !suicide(ai, aj, OTHER_COLOR(color))) {
            /* Further require the move not to be suicide for the opponent.color.. */
            if (!suicide(ai, aj, color))
              moves2[num_moves2++] = POS(ai, aj);
            else {
            /* ...however, if the move captures at least one stone,
                   * consider it anyway.
             */
              for (k = 0; k < 4; k++) {
                int bi = ai + deltai[k];
                int bj = aj + deltaj[k];
                if (on_board(bi, bj) && get_board(bi, bj) == color) {
                  moves2[num_moves2++] = POS(ai, aj);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
  int bScore = 0;
  int wScore = 0;
  int b = 0;
  int w = 0;
  calcGame(&b, &w, &bScore, &wScore);
  return b-w;
}







void getAvailableMonteCarloMove(uctNode *root, int *games)
{
  root->opened = true;
  int x = I(root->pos);
  int y = J(root->pos);
  int * store_board = new int [board_size*board_size];
  int * store_next_stone = new int [board_size*board_size];
  for (int i = 0;i<board_size*board_size;++i)
  {
    store_board[i] = board[i];
    store_next_stone[i] = next_stone[i];
  }
  int storeStep = step;
  int storeko_i = ko_i;
  int storeko_j = ko_j;

  for (int i = max(0, x - MONTECARLORANGE); i < min(board_size, x + MONTECARLORANGE); ++i)
  {
    for (int j = max(0, y - MONTECARLORANGE); j < min(board_size, y + MONTECARLORANGE); ++j)
    {
      if (available(i,j,OTHER_COLOR(root->color)))
      {
        uctNode *next = new uctNode(POS(i,j), OTHER_COLOR(root->color), root);
        root->addPos(next);
        play_move(i, j, OTHER_COLOR(root->color));
        ++step;
        int r = autoRun(root->color);
        ++(*games);
        next->result(r);
        for(int ii = 0;ii<board_size*board_size;++ii)
        {
          board[ii] = store_board[ii];
          next_stone[ii] = store_next_stone[ii];
        }
        step = storeStep;
        ko_i = storeko_i;
        ko_j = storeko_j;
      }
    }
  }
  delete []store_board;
  delete []store_next_stone;
}

void aiMoveMonteCarlo(int *pos, int color,int *moves,int num_moves)
{
  if (rivalMovei==-1 || rivalMovej==-1)
  {
    aiMoveGreedy2(pos,color,moves,num_moves);
    return;
  }
  srand(time(NULL));
  int bScore = 0;
  int wScore = 0;
  int b = 0;
  int w = 0;
  calcGame(&b, &w, &bScore, &wScore);

  int * store_board = new int [board_size*board_size];
  int * store_next_stone = new int [board_size*board_size];
  for (int i = 0;i<board_size*board_size;++i)
  {
    store_board[i] = board[i];
    store_next_stone[i] = next_stone[i];
  }
  int storeStep = step;
  int storeko_i = ko_i;
  int storeko_j = ko_j;

  uctNode* root = new uctNode(POS(rivalMovei,rivalMovej), OTHER_COLOR(color), NULL);
  int games = 0;
  getAvailableMonteCarloMove(root, &games);
  while (games < MAXGAMES)
  {
    uctNode *tmp = root;
    while (tmp->play>1 && tmp->nextMove.size() > 0 && tmp->opened)
    {
      if (tmp!=root)
      {
        play_move(I(tmp->pos), J(tmp->pos), tmp->color);
      }
      for (int ii = 0; ii < tmp->nextMove.size(); ++ii)
      {
        uctNode *tt = tmp->nextMove[ii];
        tt->score = (tt->playResult + 0.0) / tt->play + sqrtf(2*log(games)/tt->play);
      }
      sort(tmp->nextMove.begin(), tmp->nextMove.end(), cmpLess);
      if (tmp->color==WHITE)
      {
        tmp = tmp->nextMove[tmp->nextMove.size() - 1];
      }
      else
      {
        tmp = tmp->nextMove[0];
      }
    }
    if (tmp->opened && tmp->nextMove.size() == 0)
    {
      break;
    }
    getAvailableMonteCarloMove(tmp, &games);
    for(int ii = 0;ii<board_size*board_size;++ii)
    {
      board[ii] = store_board[ii];
      next_stone[ii] = store_next_stone[ii];
    }
    step = storeStep;
    ko_i = storeko_i;
    ko_j = storeko_j;
  }
  for(int ii = 0;ii<board_size*board_size;++ii)
  {
    board[ii] = store_board[ii];
    next_stone[ii] = store_next_stone[ii];
  }
  step = storeStep;
  ko_i = storeko_i;
  ko_j = storeko_j;
  if ( root->nextMove.size() > 0)
  {
    for (int ii = 0; ii < root->nextMove.size(); ++ii)
    {
      uctNode *tt = root->nextMove[ii];
      tt->score = (tt->playResult + 0.0) / tt->play + sqrtf(2 * log(games) / tt->play);
    }
    if (color==BLACK)
    {
      uctNode *tmpNode = root->nextMove[root->nextMove.size() - 1];
      //mylog("black:",tmpNode->playResult / tmpNode->play);
      if (tmpNode->playResult / tmpNode->play > b - w)
      {
        (*pos) = tmpNode->pos;
      }
      else
      {
        (*pos) = -1;
      }
    }
    else
    {
      uctNode *tmpNode = root->nextMove[0];
      mylog("white:",tmpNode->playResult / tmpNode->play);
      if (tmpNode->playResult / tmpNode->play < b - w)
      {
        (*pos) = tmpNode->pos;
      }
      else
      {
        (*pos) = -1;
      }
    }
  }
  else
  {
    (*pos) = -1;
  }
  delete root;
  delete []store_board;
  delete []store_next_stone;
  if ((*pos) == -1 || (*pos)==POS(ko_i,ko_j))
  {
    aiMoveGreedy2(pos, color, moves, num_moves);
  }
}



void mylog(const char str[])
{
  ofstream outfile1("loglog.txt",ios_base::app);
  outfile1<<str;
  outfile1<<"\r\n";
  outfile1.close();
}

void mylog(int num1, int num2)
{
  ofstream outfile1("lognum.txt",ios_base::app);
  outfile1<<num1<<" "<<num2;
  outfile1<<"\r\n";
  outfile1.close();
}

void mylog(int num)
{
  ofstream outfile1("loglog.txt",ios_base::app);
  outfile1<<num;
  outfile1<<"\r\n";
  outfile1.close();
}

void mylog(const char str[], int num)
{
  ofstream outfile1("loglog.txt",ios_base::app);
  outfile1<<str;
  outfile1<<num;
  outfile1<<"\r\n";
  outfile1.close();
}

void show_game()
{
  ofstream outfile1("log_game.txt", ios_base::app);
  outfile1<<"----------------------------\r\n";
  outfile1<<"board\r\n";
  outfile1<<"   ";
  for (int i=0; i<board_size; ++i)
  {
    outfile1<<i;
    if (i/10==0)
      outfile1<<"  ";
    else
      outfile1<<" ";
  }
  outfile1<<"\t     ";

  for (int i=0; i<board_size; ++i)
  {
    outfile1<<i;
    if (i/10==0)
      outfile1<<"     ";
    else
      outfile1<<"    ";
  }

  outfile1<<"\r\n";
  for (int i=0; i<board_size; ++i)
  {
    outfile1<<i;
    if (i/10==0)
      outfile1<<"  ";
    else
      outfile1<<" ";
    for (int j=0; j<board_size; ++j)
    {
      if (get_board(i,j)==BLACK)
        outfile1<<"B";
      else if (get_board(i,j)==WHITE)
        outfile1<<"W";
      else
        outfile1<<"O";
      outfile1<<"  ";
    }

    outfile1<<"\t";
    outfile1<<i;
    if (i/10==0)
      outfile1<<"  ";
    else
      outfile1<<" ";
    for (int j=0; j<board_size; ++j)
    {
      int t = next_stone[POS(i,j)];
      int ui = I(t);
      int uy = J(t);
      if (ui/10==0)
        outfile1<<" ";
      outfile1<<ui;
      outfile1<<",";
      outfile1<<uy;
      if (uy/10==0)
        outfile1<<"  ";
      else
        outfile1<<" ";

    }
    outfile1<<"\r\n";
  }
  outfile1<<"\r\n";
  outfile1.close();
}




















void aiMovePreCheck(int *pos, int color, int *moves, int num_moves)
{
	int ai, aj;
	for (int k = 0; k < 4; ++k)
	{
		ai = rivalMovei + deltai[k];
		aj = rivalMovej + deltaj[k];
		if (checkLiberty(ai, aj) == 1)
		{
			int ppos = findALiberty(ai, aj);
			if (available(I(ppos), J(ppos), color))
			{
				*pos = ppos;
				return;
			}				
		}
	}
	*pos = -1;
}

int findALiberty(int i, int j)
{
	if (!on_board(i, j))
		return -1;
	int color = get_board(i, j);
	if (color == EMPTY)
		return -1;
	int ai;
	int aj;
	int pos = POS(i, j);
	int pos1 = pos;
	do{
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
				return POS(bi, bj);
		}
		pos1 = next_stone[pos1];
	} while (pos1 != pos);
	return -1;
}

