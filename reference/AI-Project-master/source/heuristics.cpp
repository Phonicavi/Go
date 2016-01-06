#include "GoBoard.h"
#include <random>
//void GoBoard::try_to_save_by_eat(int i, int j, int *saves, int &saves_number)// find the group's adjacent groups and check whether we can eat it
//{
//	int color = get_board(i, j);
//	int ai;
//	int aj;
//	int pos = POS(i, j);
//	int pos1 = pos;
//	bool checked[MAX_BOARD*MAX_BOARD];
//	for (int i = 0; i < board_size*board_size; ++i)
//	{
//		checked[i] = false;
//	}
//	do {
//		ai = I(pos1);
//		aj = J(pos1);
//		for (int k = 0; k < 4; ++k)
//		{
//			int bi = ai + deltai[k];
//			int bj = aj + deltaj[k];
//			if (!on_board(bi, bj))
//				continue;
//			if (get_board(bi, bj) == OTHER_COLOR(color))
//			{
//				int move = find_one_Liberty_for_atari2(bi, bj, checked);
//				if (move == -1)
//					continue;
//				if (available(I(move), J(move), color))
//				{
//					saves[saves_number++] = move;
//				}
//			}
//
//		}
//		pos1 = next_stone[pos1];
//
//	} while (pos1 != pos);
//
//}
//
//int GoBoard::gains_liberty(int move,int color)
//{
//	int libs = 0;
//	for (int j = 0; j < 4 && libs<2; ++j)
//	{
//		int ne_lib_i = I(move) + deltai[j];				//check the liberty's near by provides more liberty 
//		int ne_lib_j = J(move) + deltaj[j];
//		if (!on_board(ne_lib_i, ne_lib_j))
//			continue;
//		if (get_board(ne_lib_i, ne_lib_j) == OTHER_COLOR(color))
//			continue;
//		if (get_board(ne_lib_i, ne_lib_j) == EMPTY)
//		{
//			libs++;
//			continue;
//		}
//		if (get_board(ne_lib_i, ne_lib_j) == color)
//		{
//			libs += checkLiberty(ne_lib_i, ne_lib_j) - 1;
//		}
//	}
//	if (libs > 1)
//		return true;
//	return false;
//}
//
//int GoBoard::last_atari_heuristic( int color)
//{
//	if (last_point) == -1)
//		return -1;
//
//	int saves[169];
//	int saves_number = 0;
//	for (int i = 0; i < 4; ++i) {									//check whether there is a atari
//		int neighbor_i = I(last_point) + deltai[i];
//		int neighbor_j = J(last_point) + deltaj[i];
//		if (!on_board(neighbor_i, neighbor_j))
//			continue;
//		if (get_board(neighbor_i, neighbor_j) == color)
//		{
//
//			int move = find_one_Liberty_for_atari(neighbor_i, neighbor_j); // only one liberty means in atari
//			if (move == -1)
//				continue;
//			try_to_save_by_eat(neighbor_i, neighbor_j, saves, saves_number);
//			/*try to save by  escape, that is to check the "move" provides more liberty*/
//			if (available (I(move),J(move),color)&& gains_liberty(move, color) )
//				saves[saves_number++] = move;
//		}
//
//	}
//	if (saves_number)
//		return  saves[rand()*saves_number / (RAND_MAX + 1)];
//	else return -1;
//}
//
/*int  GoBoard::capture_heuristic(int color)
{
	bool checked [MAX_BOARD*MAX_BOARD];
	for (int i = 0; i < board_size*board_size; ++i)
		checked[i] = false;
	int captures[MAX_BOARD*MAX_BOARD];
	int captures_number = 0;
	for (int i = 0; i < board_size*board_size; ++i)
	{
		if (board[i] == OTHER_COLOR(color))
		{
			int move = find_one_Liberty_for_atari2(I(i), J(i), checked);
			if (move == -1)
				continue;
			if (available(I(move), J(move), color))
				captures[captures_number++] = move;
		}
	}
	if (captures_number)
		return captures[rand()*captures_number / (RAND_MAX + 1)];
	return -1;
}*/

int GoBoard::capture_move(int bi, int bj, int color)
{
	if (!on_board(bi, bj))
		return -1;
	if (get_board(bi, bj) != OTHER_COLOR(color))
		return -1;
	if (board[POS(bi, bj)]->get_liberties_number() != 1)
		return -1;
	int move = board[POS(bi,bj)]->liberties[0];

	//if (!available(I(move), J(move), color))
	//	return -1;
	//if (is_virtual_eye(POS(bi, bj), color))
	//	return -1;
	//if (!gains_liberty(move,board[POS(bi,bj)]))
	//	return -1;
	return move;
}
int GoBoard::capture_heuristic(int color)// sometimes check  the same string
{
	int capture_moves[18];
	int captures_moves_number = 0;
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point) + around_i[i];
		int bj = J(last_point) + around_j[i];
		int move = capture_move(bi, bj, color);
		if (move != -1)
			capture_moves[captures_moves_number++] = move;
	}
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point2) + around_i[i];
		int bj = J(last_point2) + around_j[i];
		int move = capture_move(bi, bj, color);
		if (move != -1)
			capture_moves[captures_moves_number++] = move;
	}
	int bi = I(last_point);
	int bj = J(last_point);
	int move = capture_move(bi, bj, color);
	if (move != -1)
		capture_moves[captures_moves_number++] = move;
	
	move = random_choose_move(capture_moves, captures_moves_number, color);
	if(move<0)
		return -1;
	else return move;
}

int GoBoard::save_heuristic(int color)
{
	if (last_point < 0)
		return -1;
	int save_moves[18];
	int save_moves_number = 0;
	if (board[last_point] && board[last_point]->get_liberties_number() == 2)
	{
		for (int i = 0; i < 2; ++i)
		{
			int lib = board[last_point]->get_liberty(i);
			if (available(I(lib), J(lib), color) && gains_liberty(lib, board[last_point]))
			{
				save_moves[save_moves_number++] = lib;
			}
		}
	}
	int ai, aj, pos;
	for (int k = 0; k < 4; ++k)
	{
		ai = I(last_point) + deltai[k];
		aj = J(last_point) + deltaj[k];
		pos = POS(ai, aj);
		if (on_board(ai, aj) && get_board(ai, aj) == color && board[pos]->get_liberties_number() < 2)
		{
			for (int i = 0; i < board[pos]->get_liberties_number(); ++i)
			{
				int lib = board[pos]->get_liberty(i);
				if (available(I(lib), J(lib), color) && gains_liberty(lib, board[pos]))
				{
					save_moves[save_moves_number++] = lib;
				}
			}
		}
	}

	if (save_moves_number)
	{
		return save_moves[rand()*save_moves_number / (RAND_MAX + 1)];
	}
	return -1;
}

int GoBoard::last_atari_heuristic(int color)
{
	int last_atari_moves[4];
	int last_atari_moves_number = 0;

	if (on_board(I(last_point), J(last_point)) && board[last_point] && board[last_point]->get_liberties_number() == 1)
	{
		int lib = board[last_point]->get_liberty(0);
		if (available(I(lib), J(lib), color) && gains_liberty(lib, board[last_point]))
			last_atari_moves[last_atari_moves_number++] = lib;
	}
	if (on_board(I(last_point2), J(last_point2)) && board[last_point2] && board[last_point2]->get_liberties_number() == 1)
	{
		int lib = board[last_point2]->get_liberty(0);
		if (available(I(lib), J(lib), color) && gains_liberty(lib, board[last_point2]))
			last_atari_moves[last_atari_moves_number++] = lib;
	}
	if (last_atari_moves_number)
	{
		return last_atari_moves[rand()*last_atari_moves_number / (RAND_MAX + 1)];
	}
	return -1;
}

bool GoBoard::match_hane(int i, int j, int my_color)
{
	int pos = POS(i, j);
	for (int start = 0; start < 8; start += 2)
	{

		int around_points[8];
		for (int m = 0; m < 8; ++m)
		{
			around_points[m] = POS(i + around_i[(start + m) % 8], j + around_j[(start + m) % 8]);
		}
		if (!board[around_points[0]])
			continue;
		int color = board[around_points[0]]->get_color();
		if (!board[around_points[1]])
			continue;
		if (board[around_points[1]]->get_color() != OTHER_COLOR(color))
			continue;
		if (board[around_points[7]])
		{
			if (board[around_points[7]]->get_color() == color &&!board[around_points[3]] && !board[around_points[5]])
				return true;
			continue;
		}
		if (get_board(I(around_points[2]), J(around_points[2])) == color && get_board(I(around_points[3]), J(around_points[3])) == EMPTY)
			return true;
		if (!board[around_points[2]] && !board[around_points[3]] && !board[around_points[5]])
			return true;
		if (!board[around_points[3]] && !board[around_points[5]] && color == my_color  &&get_board(I(around_points[2]), J(around_points[2])) == OTHER_COLOR(color))
			return true;
	}
	for (int start = 0; start < 8; start += 2)
	{

		int around_points[8];
		for (int m = 0; m < 8; ++m)
		{
			around_points[m] = POS(i + around_i[(start - m+8) % 8], j + around_j[(start - m+8) % 8]);
		}
		if (!board[around_points[0]])
			continue;
		int color = board[around_points[0]]->get_color();
		if (!board[around_points[1]])
			continue;
		if (board[around_points[1]]->get_color() != OTHER_COLOR(color))
			continue;
		if (board[around_points[7]])
		{
			if (board[around_points[7]]->get_color() == color &&!board[around_points[3]] && !board[around_points[5]])
				return true;
			continue;
		}
		if (get_board(I(around_points[2]), J(around_points[2])) == color && get_board(I(around_points[3]), J(around_points[3])) == EMPTY)
			return true;
		if (!board[around_points[2]] && !board[around_points[3]] && !board[around_points[5]])
			return true;
		if (!board[around_points[3]] && !board[around_points[5]] && color == my_color  &&get_board(I(around_points[2]), J(around_points[2])) == OTHER_COLOR(color))
			return true;
	}


	return false; //?
}
bool GoBoard::match_cut1(int i, int j, int color) //symmetric
{
	for (int start = 0; start < 8; start += 2) //clock wise
	{
		bool color_same = false;
		if (!board[  POS(i+around_i[start],j+around_j[start] ) ])
			continue;
		if (board[POS(i + around_i[start], j + around_j[start])]->get_color() == BLACK)
			color_same = true;
		int bis[8];
		int bjs[8];
		for (int m = 0; m < 8; ++m) {
			bis[m] = i + around_i[(start + m) % 8];
			bjs[m] = j + around_j[(start + m) % 8];
		}
		if (color_same) {
			if (get_board(bis[3], bjs[3]) == WHITE &&
				get_board(bis[1], bjs[5]) == EMPTY)
				continue;
			if (get_board(bis[3], bjs[3]) == EMPTY &&
				get_board(bis[1], bjs[5]) == WHITE)
				continue;

			if (get_board(bis[0], bjs[0]) == BLACK &&
				get_board(bis[1], bjs[1]) == WHITE &&
				get_board(bis[7], bjs[7]) == WHITE)
				return true;
		}
		else
		{
			if (get_board(bis[3], bjs[3]) == BLACK &&
				get_board(bis[1], bjs[5]) == EMPTY)
				continue;
			if (get_board(bis[3], bjs[3]) == EMPTY &&
				get_board(bis[1], bjs[5]) == BLACK)
				continue;

			if (get_board(bis[0], bjs[0]) == WHITE &&
				get_board(bis[1], bjs[1]) == BLACK &&
				get_board(bis[7], bjs[7]) == BLACK)
				return true;
		}
	}
	return false;

}
bool GoBoard::match_cut2(int i, int j, int color) //symmetric
{
	for (int start = 0; start < 8; start += 2) //clock wise
	{
		bool color_same = false;
		if (!board[POS(i + around_i[(start+1)%8], j + around_j[(start + 1) % 8])])
			continue;
		if (board[POS(i + around_i[(start + 1) % 8], j + around_j[(start + 1) % 8])]->get_color() == BLACK)
			color_same = true;
		int bis[8];
		int bjs[8];
		for (int m = 0; m < 8; ++m) {
			bis[m] = i + around_i[(start + m) % 8];
			bjs[m] = j + around_j[(start + m) % 8];
		}
		if (color_same) {
			if (get_board(bis[4], bjs[4]) == WHITE ||
				get_board(bis[5], bjs[5]) == WHITE ||
				get_board(bis[6],bjs[6]) == WHITE)
				continue;
			if (get_board(bis[1], bjs[1]) == BLACK&&
				get_board(bis[3], bjs[3]) == WHITE&&
				get_board(bis[7], bjs[7]) == WHITE
				)
				return true;
		}
		else
		{
			if (get_board(bis[4], bjs[4]) == BLACK ||
				get_board(bis[5], bjs[5]) == BLACK ||
				get_board(bis[6], bjs[6]) == BLACK)
				continue;
			if (get_board(bis[1], bjs[1]) == WHITE&&
				get_board(bis[3], bjs[3]) == BLACK&&
				get_board(bis[7], bjs[7]) == BLACK
				)
				return true;
		}
	}
	return false;
}
bool GoBoard::match_board_side(int i, int j, int color)
{
	int start = 0;
	if (i == 0)
		start = 2;
	if (i == board_size - 1)
		start = 6;
	if (j == 0)
		start = 4;
	if (j == board_size - 1)
		start = 0;
	int bis[8];
	int bjs[8];
	for (int m = 0; m < 8; ++m) {
		bis[m] = i + around_i[(start + m) % 8];
		bjs[m] = j + around_j[(start + m) % 8];
	}
	//8.1
	if (get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
		return true;
	if (get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
		return true;
	if (get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
		return true;
	if (get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
		return true;
	//8.2
	if (get_board(bis[7], bjs[7]) != BLACK&&
		get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE
		)
		return true;
	if (get_board(bis[7], bjs[7]) != WHITE&&
		get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK
		)
		return true;
	if (get_board(bis[3], bjs[3]) != BLACK&&
		get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE
		)
		return true;
	if (get_board(bis[3], bjs[3]) != WHITE&&
		get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK
		)
		return true;
	//8.3
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		color == BLACK
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		color == WHITE
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		color == WHITE
		)
		return true;
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		color == BLACK
		)
		return true;
	//8.4
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) != BLACK&&
		color == WHITE
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) != WHITE&&
		color == BLACK
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) != WHITE&&
		color == BLACK
		)
		return true;
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) != BLACK&&
		color == WHITE
		)
		return true;
	//8.5
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		color == WHITE
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		color == BLACK
		)
		return true;
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		color == WHITE
		)
		return true;
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		color == BLACK
		)
		return true;
	return false;
}
bool GoBoard::match_mogo_pattern(int bi, int bj, int color)
{
	if ((bi == 0 && bj == 0) || (bi == board_size - 1 && bj == 0) || (bi == board_size - 1 && bj == board_size - 1) || (bi == 0 && bj == board_size - 1)) // at corner
		return false;

	if (bi > 0 && bi < board_size - 1 && bj>0 && bj < board_size - 1)
	{
		if (match_hane(bi, bj, color))
			return true;
		if (match_cut1(bi, bj, color))
			return true;
		
		if (match_cut2(bi, bj, color))
			return true;
	}
	else
	{
		if (match_board_side(bi, bj, color))
			return true;
	}
	return false;

}
int GoBoard::mogo_pattern_heuristic(int color)//require not self_atari!!!
{
	int matches[8];
	int matches_number = 0;
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point) + around_i[i];
		int bj = J(last_point) + around_j[i];
		if (!on_board(bi, bj))
			continue;
		if (get_board(bi, bj) != EMPTY)
			continue;
		if (match_mogo_pattern(bi, bj, color))
		{
			matches[matches_number++] = POS(bi, bj);
		}
	}
	int move = random_choose_move(matches, matches_number, color);
	if (move < 0)
		return -1;
	else return move;
}