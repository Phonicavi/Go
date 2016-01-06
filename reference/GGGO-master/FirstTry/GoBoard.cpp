#include "GoBoard.h"
#include <memory.h>
#include <stdlib.h>
#include "GoEngine.h"
#include <fstream>
#include <iostream>
using namespace std;

int GoBoard::board_size = 13;
float GoBoard::komi = 6.5;
int GoBoard::final_status[MAX_BOARD * MAX_BOARD];
int GoBoard::deltai[4] = {-1, 1, 0, 0};
int GoBoard::deltaj[4] = {0, 0, -1, 1};
int GoBoard::diag_i[4] = { -1,1,-1,1 };
int GoBoard::diag_j[4] = { -1,-1,1,1 };
int GoBoard::around_i[8] = {-1,0,1,1,1,0,-1,-1};
int GoBoard::around_j[8] = {-1,-1,-1,0,1,1,1,0};
int GoBoard::pass_move(int i, int j) { return i == -1 && j == -1; }
int GoBoard::POS(int i, int  j) { return ((i)* board_size + (j)); }
int GoBoard::I(int pos) { return ((pos) / board_size); }
int GoBoard::J(int pos) { return ((pos) % board_size); }
int GoBoard::suicideLike(int i, int j, int color)
{
	int k;
	int ans = 0;
	for (k = 0; k < 4; ++k)
	{
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (on_board(ai, aj) && get_board(ai, aj) == EMPTY)
			++ans;
		else if (on_board(ai, aj) && get_board(ai, aj) == color)
			ans += checkLiberty(ai, aj) - 1;
	}
	return ans == 1;
}


int GoBoard::findALiberty(int i, int j)
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

GoBoard::~GoBoard()
{
	delete[]board;
	delete[]next_stone;
}

GoBoard::GoBoard()
{
	rival_move_i = -1;
	rival_move_j = -1;
	my_last_move_i = -1;
	my_last_move_j = -1;
	int ko_i = -1;
	int ko_j = -1;
	int step = 0;
	int handicap = 0;
	board = new int[board_size*board_size];
	next_stone = new int[board_size*board_size];
	for (int i = 0; i < board_size*board_size; ++i)
	{
		board[i] = EMPTY;
		next_stone[i] = i;
	}
}

GoBoard * GoBoard::copy_board()
{
	GoBoard *temp = new GoBoard();
	for (int i = 0; i < board_size*board_size; ++i)
	{
		temp->board[i] = board[i];
		temp->next_stone[i] = next_stone[i];
	}
	temp->rival_move_i = rival_move_i;
	temp->rival_move_j = rival_move_j;
	temp->my_last_move_i = my_last_move_i;
	temp->my_last_move_j = my_last_move_j;
	temp->ko_i = ko_i;
	temp->ko_j = ko_j;
	temp->step = step;

	temp->handicap = handicap;
	return temp;
}

int GoBoard::board_empty()
{
	int i;
	for (i = 0; i < board_size*board_size; i++)
		if (board[i] != EMPTY)
			return 0;

	return 1;
}

void GoBoard::clear_board()
{
	memset(board, 0, sizeof(board));

}


int GoBoard::get_string(int i, int j, int *stonei, int *stonej)
{
	int num_stones = 0;
	int pos = POS(i, j);
	do {
		stonei[num_stones] = I(pos);
		stonej[num_stones] = J(pos);
		num_stones++;
		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return num_stones;
}


int GoBoard::on_board(int i, int j)
{
	return i >= 0 && i < board_size && j >= 0 && j <board_size;
}

int GoBoard::get_board( int i, int j)
{
	return board[i *board_size + j];
}



/* Does the string at (i, j) have any more liberty than the one at
* (libi, libj)?
*/
int GoBoard::has_additional_liberty(int i, int j, int libi, int libj)
{
	int pos = POS(i, j);
	do {
		int ai = I(pos);
		int aj = J(pos);
		int k;
		for (k = 0; k < 4; k++) {
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY
				&& (bi != libi || bj != libj))
				return 1;
		}

		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return 0;
}

/* Does (ai, aj) provide a liberty for a stone at (i, j)? */
int GoBoard::provides_liberty(int ai, int aj, int i, int j, int color)
{
	/* A vertex off the board does not provide a liberty. */
	if (!on_board(ai, aj))
		return 0;

	/* An empty vertex IS a liberty. */
	if (get_board(ai, aj) == EMPTY)
		return 1;

	/* A friendly string provides a liberty to (i, j) if it currently
	* has more liberties than the one at (i, j).
	*/
	if (get_board(ai, aj) == color)
		return has_additional_liberty(ai, aj, i, j);

	/* An unfriendly string provides a liberty if and only if it is
	* captured, i.e. if it currently only has the liberty at (i, j).
	*/
	return !has_additional_liberty(ai, aj, i, j);
}

int GoBoard::suicide(int i, int j, int color)
{
	int k;
	for (k = 0; k < 4; k++)
	{
		if (provides_liberty(i + deltai[k], j + deltaj[k], i, j, color))
			return 0;
	}


	return 1;
}


/* Remove a string from the board array. There is no need to modify
* the next_stone array since this only matters where there are
* stones present and the entire string is removed.
*/
int GoBoard::remove_string(int i, int j)
{
	int pos = POS(i, j);
	int removed = 0;
	do {
		board[pos] = EMPTY;
		removed++;
		pos = next_stone[pos];
	} while (pos != POS(i, j));
	return removed;
}

/* Do two vertices belong to the same string. It is required that both
* pos1 and pos2 point to vertices with stones.
*/
int GoBoard::same_string(int pos1, int pos2)
{
	int pos = pos1;
	do {
		if (pos == pos2)
			return 1;
		pos = next_stone[pos];
	} while (pos != pos1);

	return 0;
}

void GoBoard::play_move( int i, int j, int color)
{
	if (!on_board(i, j) || get_board( i, j) != EMPTY)
	{
		//mylog("play move error");
		return;
	}
	int pos = POS(i, j);
	int captured_stones = 0;
	int k;

	/* Reset the ko point. */
	ko_i = -1;
	ko_j = -1;
	++step;
	/* Nothing more happens if the move was a pass. */
	if (pass_move(i, j))
	{
		my_last_move_i = rival_move_i;
		my_last_move_j = rival_move_j;
		rival_move_i = -1;
		rival_move_j = -1;

		return;
	}
	my_last_move_i = rival_move_i;
	my_last_move_j = rival_move_j;
	rival_move_i = i;
	rival_move_j = j;

	/* If the move is a suicide we only need to remove the adjacent
	* friendly stones.
	*/
	if (suicide(i, j, color))
	{
		for (k = 0; k < 4; k++)
		{
			int ai = i + deltai[k];
			int aj = j + deltaj[k];
			if (on_board(ai, aj) && get_board(ai, aj) == color)
				 remove_string(ai, aj);
		}
		return;
	}

	/* Not suicide. Remove captured opponent strings. */
	for (k = 0; k < 4; k++)
	{
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (on_board(ai, aj) && get_board(ai, aj) == OTHER_COLOR(color) && !has_additional_liberty(ai, aj, i, j))
		{
			int removed = remove_string(ai, aj);
			captured_stones += removed;
		}
	}

	/* Put down the new stone. Initially build a single stone string by
	* setting next_stone[pos] pointing to itself.
	*/
	board[pos] = color;
	next_stone[pos] = pos;
	/* If we have friendly neighbor strings we need to link the strings
	* together.
	*/
	for (k = 0; k < 4; k++)
	{
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		int pos2 = POS(ai, aj);
		/* Make sure that the stones are not already linked together. This
		* may happen if the same string neighbors the new stone in more
		* than one direction.
		*/
		if (on_board(ai, aj) && board[pos2] == color && !same_string(pos, pos2))
		{
			/* The strings are linked together simply by swapping the the
			* next_stone pointers.
			*/
			int tmp = next_stone[pos2];
			next_stone[pos2] = next_stone[pos];
			next_stone[pos] = tmp;
		}
	}

	/* If we have captured exactly one stone and the new string is a
	* single stone it may have been a ko capture.
	*/
	if (captured_stones == 1 && next_stone[pos] == pos)
	{
		int ai, aj;
		/* Check whether the new string has exactly one liberty. If so it
		* would be an illegal ko capture to play there immediately. We
		* know that there must be a liberty immediately adjacent to the
		* new stone since we captured one stone.
		*/
		for (k = 0; k < 4; k++) {
			ai = i + deltai[k];
			aj = j + deltaj[k];
			if (on_board(ai, aj) && get_board(ai, aj) == EMPTY)
			{
				break;
			}
		}

		if (!has_additional_liberty(i, j, ai, aj)) {
			ko_i = ai;
			ko_j = aj;
		}
	}
}


int GoBoard::legal_move(int i, int j, int color)
{
	int other = OTHER_COLOR(color);

	/* Pass is always legal. */
	if (pass_move(i, j))
		return 1;

	/* Already occupied. */
	if (get_board(i, j) != EMPTY)
	{
		return 0;
	}

	/* Illegal ko recapture. It is not illegal to fill the ko so we must
	* check the color of at least one neighbor.
	*/
	if (i == ko_i && j == ko_j
		&& ((on_board(i - 1, j) && get_board(i - 1, j) == other)
			|| (on_board(i + 1, j) && get_board(i + 1, j) == other)))
		return 0;

	return 1;
}

//Start here
int GoBoard::generate_legal_moves(int* moves, int color)
{
	int num_moves = 0;
	int ai, aj;
	int k;

	memset(moves, 0, sizeof(moves));
	for (ai = 0; ai < board_size; ai++)
	{
		for (aj = 0; aj < board_size; aj++)
		{
			/* Consider moving at (ai, aj) if it is legal and not suicide. */
			if (legal_move(ai, aj, color) && !suicide(ai, aj, color))
			{
				/* Further require the move not to be suicide for the opponent... */
				if (!suicide(ai, aj, OTHER_COLOR(color)))
					moves[num_moves++] = POS(ai, aj);
				else
				{
					/* ...however, if the move captures at least one stone,
					* consider it anyway.
					*/
					for (k = 0; k < 4; k++)
					{
						int bi = ai + deltai[k];
						int bj = aj + deltaj[k];
						if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color))
						{
							moves[num_moves++] = POS(ai, aj);
							break;
						}
						//if (get_board(bi, bj) && get_board(bi, bj) == color && checkLiberty(bi, bj) == 1)
						//{
						//	moves[num_moves++] = POS(ai, aj);
						//	break;
						//}
					}
				}
			}
		}
	}
	return num_moves;
}

int GoBoard::checkLiberty(int i, int j)
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
	int ans = 0;
	do {
		if (ans > 2) return 3;
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
				++ans;
		}
		pos1 = next_stone[pos1];
	} while (pos1 != pos);
	return ans;
}

int GoBoard::find_one_Liberty_for_atari2(int i, int j, bool*checked)
{
	int color = get_board(i, j);
	if (color == EMPTY)
		return -1;
	int ai;
	int aj;
	int pos = POS(i, j);
	int pos1 = pos;
	int liberty_point = -1;
	int ans = 0;
	do {
		if (checked[pos1])
			return -1;
		checked[pos1] = true;
		if (ans > 1) return -1;
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
			{
				++ans;
				liberty_point = POS(bi, bj);
			}
		}
		pos1 = next_stone[pos1];
	} while (pos1 != pos);
	if (ans == 1)
		return liberty_point;
	return -1;
}
int GoBoard::find_one_Liberty_for_atari(int i, int j)
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
	int liberty_point = -1;
	int ans = 0;
	do {
		if (ans > 1) return -1;
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
			{
				++ans;
				liberty_point = POS(bi, bj);
			}
		}
		pos1 = next_stone[pos1];
	} while (pos1 != pos);
	if (ans == 1)
		return liberty_point;
	return -1;
}

int GoBoard::check_one_Liberty(int i, int j)
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
	int ans = 0;
	do {
		if (ans > 1) return 2;
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
				++ans;
		}
		pos1 = next_stone[pos1];
	} while (pos1 != pos);
	return ans;
}




bool GoBoard::available(int i, int j, int color)
{
	//return (on_board(i,j)==1) && (legal_move(i,j,color)==1) && (suicide(i,j,color)==0);
	if (!on_board(i, j) || get_board(i, j) != EMPTY) return false;
	if (legal_move(i, j, color) && !suicide(i, j, color))
	{
		if (!suicide(i, j, OTHER_COLOR(color)))
			return true;
		else
		{
			for (int k = 0; k<4; ++k)
			{
				int bi = i + deltai[k];
				int bj = j + deltaj[k];
				if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color)) {
					return true;
				}
				//if (get_board(bi, bj) && get_board(bi, bj) == color && check_one_Liberty(bi, bj) == 1)
				//{
				//	return true;
				//}
			}
		}
	}
	return false;
}

int minmin(int a, int b) { return (a < b) ? a : b; }
int maxmax(int a, int b) { return (a>b) ? a : b; }



int calcDisImpact(int d)
{
	int t = IMPACT;
	for (int i = 0; i < d; ++i)
	{
		t >>= 1;
	}
	return t;
}

int checkDistance(int x0, int y0, int x1, int y1)
{
	return abs(x0 - x1) + abs(y0 - y1);
}
void GoBoard::calcGame(int *b, int *w, int *bScore, int *wScore)
{
	int **extra = new int*[board_size];  // SIZE to board size
	for (int i = 0; i < board_size; ++i)
	{
		extra[i] = new int[board_size];
		for (int j = 0; j < board_size; ++j) extra[i][j] = 0;//i-x,j-y
	}
	for (int d = 0; d < board_size*board_size; ++d)
	{
		if (board[d] == EMPTY) continue; //black and white changed into board[d]
		int x = I(d);    //d%SIZE into  J(d)
		int y = J(d);   //D /SIZE into I(d)
		for (int i = maxmax(x - IMPACTDIS, 0); i <= minmin(x + IMPACTDIS, board_size - 1); ++i)
		{
			for (int j = maxmax(y - IMPACTDIS, 0); j <= minmin(y + IMPACTDIS, board_size - 1); ++j)
			{
				if (abs(x - i) + abs(y - j)>IMPACTDIS) continue;
				int dis = checkDistance(x, y, i, j);
				if (dis == -1 || dis>IMPACTDIS) continue;
				int ppos = POS(i, j);
				if (board[d] == BLACK)    //black[d] -> board
				{
					if (board[ppos] == BLACK  && ppos != d) extra[j][i] += calcDisImpact(dis) / SAMECOLOR;
					else if (board[ppos] == WHITE && ppos != d) extra[j][i] += calcDisImpact(dis) / DIFFERENTCOLOR;
					else extra[j][i] += calcDisImpact(dis);
				}
				else
				{
					if (board[ppos] == WHITE && ppos != d) extra[j][i] -= calcDisImpact(dis) / SAMECOLOR;
					else if (board[ppos] == BLACK && ppos != d) extra[j][i] -= calcDisImpact(dis) / DIFFERENTCOLOR;
					else extra[j][i] -= calcDisImpact(dis);
				}
			}
		}
	}

	for (int i = 0; i < board_size; ++i)
	{
		for (int j = 0; j < board_size; ++j)
		{
			if (extra[i][j]>BLACKEDGE)
			{
				++(*b);
				*bScore += extra[i][j];
			}
			else if (extra[i][j] < WHITEEDGE)
			{
				++(*w);
				*wScore += extra[i][j];
			}
		}
		delete[]extra[i];
	}
	delete[]extra;
	//printf("%d %d\n", bScore, wScore);
}

void GoBoard::show_game()
{
	ofstream outfile1("log3.txt", ios_base::app);
	outfile1 << "----------------------------\r\n";
	outfile1 << "board\r\n";
	outfile1 << "   ";
	for (int i = 0; i<board_size; ++i)
	{
		outfile1 << i;
		if (i / 10 == 0)
			outfile1 << "  ";
		else
			outfile1 << " ";
	}
	outfile1 << "\t     ";

	for (int i = 0; i<board_size; ++i)
	{
		outfile1 << i;
		if (i / 10 == 0)
			outfile1 << "     ";
		else
			outfile1 << "    ";
	}

	outfile1 << "\r\n";
	for (int i = 0; i<board_size; ++i)
	{
		outfile1 << i;
		if (i / 10 == 0)
			outfile1 << "  ";
		else
			outfile1 << " ";
		for (int j = 0; j<board_size; ++j)
		{
			if (get_board(i, j) == BLACK)
				outfile1 << "B";
			else if (get_board(i, j) == WHITE)
				outfile1 << "W";
			else
				outfile1 << "O";
			outfile1 << "  ";
		}

		outfile1 << "\t";
		outfile1 << i;
		if (i / 10 == 0)
			outfile1 << "  ";
		else
			outfile1 << " ";
		for (int j = 0; j<board_size; ++j)
		{
			int t = next_stone[POS(i, j)];
			int ui = I(t);
			int uy = J(t);
			if (ui / 10 == 0)
				outfile1 << " ";
			outfile1 << ui;
			outfile1 << ",";
			outfile1 << uy;
			if (uy / 10 == 0)
				outfile1 << "  ";
			else
				outfile1 << " ";

		}
		outfile1 << "\r\n";
	}
	outfile1 << "\r\n";
	outfile1.close();
}

//int GoBoard::autoRun(int color, bool* blackExist, bool* whiteExist)
//{
//	if (color != BLACK && color != WHITE) return 0;
//	bool passBlack = false;
//	bool passWhite = false;
//	int iterstep = 0;
//	int maxStep = MAXSTEP - step > 10 ? MAXSTEP - step : 10;
//
//	if (color == BLACK)
//	{
//		while ((!passBlack || !passWhite) && iterstep < maxStep)
//		{
//			++iterstep;
//			bool flagBlack = true;
//			bool flagWhite = true;
//			for (int i = 0; i < TRYTIME; ++i)
//			{
//				int ppos = rand() % (board_size*board_size);
//				flagBlack = available(I(ppos), J(ppos), BLACK);
//				if (flagBlack)
//				{
//					play_move(I(ppos), J(ppos), BLACK);			
//					blackExist[ppos] = 1;			
//					break;
//				}
//			}
//			passBlack = !flagBlack;
//			for (int i = 0; i < TRYTIME; ++i)
//			{
//				int ppos = rand() % (board_size*board_size);
//				flagWhite = available(I(ppos), J(ppos), WHITE);
//				if (flagWhite)
//				{
//					play_move(I(ppos), J(ppos), WHITE);
//					whiteExist[ppos] = 1;
//					break;
//				}
//			}
//			passWhite = !flagWhite;
//		}
//	}
//	else
//	{
//		while ((!passBlack || !passWhite) && iterstep < maxStep)
//		{
//			++iterstep;
//			bool flagBlack = false;
//			bool flagWhite = false;
//			for (int i = 0; i < TRYTIME; ++i)
//			{
//				int ppos = rand() % (board_size*board_size);
//				flagWhite = available(I(ppos), J(ppos), WHITE);
//				if (flagWhite)
//				{
//					play_move(I(ppos), J(ppos), WHITE);
//					whiteExist[ppos] = 1;
//					break;
//				}
//			}
//			passWhite = !flagWhite;
//			for (int i = 0; i < TRYTIME; ++i)
//			{
//				int ppos = rand() % (board_size*board_size);
//				flagBlack = available(I(ppos), J(ppos), BLACK);
//				if (flagBlack)
//				{
//					play_move(I(ppos), J(ppos), BLACK);
//					blackExist[ppos] = 1;
//					break;
//				}
//			}
//			passBlack = !flagBlack;
//		}
//	}
//	int bScore = 0;
//	int wScore = 0;
//	int b = 0;
//	int w = 0;
//	calcGame(&b, &w, &bScore, &wScore);
//	//only +
//	if (b - w > 0 && color == WHITE)
//		return 1;
//	if (b - w < 0 && color == BLACK)
//		return 1;
//	return 0;
//	//+1 -1
//	//if (b - w > 0)
//	//	return 1;
//	//else if (b - w < 0)
//	//	return -1;
//	//return 0;
//	// origin
//	//return b - w;
//}


int GoBoard::random_legal_move(int color)
{

	int pos = rand()*board_size*board_size / (RAND_MAX + 1);

	for (int i = pos; i < board_size*board_size; ++i)
	{
		if (available(I(i), J(i), color))
			return i;
	}
	for (int i = 0; i < pos; ++i)
	{
		if (available(I(i), J(i), color))
			return i;
	}
	return -1;
	//for (int i = 0; i < 95; ++i)
	//{
	//	int pos = rand()*board_size*board_size / (RAND_MAX + 1);
	//	if (available(I(pos), J(pos), color))
	//		return pos;
	//}
	////unsigned long long middle = GetCycleCount() ;
	////unsigned long long middle2 = GetCycleCount();
	//
	////nsigned long long middle3 = GetCycleCount();
	//int  reasonable_moves[169];
	//
	//int num = 0;
	//for (int i = 0; i < board_size*board_size; ++i)
	//{
	//	if (available(I(i), J(i), color))
	//		reasonable_moves[num++] = i;

	//}

	////unsigned long long middle4 = GetCycleCount();
	////printf("%llu\n%llu\n%llu\n%llu\n%llu\n", start,middle, middle2, middle3, middle4);


	////int num = generate_legal_moves(reasonable_moves, color);

	//if (num == 0)
	//{
	//	//delete reasonable_moves;
	//	return -1;
	//}
	//int move = reasonable_moves[rand()*num / (RAND_MAX + 1)];
	////delete reasonable_moves;


	//return move;
}


int GoBoard::select_and_play(int color)
{

	int save_atari_plays[MAX_BOARD*MAX_BOARD];
	int save_atari_number = save_atari(POS(rival_move_i, rival_move_j), save_atari_plays);
	if (save_atari_number>=0)
	{
		int move = save_atari_plays[rand()*save_atari_number / (RAND_MAX + 1)];
		play_move(I(move), J(move), color);
		return move;
	}
	//move = last_atari_heuristic(color);   //If the rival's last move is an atari, then try to find away to move out.(any point provide more liberty)
	//if (move != -1 )
	//{
	//	play_move(I(move), J(move), color);
	//	return move;
	//}
	int move;
	/*move = nakade_heuristic();		//not consider it at present
	if (move != -1)
	{
	play_move(I(move), J(move), color);
	}*/

	/*int move = fill_the_board_heuristic();  // randomly select a move, if the move is empty and 8 around moves are all empty, then chose it.
	if (move != -1)
	{
	play_move(I(move), J(move), color);
	return move;
	}*/
	//move = mogo_pattern_heuristic(color);  // check whether the opponent's last move's around_eight_moves match a pattern, if match ,chose it.
	//if (move != -1 )
	//{
	//	play_move(I(move), J(move), color);
	//	return move;
	//}
	move = capture_heuristic( color);					//try to find a move that will capture the opponent
	if (move != -1 )
	{
		play_move(I(move), J(move), color);
		return move;
	}
	move = random_legal_move(color);			//select a random  legal move
	if (move != -1)
	{
		play_move(I(move), J(move), color);
		return move;
	}

	return -1;
}


bool GoBoard::is_surrounded(int point, int color)
{
	if (board[point] != EMPTY)
		return false;
	int ai = I(point);
	int aj = J(point);
	for (int k = 0; k < 4; ++k) {
		int bi = ai + deltai[k];
		int bj = aj + deltaj[k];
		if (!on_board(bi, bj))
			continue;
		if (board[POS(bi, bj)] != color)
			return false;
	}
	return true;
}

double GoBoard::chinese_count()
{
	int black_score = 0, white_score = 0, eyes_result = 0;
	for (int i = 0; i < board_size*board_size; i++) {
		if (board[i] == WHITE)
		{
			white_score++;
			continue;
		}
		if (board[i] == BLACK)
		{
			black_score++;
			continue;
		}
		if (is_surrounded(i, BLACK))
			eyes_result++;
		if (is_surrounded(i, WHITE))
			eyes_result--;

	}
	return eyes_result + black_score - white_score - komi;
}

int GoBoard::autoRun_fill_the_board(int color,bool* blackExist, bool* whiteExist)
{
	if (color != BLACK && color != WHITE) return -1;
	int pass = 0;
	int iterstep = step;
	if (color == BLACK)
	{
		while (pass < 2)
		{
			++iterstep;
			int move = select_and_play(color);
			if (move != -1)
			{
				blackExist[move] = 1;
				pass = 0;
			}
			else pass++;
			move = select_and_play(OTHER_COLOR(color));

			if (move != -1)
			{
				whiteExist[move] = 1;
				pass = 0;
			}
			else pass++;

			if (iterstep > board_size*board_size)
				return -1;

		}
	}
	if (color == WHITE)
	{
		while (pass < 2)
		{
			++iterstep;
			int move = select_and_play(color);

			if (move != -1)
			{
				whiteExist[move] = 1;
				pass = 0;
			}
			else pass++;
			move = select_and_play(OTHER_COLOR(color));

			if (move != -1)
			{
				blackExist[move] = 1;
				pass = 0;
			}
			else pass++;
			if (iterstep > board_size*board_size)
				return -1;
		}
	}
	double count = chinese_count();

	if (count > 0 && color == WHITE)
	{
		return 1;
	}
	if (count < 0 && color == BLACK)
		return 1;
	return 0;
}



void GoBoard::set_final_status_string(int pos, int status)
{
	int pos2 = pos;
	do
	{
		final_status[pos2] = status;
		pos2 = next_stone[pos2];
	} while (pos2 != pos);
}

/* Compute final status. This function is only valid to call in a
* position where generate_move() would return pass for at least one
* color.
*
* Due to the nature of the move generation algorithm, the final
* status of stones can be determined by a very simple algorithm:
*
* 1. Stones with two or more liberties are alive with territory.
* 2. Stones in atari are dead.
*
* Moreover alive stones are unconditionally alive even if the
* opponent is allowed an arbitrary number of consecutive moves.
* Similarly dead stones cannot be brought alive even by an arbitrary
* number of consecutive moves.
*
* Seki is not an option. The move generation algorithm would never
* leave a seki on the board.
*
* Comment: This algorithm doesn't work properly if the game ends with
*          an unfilled ko. If three passes are required for game end,
*          that will not happen.
*/
void GoBoard::compute_final_status(void)
{
	int i, j;
	int pos;
	int k;

	for (pos = 0; pos < board_size * board_size; pos++)
		final_status[pos] = UNKNOWN;

	for (i = 0; i < board_size; i++)
		for (j = 0; j < board_size; j++)
			if (get_board(i, j) == EMPTY)
				for (k = 0; k < 4; k++) {
					int ai = i + deltai[k];
					int aj = j + deltaj[k];
					if (!on_board(ai, aj))
						continue;
					/* When the game is finished, we know for sure that (ai, aj)
					* contains a stone. The move generation algorithm would
					* never leave two adjacent empty vertices. Check the number
					* of liberties to decide its status, unless it's known
					* already.
					*
					* If we should be called in a non-final position, just make
					* sure we don't call set_final_status_string() on an empty
					* vertex.
					*/
					pos = POS(ai, aj);
					if (final_status[pos] == UNKNOWN) {
						if (get_board(ai, aj) != EMPTY) {
							if (has_additional_liberty(ai, aj, i, j))
								set_final_status_string(pos, ALIVE);
							else
								set_final_status_string(pos, DEAD);
						}
					}
					/* Set the final status of the (i, j) vertex to either black
					* or white territory.
					*/
					if (final_status[POS(i, j)] == UNKNOWN) {
						if ((final_status[pos] == ALIVE) ^ (get_board(ai, aj) == WHITE))
							final_status[POS(i, j)] = BLACK_TERRITORY;
						else
							final_status[POS(i, j)] = WHITE_TERRITORY;
					}
				}
}

int GoBoard::get_final_status(int i, int j)
{
	return final_status[POS(i, j)];
}

void GoBoard::set_final_status(int i, int j, int status)
{
	final_status[POS(i, j)] = status;
}

/* Valid number of stones for fixed placement handicaps. These are
* compatible with the GTP fixed handicap placement rules.
*/
int GoBoard::valid_fixed_handicap(int handicap)
{
	if (handicap < 2 || handicap > 9)
		return 0;
	if (board_size % 2 == 0 && handicap > 4)
		return 0;
	if (board_size == 7 && handicap > 4)
		return 0;
	if (board_size < 7 && handicap > 0)
		return 0;

	return 1;
}

/* Put fixed placement handicap stones on the board. The placement is
* compatible with the GTP fixed handicap placement rules.
*/
void GoBoard::place_fixed_handicap(int handicap)
{
	int low = board_size >= 13 ? 3 : 2;
	int mid = board_size / 2;
	int high = board_size - 1 - low;

	if (handicap >= 2) {
		play_move(high, low, BLACK);   /* bottom left corner */
		play_move(low, high, BLACK);   /* top right corner */
	}

	if (handicap >= 3)
		play_move(low, low, BLACK);    /* top left corner */

	if (handicap >= 4)
		play_move(high, high, BLACK);  /* bottom right corner */

	if (handicap >= 5 && handicap % 2 == 1)
		play_move(mid, mid, BLACK);    /* tengen */

	if (handicap >= 6) {
		play_move(mid, low, BLACK);    /* left edge */
		play_move(mid, high, BLACK);   /* right edge */
	}

	if (handicap >= 8) {
		play_move(low, mid, BLACK);    /* top edge */
		play_move(high, mid, BLACK);   /* bottom edge */
	}
}