#include "GoBoard.h"
#include <memory.h>
#include <stdlib.h>
#include "GoEngine.h"
#include <fstream>


using namespace std;

int GoBoard::board_size = 13;
int GoBoard::board_size2 = board_size*board_size;
float GoBoard::komi = 6.5;
int GoBoard::handicap = 0;
int GoBoard::final_status[MAX_BOARD2];
int GoBoard::deltai[4] = {-1, 1, 0, 0};
int GoBoard::deltaj[4] = {0, 0, -1, 1};
int GoBoard::diag_i[4] = { -1,1,-1,1 };
int GoBoard::diag_j[4] = { -1,-1,1,1 };
int GoBoard::around_i[8] = {-1,0,1,1,1,0,-1,-1};
int GoBoard::around_j[8] = {-1,-1,-1,0,1,1,1,0};
int GoBoard::pass_move(int i, int j)  { return i == -1 ; }
int GoBoard::POS(int i, int  j) { return ((i)* board_size + (j)); }
int GoBoard::I(int pos) { return ((pos) / board_size); }
int GoBoard::J(int pos) { return ((pos) % board_size); }


GoBoard::GoBoard()
{
	 ko_i = -1;
	 ko_j = -1;
	 for (int i = 0; i < 3; ++i)
		 last_atari[i] = -1;
	 step = 0;
	 stones_on_board[BLACK] = 0;
	 stones_on_board[WHITE] = 0;
	// last_atari[BLACK] = -1;
	 //last_atari[WHITE] = -1;
	 last_point = -1;
	 last_point2 = -1;

	for (int i = 0; i < board_size2; ++i)
	{
		board[i] = NULL;
		strings[i].clear();
	}
	for (int i = 0; i < board_size2; ++i)
	{
		empty_points[i] = i;
	}
	empty_points_number = board_size2;
}
GoBoard::~GoBoard()
{
	//for (int i = 0; i < board_size2; ++i)
		//delete board[i];
}
int GoBoard::board_empty()
{
	int i;
	for (i = 0; i < board_size2; i++)
		if (board[i])
			return 0;

	return 1;
}

int GoBoard::on_board(int i, int j) const
{
	return i >= 0 && i < board_size && j >= 0 && j <board_size;
}
int GoBoard::get_board(int i, int j)
{
	int pos = POS(i, j);
	if (!board[pos])
		return EMPTY;
	return board[pos]->get_color();
}

int GoBoard::checkLiberty(int i, int j)
{
	if (!on_board(i, j))
		return -1;
	if (get_board(i, j) == EMPTY)
		return -1;
	return board[POS(i, j)]->get_liberties_number();
}

void GoBoard::play_move(int i, int j, int color)
{
	if (!on_board(i, j) || get_board(i, j) != EMPTY)
	{
		//printf(" error in GoBoard::play_move\n%d %d\n",i,j);
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
		last_point2 = last_point;
		last_point = -1;
		return;
	}
	last_point2 = last_point;
	last_point = pos;
	/* If the move is a suicide we only need to remove the adjacent
	* friendly stones.
	*/
	if (suicide(i, j, color))
	{
		for (k = 0; k < 4; ++k)
		{
			int ai = i + deltai[k];
			int aj = j + deltaj[k];
			if (on_board(ai, aj) && get_board(ai, aj) == color)
				remove_string(ai, aj);
		}
		return;
	}

	/* Not suicide. Remove captured opponent strings. */
	for (k = 0; k < 4; ++k)
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
	int libs[4];
	int libs_number = 0;
	for (int m = 0; m < 4; ++m)
	{
		if (!on_board(i + deltai[m], j + deltaj[m]))
			continue;
		if (!board[POS(i + deltai[m], j + deltaj[m])])
		{
			libs[libs_number++] = POS(i + deltai[m], j + deltaj[m]);
			continue;
		}
		board[POS(i + deltai[m], j + deltaj[m])]->remove_liberty(pos);

	}

	strings[pos].set_up(pos, color, libs, libs_number);
	board[pos] = &strings[pos];
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
		if (on_board(ai, aj) && board[pos2] && board[pos2]->get_color() == color && !same_string(pos, pos2))
		{
			/* The strings are linked together simply by swapping the the
			* next_stone pointers.
			*/
			/*board[pos]->merge_string(board[pos2]);
			board[pos2]->clear();
			board[pos2] = board[pos];*/
			board[pos]->merge_string(board[pos2]);
			String *temp = board[pos2];
			for (int m = 0; m < board[pos2]->get_stones_number(); ++m)
			{
				board[board[pos2]->stones[m]] = board[pos];
			}
			
			temp->clear();

		}
	}

	/* If we have captured exactly one stone and the new string is a
	* single stone it may have been a ko capture.
	*/
	if (captured_stones == 1 && board[pos]->get_stones_number()==1 )
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
	if (board[POS(i, j)] && board[POS(i, j)]->get_liberties_number() == 1)
		last_atari[color] = POS(i, j);
	for (int m = 0; m < 4; ++m)
	{
		int bi = i + deltai[m];
		int bj = j + deltaj[m];
		if (!on_board(bi,bj ))
			continue;
		if (board[POS(bi, bj)] && board[POS(bi, bj)]->get_color() == OTHER_COLOR(color) && board[POS(bi, bj)]->get_liberties_number() == 1)
		{
			last_atari[OTHER_COLOR(color)] = POS(bi, bj);
			return;
		}

	}

}

bool GoBoard::is_virtual_eye(int point, int color)
{
	if (!is_surrounded(point, color)) return false;
	int nopponent = 0;
	int ai = I(point);
	int aj = J(point);
	bool at_edge = false;
	for (int i = 0; i < 4; i++) {
		int bi = ai + diag_i[i];
		int bj = aj + diag_j[i];
		if (!on_board(bi, bj))
		{
			at_edge = true;
			continue;
		}
		if (get_board(bi, bj) == OTHER_COLOR(color)) {
			nopponent++;
		}
	}
	if (at_edge)
		++nopponent;
	return nopponent < 2;
}

void GoBoard::show_board()
{
	for (int i = 0; i < GoBoard::board_size; ++i)
	{
		for (int j = 0; j < GoBoard::board_size; ++j)
		{
			if (board[POS(i, j)])
				cerr << board[POS(i, j)]->get_color() << " ";
			else
				cerr << EMPTY << " ";
		}
		cerr << "\n";
	}
	cerr<<"\n";
	int string_number = 0;
	for (int i = 0; i < GoBoard::board_size2; ++i)
	{
		if (strings[i].get_stones_number() || strings[i].get_liberties_number())
		{
			cerr << "string" << string_number++<<":";
			for (int j = 0; j < strings[i].get_stones_number(); ++j)
				cerr << strings[i].stones[j] << " ";
			cerr << "\n";
			cerr << "color:" << strings[i].get_color()<<"\n";
			cerr << "liberties:";
			for (int j = 0; j < strings[i].get_liberties_number(); ++j)
				cerr << strings[i].liberties[j]<<" ";
			cerr << "\n";
		}
	}
}
GoBoard * GoBoard::copy_board()
{
	GoBoard *temp = new GoBoard();
	for (int i = 0; i < 3; ++i)
		temp->last_atari[i] = last_atari[i];
	for (int i = 0; i < board_size2; ++i)
	{
		temp->strings[i] = strings[i];
		temp->board[i] = NULL;
	}
	for (int i = 0; i < board_size2; ++i)
	{
		for (int j = 0; j < strings[i].get_stones_number(); ++j)
		{
			temp->board[strings[i].stones[j]] = &temp->strings[i];
		}

	}
	temp->empty_points_number = empty_points_number;
	for (int i = 0; i < empty_points_number; ++i)
	{
		temp->empty_points[i] = empty_points[i];
	}
	temp->ko_i = ko_i;
	temp->ko_j = ko_j;
	temp->step = step;
	temp->last_point = last_point;
	temp->last_point2 = last_point2;
	temp->stones_on_board[BLACK] = stones_on_board[BLACK];
	temp->stones_on_board[WHITE] = stones_on_board[WHITE];
	return temp;
}

//int GoBoard::suicideLike(int i, int j, int color)
//{
//	int k;
//	int ans = 0;
//	for (k = 0; k < 4; ++k)
//	{
//		int ai = i + deltai[k];
//		int aj = j + deltaj[k];
//		if (on_board(ai, aj) && get_board(ai, aj) == EMPTY)
//			++ans;
//		else if (on_board(ai, aj) && get_board(ai, aj) == color)
//			ans += checkLiberty(ai, aj) - 1;
//	}
//	return ans == 1;
//}


int GoBoard::findALiberty(int i, int j)
{
	if (!on_board(i, j))
		return -1;
	int color = get_board(i, j);
	if (color == EMPTY)
		return -1;
	return board[POS(i, j)]->get_liberties_number();
}








void GoBoard::clear_board()
{
	for (int i = 0; i < board_size2; ++i)
	{
		board[i] = NULL;
		strings[i].clear();
	}
	ko_i = -1;
	ko_j = -1;
	step = 0;
	stones_on_board[BLACK] = 0;
	stones_on_board[WHITE] = 0;
	// last_atari[BLACK] = -1;
	//last_atari[WHITE] = -1;
	last_point = -1;
	last_point2 = -1;

	for (int i = 0; i < board_size2; ++i)
	{
		board[i] = NULL;
		strings[i].clear();
	}
	for (int i = 0; i < board_size2; ++i)
	{
		empty_points[i] = i;
	}
	empty_points_number = board_size2;
}


int GoBoard::get_string(int i, int j, int *stonei, int *stonej)//have problem
{
	int num_stones = 0;
	int pos = POS(i, j);
	return board[pos]->get_stones_number();
}








/* Does the string at (i, j) have any more liberty than the one at
* (libi, libj)?
*/
int GoBoard::has_additional_liberty(int i, int j, int libi, int libj)
{
	int pos = POS(i, j);
	//if (!board[pos])
	//{
	//	printf("error in has_additional_liberty\n");
	//	return 0;
	//}
	int lib = POS(libi, libj);
	return board[pos]->get_liberties_number() > 1;
	//for (int m = 0; m < board[pos]->get_liberties_number(); ++m)
	//{
	//	if (board[pos]->liberties[m] != lib)
	//		return 1;
	//}
	//return 0;
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
	{
		return has_additional_liberty(ai,aj,i,j);
	}
	/* An unfriendly string provides a liberty if and only if it is
	* captured, i.e. if it currently only has the liberty at (i, j).
	*/
	return !has_additional_liberty(ai,aj,i,j);
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
bool GoBoard::is_legal(int point, int color)
{
	int ai = I(point);
	int aj = J(point);
	String *strings[4];
	int strings_number = 0;
	if (ai == ko_i &&aj == ko_j) return false;
	for (int i = 0; i < 4; ++i)
	{
		int bi = ai + deltai[i];
		int bj = aj + deltaj[i];
		if (!on_board(bi, bj))
			continue;
		if (!board[POS(bi, bj)])
		{
			return 1;
		}
		else
		{
			strings_number += add_string(strings, strings_number, board[POS(bi, bj)]);
		}
	}


	for (int i = 0; i < strings_number; ++i)
	{
		if (strings[i]->get_color() == color &&!strings[i]->get_liberties_number() == 1)
			return 1;
	}
	return strings_in_atari(point, color, strings,strings_number)>0;
}
int GoBoard::strings_in_atari(int point, int color, String*strings[],int string_number)
{
	int atari_number = 0;
	for (int i = 0; i < string_number; ++i)
	{
		if (strings[i]->get_color() != color&&strings[i]->get_liberties_number() == 1)
			atari_number++;
	}
	return atari_number;
}

int GoBoard::remove_string(int i, int j)
{
	String * removed_string = board[POS(i, j)];
	int color = removed_string->get_color();
	for (int m = 0; m < removed_string->get_stones_number(); ++m)
	{
		board[removed_string->stones[m]] = NULL;
		stones_on_board[color]--;
		empty_points[empty_points_number++] = removed_string->stones[m];
		int removed_point_i = I(removed_string->stones[m]);
		int removed_point_j = J(removed_string->stones[m]);
		for (int n = 0; n < 4; ++n)
		{
			int bi = removed_point_i + deltai[n];
			int bj = removed_point_j + deltaj[n];
			if (on_board(bi, bj))
			if (on_board(bi, bj) && board[POS(bi, bj)])


			if (on_board(bi, bj) && board[POS(bi, bj)] && board[POS(bi, bj)]->get_color() == OTHER_COLOR(color))
				board[POS(bi, bj)]->add_liberty(POS(removed_point_i, removed_point_j));
		}
		
	}
	int removed = removed_string->get_stones_number();
	removed_string->clear();
	return removed;
}

/* Do two vertices belong to the same string. It is required that both
* pos1 and pos2 point to vertices with stones.
*/
int GoBoard::same_string(int pos1, int pos2)
{
	if (!board[pos1] || !board[pos2])
		return 0;
	if (board[pos1] == board[pos2])
		return 1;
	else return 0;
}


int GoBoard::gains_liberty(int move, String* s)
{
	int cur_liberties = 1;
	if (s) cur_liberties = s->get_liberties_number ();
	int nlibs = total_liberties(move, s->get_color(), 0, cur_liberties, s);
	return nlibs > cur_liberties;
}
int GoBoard::add_point(int *points, int points_number, int point)
{
	for (int i = 0; i < points_number; ++i)
	{
		if (points[i] == point)
		{
			return 0;
		}
	}
	points[points_number] =point;
	return 1;

}
int GoBoard::add_string(String * strings[], int strings_number, String* string)
{
	for (int i = 0; i < strings_number; ++i)
	{
		if (strings[i] == string)
			return 0;
	}
	strings[strings_number] = string;
	return 1;
}
int GoBoard::total_liberties(int point, int color, int *liberties, int enough, String *exclude)
{

	int libs[MAX_BOARD2];
	int libs_number=0;
	String* strings[4];
	int string_number = 0;
	for (int i = 0; i < 4; ++i)
	{
		int ai = I(point) + deltai[i];
		int aj = J(point) + deltaj[i];
		if (!on_board(ai, aj))
			continue;
		if (!board[POS(ai, aj)])
			libs[libs_number++] = POS(ai, aj);
		else
		{
			string_number += add_string(strings, string_number, board[POS(ai, aj)]);
		}
	}
	if (enough && libs_number>enough)  return libs_number;
	for (int i = 0; i < string_number; ++i)
	{
		String* cur_string = strings[i];
		if (cur_string != exclude) 
		{
			if (cur_string->get_color() == color)
			{
				for (int j = 0; j < cur_string->get_liberties_number(); ++j)
				{
					if (cur_string->liberties[j] != point)
					{
						libs_number+= add_point(libs, libs_number, cur_string->liberties[j]);
						if (enough && libs_number > enough) return libs_number;
					}
				}
			}
			else if (cur_string->get_liberties_number() == 1)
				{
					for (int j = 0; j < cur_string->get_stones_number(); ++j)
					{
						int bi = I(cur_string->stones[j]);
						int bj = J(cur_string->stones[j]);
						for (int k = 0; k < 4; ++k)
						{
							int ci = bi + deltai[k];
							int cj = bj + deltaj[k];
							if (!on_board(ci, cj))
								continue;
							if (POS(ci, cj) == point)
							{
								libs_number += add_point(libs, libs_number, cur_string->stones[j]);
								if (enough &&libs_number > enough) return libs_number;
							}
							else
								if (board[POS(ci, cj)] && board[POS(ci, cj)]->get_color() == color)
								{
									for (int m = 0; m < string_number; ++m)
									{
										if (board[POS(ci, cj)] == strings[m])
										{
											libs_number += add_point(libs, libs_number, POS(bi, bj));
											if (enough&&libs_number > enough) return libs_number;
										}

									}
								}


						}
					}
				}
		}
	}
	return libs_number;

}
//bool GoBoard::available(int i, int j, int color)
//
//{
//	//return (on_board(i,j)==1) && (legal_move(i,j,color)==1) && (suicide(i,j,color)==0);
//
//	if (!on_board(i, j) || get_board(i, j) != EMPTY)
//	{
//		return false;
//	}
//	if (legal_move(i, j, color) && !suicide(i, j, color))
//	{
//		if (!suicide(i, j, OTHER_COLOR(color)))
//		{
//			return true;
//		}
//		else
//		{
//			for (int k = 0; k<4; ++k)
//			{
//				int bi = i + deltai[k];
//				int bj = j + deltaj[k];
//				if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color))
//				{
//					return true;
//				}
//				if (on_board(bi, bj) && get_board(bi, bj) && get_board(bi, bj) == color && checkLiberty(bi, bj) == 1)
//				{
//					return true;
//				}
//			}
//		}
//	}
//	return false;
//}
bool GoBoard::available(int i, int j, int color)
{
	/* Consider moving at (ai, aj) if it is legal and not suicide. */
	if (!legal_move(i, j, color))
		return 0;
	for (int m = 0; m < 4; ++m)
	{
		int ai = i + deltai[m];
		int aj = j + deltaj[m];
		if (!on_board(ai, aj))
			continue;
		if (!board[POS(ai, aj)])
			return 1;
	}
	if (!suicide(i, j, color))
		return true;
	for (int m = 0; m < 4; ++m)
	{
		int ai = i + deltai[m];
		int aj = j + deltaj[m];
		if (!on_board(ai, aj))
			continue;
		if (board[POS(ai, aj)]->get_color() == OTHER_COLOR(color) && board[POS(ai, aj)]->get_liberties_number() == 1)
			return 1;
	}
	return 0;

	//if (legal_move(i, j, color) && !suicide(i, j, color))
	//{
	//	/* Further require the move not to be suicide for the opponent... */
	//	if (!suicide(i, j, OTHER_COLOR(color)))
	//		return 1;
	//	else
	//	{
	//		/* ...however, if the move captures at least one stone,
	//		* consider it anyway.
	//		*/
	//		for (int k = 0; k < 4; k++)
	//		{
	//			int bi = i + deltai[k];
	//			int bj = j +deltaj[k];
	//			if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color))
	//			{
	//				return 1;
	//			}
	//		}
	//	}
	//}
	//return 0;
}


bool GoBoard::is_true_eye(int point, int color)
{
	int i = 0, ncontrolled = 0;
	if (!is_surrounded(point, color)) return false;
	int ai = I(point);
	int aj = J(point);
	for (int m = 0; m < 4; ++m)
	{
		int bi = ai + diag_i[m];
		int bj = aj + diag_j[m];
		if (!on_board(bi, bj))
			continue;
		++i;
		if (board[POS(bi, bj)])
		{
			if (board[POS(bi, bj)]->get_color() == color)
				ncontrolled++;
		}
		else
		{
			if (is_surrounded(POS(bi, bj), color)) {
				ncontrolled++;
			}
		}
	}
	if (i == 4)
	{
		if (ncontrolled > 2)
			return true;
	}
	else if (ncontrolled == i)
		return true;
	return false;
}

void GoBoard::set_final_status_string(int pos, int status)
{
	for (int i = 0; i < board[pos]->get_stones_number(); ++i)
		final_status[board[pos]->stones[i]] = status;
}



int GoBoard::legal_move(int i, int j, int color)
{
	int other = OTHER_COLOR(color);

	/* Pass is always legal. */
	if (pass_move(i, j))
		return 1;

	/* Already occupied. */
	if (board[POS(i,j)])
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

////Start here

int GoBoard::generate_legal_moves(int* moves, int color)
{
	int num_moves = 0;
	int ai, aj;

	memset(moves, 0, sizeof(moves));
	for (ai = 0; ai < board_size; ai++)
	{
		for (aj = 0; aj < board_size; aj++)
		{
			if (available(ai, aj, color) && !is_true_eye(POS(ai, aj), color))
				moves[num_moves++] = POS(ai, aj);
			///* Consider moving at (ai, aj) if it is legal and not suicide. */
			//if (legal_move(ai, aj, color) && !suicide(ai, aj, color))
			//{
			//	/* Further require the move not to be suicide for the opponent... */
			//	if (!suicide(ai, aj, OTHER_COLOR(color)))
			//		moves[num_moves++] = POS(ai, aj);
			//	else
			//	{
			//		/* ...however, if the move captures at least one stone,
			//		* consider it anyway.
			//		*/
			//		for (k = 0; k < 4; k++)
			//		{
			//			int bi = ai + deltai[k];
			//			int bj = aj + deltaj[k];
			//			if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color))
			//			{
			//				moves[num_moves++] = POS(ai, aj);
			//				break;
			//			}

			//		}
			//	}
			//}
		}
	}
	return num_moves;
}


//int GoBoard::find_one_Liberty_for_atari2(int i, int j, bool*checked)
//{
//	int color = get_board(i, j);
//	if (color == EMPTY)
//		return -1;
//	int ai;
//	int aj;
//	int pos = POS(i, j);
//	int pos1 = pos;
//	int liberty_point = -1;
//	int ans = 0;
//	do {
//		if (checked[pos1])
//			return -1;
//		checked[pos1] = true;
//		if (ans > 1) return -1;
//		ai = I(pos1);
//		aj = J(pos1);
//		for (int k = 0; k < 4; ++k)
//		{
//			int bi = ai + deltai[k];
//			int bj = aj + deltaj[k];
//			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
//			{
//				++ans;
//				liberty_point = POS(bi, bj);
//			}
//		}
//		pos1 = next_stone[pos1];
//	} while (pos1 != pos);
//	if (ans == 1)
//		return liberty_point;
//	return -1;
//}
//int GoBoard::find_one_Liberty_for_atari(int i, int j)
//{
//	if (!on_board(i, j))
//		return -1;
//	int color = get_board(i, j);
//	if (color == EMPTY)
//		return -1;
//	int ai;
//	int aj;
//	int pos = POS(i, j);
//	int pos1 = pos;
//	int liberty_point = -1;
//	int ans = 0;
//	do {
//		if (ans > 1) return -1;
//		ai = I(pos1);
//		aj = J(pos1);
//		for (int k = 0; k < 4; ++k)
//		{
//			int bi = ai + deltai[k];
//			int bj = aj + deltaj[k];
//			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
//			{
//				++ans;
//				liberty_point = POS(bi, bj);
//			}
//		}
//		pos1 = next_stone[pos1];
//	} while (pos1 != pos);
//	if (ans == 1)
//		return liberty_point;
//	return -1;
//}

//int GoBoard::check_one_Liberty(int i, int j)
//{
//	if (!on_board(i, j))
//		return -1;
//	int color = get_board(i, j);
//	if (color == EMPTY)
//		return -1;
//	int ai;
//	int aj;
//	int pos = POS(i, j);
//	int pos1 = pos;
//	int ans = 0;
//	do {
//		if (ans > 1) return 2;
//		ai = I(pos1);
//		aj = J(pos1);
//		for (int k = 0; k < 4; ++k)
//		{
//			int bi = ai + deltai[k];
//			int bj = aj + deltaj[k];
//			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY)
//				++ans;
//		}
//		pos1 = next_stone[pos1];
//	} while (pos1 != pos);
//	return ans;
//}




int GoBoard::random_legal_move(int color)
{

	int pos = rand()*board_size2 / (RAND_MAX + 1);
	for (int i = pos; i < board_size2; ++i)
	{
		if (available(I(i), J(i), color) &&!is_virtual_eye(i,color ) &&!is_self_atari(i,color) )
			return i;
	}
	for (int i = 0; i < pos; ++i)
	{
		if (available(I(i), J(i), color) &&!is_virtual_eye(i,color)&&!is_self_atari(i,color)   )
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
	int move;



	move = last_atari_heuristic(color);					//try to find a move that will capture the opponent
	if (move != -1)
	{
		play_move(I(move), J(move), color);
		return move;
	}
	move = save_heuristic(color);					//try to find a move that will capture the opponent
	if (move != -1)
	{
		play_move(I(move), J(move), color);
		return move;
	}

	//move = mogo_pattern_heuristic(color);  // check whether the opponent's last move's around_eight_moves match a pattern, if match ,chose it.
	//if (move != -1)
	//{
	//	play_move(I(move), J(move), color);
	//	return move;
	//}
	move = capture_heuristic(color);					//try to find a move that will capture the opponent
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
	if (board[point])
		return false;
	int ai = I(point);
	int aj = J(point);
	for (int k = 0; k < 4; ++k) {
		int bi = ai + deltai[k];
		int bj = aj + deltaj[k];
		if (!on_board(bi, bj))
			continue;
		if (get_board(bi,bj) != color)
			return false;
	}
	return true;
}

double GoBoard::chinese_count()
{
	int black_score = 0, white_score = 0, eyes_result = 0;
	for (int i = 0; i < board_size2; i++) {
		if ( get_board(I(i),J(i)) == WHITE)
		{
			white_score++;
			continue;
		}
		if (get_board(I(i),J(i)) == BLACK)
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

int GoBoard::autoRun_fill_the_board(int color, int*simul_len, AmafBoard* tamaf)
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
				tamaf->play(move, ++(*simul_len));
				pass = 0;
			}
			else
			{
				pass++;
				tamaf->side = !tamaf->side;
			}
			move = select_and_play(OTHER_COLOR(color));

			if (move != -1)
			{
				tamaf->play(move, ++(*simul_len));

				pass = 0;
			}
			else
			{
				pass++;
				tamaf->side = !tamaf->side;
			}

			if (iterstep > 3*board_size*board_size)
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
				tamaf->play(move, ++(*simul_len));

				pass = 0;
			}
			else
			{
				pass++;
				tamaf->side = !tamaf->side;
			}
			move = select_and_play(OTHER_COLOR(color));

			if (move != -1)
			{
				tamaf->play(move, ++(*simul_len));

				pass = 0;
			}
			else
			{
				pass++;
				tamaf->side = !tamaf->side;
			}

			if (iterstep > 3*board_size*board_size)
				return -1;
		}
	}
	double count = chinese_count();
	//cout << *simul_len << "?" << endl;
	if (count > 0 && color == WHITE)
	{
		return 1;
	}
	if (count < 0 && color == BLACK)
		return 1;
	return 0;
}

int GoBoard::random_choose_move(int * moves, int number_moves,int color)
{
	int pos = rand()*number_moves / (RAND_MAX + 1);
	for (int i = pos; i < number_moves; ++i)
	{
		int move = moves[i];
		if (available(I(move), J(move), color) && !is_virtual_eye(move, color)&&!is_self_atari(move,color) )
			return move;
	}
	for (int i = 0; i < pos; ++i)
	{
		int move = moves[i];
		if (available(I(move), J(move), color) && !is_virtual_eye(move, color) &&!is_self_atari(move,color))
			return move ;
	}
	return -1;


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
