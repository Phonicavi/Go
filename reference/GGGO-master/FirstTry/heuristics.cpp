#include "GoBoard.h"
#include <random>
int GoBoard::last_atari_heuristic( int color)
{
	if (rival_move_i == -1)
		return -1;
	int rival_lib = find_one_Liberty_for_atari(rival_move_i, rival_move_j);//if rival has only one liberty, then capture it

	if (rival_lib != -1 && available(I(rival_lib), J(rival_lib), color))
		return rival_lib;
	int saves[4];
	int saves_number = 0;
	for (int i = 0; i < 4; ++i) {									//check whether there is a atari
		int neighbor_i = rival_move_i + deltai[i];
		int neighbor_j = rival_move_j + deltaj[i];
		if (get_board(neighbor_i, neighbor_j) == color)
		{
			int move = find_one_Liberty_for_atari(neighbor_i, neighbor_j); // only one liberty means atari
			if (move == -1)
				continue;
			int libs = 0;										// deal with atari
			for (int j = 0; j < 4 && libs<2; ++j)
			{
				int ne_lib_i = I(move) + deltai[j];				//check the liberty's near by provides more liberty 
				int ne_lib_j = J(move) + deltaj[j];
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
			if (libs > 1 && available(I(move), J(move), color))
			{
				saves[saves_number++] = move;
			}
		}

	}
	if (saves_number)
		return  saves[rand()*saves_number / (RAND_MAX + 1)];
	else return -1;
}