#include "GoBoard.h"
#include <random>
void GoBoard::try_to_save_by_eat(int i, int j, int *saves, int &saves_number)// find the group's adjacent groups and check whether we can eat it
{
	int color = get_board(i, j);
	int ai;
	int aj;
	int pos = POS(i, j);
	int pos1 = pos;
	bool checked[MAX_BOARD*MAX_BOARD];
	for (int i = 0; i < board_size*board_size; ++i)
	{
		checked[i] = false;
	}
	do {
		ai = I(pos1);
		aj = J(pos1);
		for (int k = 0; k < 4; ++k)
		{
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (!on_board(bi, bj))
				continue;
			if (get_board(bi, bj) == OTHER_COLOR(color))
			{
				int move = find_one_Liberty_for_atari2(bi, bj, checked);
				if (move == -1)
					continue;
				if (available(I(move), J(move), color))
				{
					saves[saves_number++] = move;
				}
			}

		}
		pos1 = next_stone[pos1];

	} while (pos1 != pos);

}

int GoBoard::gains_liberty(int move,int color)
{
	int libs = 0;
	for (int j = 0; j < 4 && libs<2; ++j)
	{
		int ne_lib_i = I(move) + deltai[j];				//check the liberty's near by provides more liberty 
		int ne_lib_j = J(move) + deltaj[j];
		if (!on_board(ne_lib_i, ne_lib_j))
			continue;
		if (get_board(ne_lib_i, ne_lib_j) == OTHER_COLOR(color))
			continue;
		if (get_board(ne_lib_i, ne_lib_j) == EMPTY)
		{
			libs++;
			continue;
		}
		if (get_board(ne_lib_i, ne_lib_j) == color)
		{
			libs += checkLiberty(ne_lib_i, ne_lib_j) - 1;
		}
	}
	if (libs > 1)
		return true;
	return false;
}

int GoBoard::last_atari_heuristic( int color)
{
	if (rival_move_i == -1)
		return -1;

	int saves[169];
	int saves_number = 0;
	for (int i = 0; i < 4; ++i) {									//check whether there is a atari
		int neighbor_i = rival_move_i + deltai[i];
		int neighbor_j = rival_move_j + deltaj[i];
		if (!on_board(neighbor_i, neighbor_j))
			continue;
		if (get_board(neighbor_i, neighbor_j) == color)
		{

			int move = find_one_Liberty_for_atari(neighbor_i, neighbor_j); // only one liberty means in atari
			if (move == -1)
				continue;
			try_to_save_by_eat(neighbor_i, neighbor_j, saves, saves_number);
			/*try to save by  escape, that is to check the "move" provides more liberty*/
			if (available (I(move),J(move),color)&& gains_liberty(move, color) )
				saves[saves_number++] = move;
		}

	}
	if (saves_number)
		return  saves[rand()*saves_number / (RAND_MAX + 1)];
	else return -1;
}

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
int GoBoard::capture_heuristic(int color)
{
	int capture_moves[8];
	int captures_moves_number = 0;
	for (int i = 0; i < 8; ++i)
	{
		int bi = rival_move_i + around_i[i];
		int bj = rival_move_j + around_j[i];
		if (!on_board(bi, bj))
			continue;
		if (get_board(bi, bj) != OTHER_COLOR(color))
			continue;
		int move = find_one_Liberty_for_atari(bi, bj);
		if (move == -1)
			continue;
		if (!available(I(move),J(move),color))
			continue;
		if (!gains_liberty(move, OTHER_COLOR(color)))
			continue;
		capture_moves[captures_moves_number++] = move;
	}
	if (captures_moves_number)
	{
		return capture_moves[rand()*captures_moves_number / (RAND_MAX + 1)];
	}
	return -1;
}

bool GoBoard::match_hane(int i, int j, int color)
{
	//white 1
	//black 2
	//empty 0
	//cross -1
	int graph1[8] = { 2,1,2,0,0,-1,-1,-1 };
	int graph2[8] = { 2,1,0,0,0,-1,0,-1 };
	int graph3[8] = { 2,1,-1,2,0,-1,0,-1 };
	int graph4[8] = { 2,1,1,0,0,-1,0,-1 };
	for (int start = 0; start < 8; start += 2) //clock wise
	{
		bool color_same = false;
		if (board[start] == EMPTY)
			continue;
		if (board[start] == BLACK)
			color_same = true;
		int bis[8];
		int bjs[8];
		for (int m = 0; m < 8; ++m) {
			bis[m] = i + around_i[(start + m) % 8];
			bjs[m] = j + around_j[(start + m) % 8];
		}
		bool match = true;
		if (color_same) {

			for (int m = 0; m < 8; ++m)
			{
				if (graph1[m] != -1 && graph1[m] != get_board(bis[m], bjs[m])) //match_graph1
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;

			for (int m = 0; m < 8; ++m)
			{
				if (graph2[m] != -1 && graph2[m] != get_board(bis[m], bjs[m])) //match_graph2
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;
			for (int m = 0; m < 8; ++m)
			{
				if (graph3[m] != -1 && graph3[m] != get_board(bis[m], bjs[m])) //match_graph3
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			if (color != BLACK)
				continue;
			for (int m = 0; m < 8; ++m)
			{
				if (graph4[m] != -1 && graph4[m] != get_board(bis[m], bjs[m]))
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
		}
		else
		{
			for (int m = 0; m < 8; ++m)
			{
				if (graph1[m] != -1 && !(graph1[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph1[m]) != get_board(bis[m], bjs[m])) //match_graph1
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;

			for (int m = 0; m < 8; ++m)
			{
				if (graph2[m] != -1 && !(graph2[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph2[m]) != get_board(bis[m], bjs[m])) //match_graph2
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;
			for (int m = 0; m < 8; ++m)
			{
				if (graph3[m] != -1 && !(graph3[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph3[m]) != get_board(bis[m], bjs[m])) //match_graph3
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			if (color != WHITE)
				continue;
			for (int m = 0; m < 8; ++m)
			{
				if (graph4[m] != -1 && !(graph4[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph4[m]) != get_board(bis[m], bjs[m])) //match_graph4
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;

		}

	}

	for (int start = 0; start < 8; start += 2) //unti_clockwise, graph1 is symetric, so no need to consider
	{
		bool color_same = false;
		if (board[start] == EMPTY)
			continue;
		if (board[start] == BLACK)
			color_same = true;
		int bis[8];
		int bjs[8];
		for (int m = 0; m < 8; ++m) {
			bis[m] = i + around_i[(start - m + 8) % 8];
			bjs[m] = j + around_j[(start - m+8) % 8];
		}
		bool match = true;
		if (color_same) {

			for (int m = 0; m < 8; ++m)
			{
				if (graph2[m] != -1 && graph2[m] != get_board(bis[m], bjs[m])) //match_graph2
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;
			for (int m = 0; m < 8; ++m)
			{
				if (graph3[m] != -1 && graph3[m] != get_board(bis[m], bjs[m])) //match_graph3
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			if (color != BLACK)
				continue;
			for (int m = 0; m < 8; ++m)
			{
				if (graph4[m] != -1 && graph4[m] != get_board(bis[m], bjs[m]))
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
		}
		else
		{
			for (int m = 0; m < 8; ++m)
			{
				if (graph2[m] != -1 && !(graph2[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph2[m]) != get_board(bis[m], bjs[m])) //match_graph2
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			match = true;
			for (int m = 0; m < 8; ++m)
			{
				if (graph3[m] != -1 && !(graph3[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph3[m]) != get_board(bis[m], bjs[m])) //match_graph3
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;
			if (color != WHITE)
				continue;
			for (int m = 0; m < 8; ++m)
			{
				if (graph4[m] != -1 && !(graph4[m] == EMPTY && get_board(bis[m], bjs[m]) == EMPTY) && OTHER_COLOR(graph4[m]) != get_board(bis[m], bjs[m])) //match_graph4
				{
					match = false;
					break;
				}
			}
			if (match)
				return true;

		}

	}
	return false;

}
bool GoBoard::match_cut1(int i, int j, int color) //symmetric
{
	for (int start = 0; start < 8; start += 2) //clock wise
	{
		bool color_same = false;
		if (board[start] == EMPTY)
			continue;
		if (board[start] == BLACK)
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
		if (board[start] == EMPTY)
			continue;
		if (board[start] == BLACK)
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
		int bi = rival_move_i + around_i[i];
		int bj = rival_move_j + around_j[i];
		if (!on_board(bi, bj))
			continue;
		if (get_board(bi, bj) != EMPTY)
			continue;
		if (!available(bi, bj, color))
			continue;
		if (match_mogo_pattern(bi, bj, color))
		{
			matches[matches_number++] = POS(bi, bj);
		}
	}
	if (matches_number)
		return matches[rand()*matches_number / (RAND_MAX + 1)];
	return -1;
}