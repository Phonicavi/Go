#include "GoEngine.h"
#include <time.h>
#include <stdlib.h>
#include <process.h>
#include <windows.h>
#include "algorithm"
#include <vector>
#include <fstream>

using namespace std;

GoEngine::~GoEngine()
{
	delete go_board;
	go_board = NULL;
}

GoEngine::GoEngine(GoBoard * b) {
	go_board = b->copy_board();
}

GoEngine * GoEngine::copy_engine(GoBoard *b)
{
	GoEngine * temp_engine = new GoEngine(b);
	temp_engine->games = games;
	temp_engine->move_color = move_color;
	if (!root)
		temp_engine->root = NULL;
	else
		temp_engine->root = root->copy();
	temp_engine->fin_clock = fin_clock;
	return temp_engine;
}

//uctNode* GoEngine::expand(uctNode* curNode, int* moves, int num_moves)
//{
//	for (int i = 0; i < num_moves; ++i)
//	{
//		bool flag = true;
//		for (int j = 0; j< (int)curNode->nextMove.size(); ++j)
//		{
//			if (moves[i] == curNode->nextMove[j]->pos)
//			{
//				flag = false;
//				break;
//			}
//		}
//		if (flag)//TODO
//		{
//			uctNode* nextchosenNode = new uctNode(moves[i], OTHER_COLOR(curNode->color), curNode);
//			curNode->addPos(nextchosenNode);
//			return nextchosenNode;
//		}
//	}
//	return NULL; //indicates error
//}


void GoEngine::calScore(uctNode* tmp)
{

	for (int ii = 0; ii < (int)tmp->nextMove.size(); ++ii)
	{
		uctNode *tt = tmp->nextMove[ii];
		if (tt->play == 0)
			tt->score = 0;
		else
			tt->score = (tt->playResult + 0.0) / tt->play;

		if (tt->amafPlay == 0)
		{
			tt->amafScore = 0;
		}
		else
			tt->amafScore = (tt->amafPlayResult + 0.0) / tt->amafPlay;
	}


}



//uctNode* GoEngine::bestchild(uctNode* curNode)
//{
//	if (curNode->nextMove.size() == 0)
//		return NULL;
//	calScore(curNode);
//	double V = 5000;
//	double alpha = V - curNode->nextMove[0]->play > 0 ? (V - curNode->nextMove[0]->play) / (V) : 0;
//	//alpha = curNode->nextMove[0]->amafPlay *1.0 / (curNode->nextMove[0]->play + curNode->nextMove[0]->amafPlay);
//	//alpha = 0.3;
//	double tmpScore;
//	double maxScore = alpha * curNode->nextMove[0]->amafScore + (1 - alpha)*curNode->nextMove[0]->score;
//	uctNode* best = curNode->nextMove[0];
//
//	for (int i = 1; i < (int )curNode->nextMove.size(); ++i)
//	{
//		alpha = V - curNode->nextMove[i]->play > 0 ? (V - curNode->nextMove[i]->play) / (V) : 0;
//		//alpha = curNode->nextMove[i]->amafPlay * 1.0 / (curNode->nextMove[i]->play + curNode->nextMove[i]->amafPlay);
//		tmpScore = alpha * curNode->nextMove[i]->amafScore + (1 - alpha)*curNode->nextMove[i]->score;
//		if (tmpScore > maxScore)
//		{
//			maxScore = tmpScore;
//			best = curNode->nextMove[i];
//		}
//	}
//
//	return best;
//}

uctNode* GoEngine::bestchild(uctNode* curNode)
{
	//if (curNode->nextMove.size() == 0)
	//{
	//	cerr << "unexpected" << endl;
	//	return NULL;
	//}
	calScore(curNode);
	const double BIAS = 1.0 / 3000000, UCTK = 0.0;


	double tmpScore;
	double maxScore = -1;
	uctNode* best = NULL;

	for (int i = 0; i < (int)curNode->nextMove.size(); ++i)
	{
//		cerr << curNode->nextMove[i]->pos << " ";
		if (curNode->nextMove[i]->play)
		{
			if (curNode->nextMove[i]->amafPlay)
			{
				//cout << curNode->nextMove[i]->amafPlay << curNode->nextMove[i]->pos << endl;
				double alpha = curNode->nextMove[i]->amafPlay / (curNode->nextMove[i]->amafPlay + curNode->nextMove[i]->play + curNode->nextMove[i]->amafPlay*curNode->nextMove[i]->play*BIAS);
				tmpScore = alpha * curNode->nextMove[i]->amafScore + (1 - alpha)*curNode->nextMove[i]->score;
			}
			else
				tmpScore = curNode->nextMove[i]->score;

		}
		else if (curNode->nextMove[i]->amafPlay)
		{
			//cerr << "??";
			tmpScore = curNode->nextMove[i]->amafScore;
		}
		else
		{
			//cerr << curNode->nextMove[i]->pos << "!" <<endl;
			tmpScore = 1;
		}

		if (tmpScore > maxScore)
		{
			maxScore = tmpScore;
			best = curNode->nextMove[i];

		}
	}

	return best;
}
//
//uctNode* GoEngine::treePolicy(uctNode* v, int games, AmafBoard *tamaf)
//{
//	uctNode* curNode = v;
//	int* moves = new int[MAX_BOARD * MAX_BOARD]; //available moves
//	int num_moves;	//available moves_count
//	while (curNode)
//	{
//		if (curNode->pos != last_point)
//		{
//			go_board->play_move(I(curNode->pos), J(curNode->pos), curNode->color);
//		}
//		
//		//if(curNode->total_num_moves = -1)
//		//	curNode->total_num_moves = go_board->generate_legal_moves(moves, OTHER_COLOR(curNode->color));
//		num_moves = go_board->generate_legal_moves(moves, OTHER_COLOR(curNode->color));
//
//		if (num_moves != curNode->nextMove.size()) //not fully expanded
//		{
//			uctNode* tmp = expand(curNode, moves, num_moves);
//			delete[]moves;
//			return tmp;
//		}
//		else
//		{
//			curNode = bestchild(curNode);
//		}
//	}
//	delete[]moves;
//	return curNode;
//}
uctNode* GoEngine::expand(uctNode* curNode, int* moves, int num_moves)
{

	for (int i = 0; i < num_moves; ++i)
	{
		uctNode *nextchosenNode = new uctNode(moves[i], OTHER_COLOR(curNode->color), curNode);
		curNode->nextMove.push_back(nextchosenNode);
	}

	return curNode;
}
uctNode* GoEngine::treePolicy(uctNode* v, int games, int* sim, AmafBoard *tamaf, std::vector<uctNode*> *node_history)
{
	uctNode* curNode = v;
	node_history->push_back(curNode);
	int pass = 0;
	while (curNode->nextMove.size() > 0 && pass < 2)
	{
		
		curNode = bestchild(curNode);
		//if (!curNode)
		//	cerr << "wt" << endl;
		node_history->push_back(curNode);
		int move = curNode->pos;
		if (move < 0)
			pass++;
		else pass = 0;
		go_board->play_move(I(move), J(move), curNode->color);
		//if (tamaf->side == 0 && curNode->color == BLACK)
		//	cerr << "wrong side";
		//if (tamaf->side == 1 && curNode->color == WHITE)
		//	cerr << "wrong side";
		tamaf->play(move, ++(*sim));
		
		//cout << "here" << endl;
	}
	int EXPAND = 40;
	if (curNode->play >= EXPAND || !curNode->lastMove)
	{
		int legal_moves[MAX_BOARD2];
		int nlegal = go_board->generate_legal_moves(legal_moves, OTHER_COLOR(curNode->color));
		expand(curNode, legal_moves, nlegal);
	}
	return curNode;
}
int GoEngine::defaultPolicy(GoBoard * temp, int color, int*simulate_len, AmafBoard* tamaf)
{
	return temp->autoRun_fill_the_board (color, simulate_len, tamaf);
}

void GoEngine::back_up_results(int result, std::vector<uctNode*> node_history, int nnodes, bool side, AmafBoard* tamaf)
{
	for (int i = 0; i < nnodes; i++) {
		node_history[i]->set_results(1 - result);
		node_history[i]->set_amaf(result, *tamaf, side, i + 1);
		side = !side;
		result = 1 - result;
	}
}

//void GoEngine::backup(uctNode* v, int reward, bool* blackExist, bool* whiteExist, int simulate_len, AmafBoard* tamaf)
//{
//	v->result(reward, blackExist, whiteExist, simulate_len, tamaf);
//}


/*unsigned __stdcall GoEngine::ThreadFunc(void * p)
{
int seed = GetCurrentThreadId()*time(NULL);
srand(seed);
int reward = 0;
GoEngine *engine = (GoEngine *) p;
while (engine->games < MAXGAMES && clock()-engine->fin_clock> MAXTIME )
{
GoBoard * temp_board = NULL;

uctNode* chosenNode = engine->treePolicy( temp_board);
if (!chosenNode)
break;
temp_board->play_move(engine->I(chosenNode->pos), engine->J(chosenNode->pos), chosenNode->color);
reward = engine->defaultPolicy(temp_board,OTHER_COLOR(chosenNode->color));
EnterCriticalSection(&engine->cs);
engine->backup(chosenNode, reward);
++engine->games;
LeaveCriticalSection(&engine->cs);
delete temp_board;
}
return 0;
}*/
//
struct param {
	GoEngine * go_engine;
	int thread_id;
};

DWORD WINAPI  GoEngine::ThreadFunc(LPVOID p)
{
	unsigned int seed = GetCurrentThreadId()*time(NULL);//no problem???
	srand(seed);
	param * temp_p = (param *)p;
	GoEngine * engine = temp_p->go_engine;
	GoEngine * temp_engine = engine->copy_engine(engine->go_board); // remember to delete
	uctNode* root = new uctNode(temp_engine->go_board->last_point, OTHER_COLOR(temp_engine->move_color), NULL);
	int reward = 0;


	while (/*temp_engine->games < MAXGAMES||*/(clock()-temp_engine->fin_clock)<MAXTIME  )  ///visit engine-> games may cause problem, we need to add lock,  or just use time information rather than games information
	{
		std::vector<uctNode*> node_history;

		int simulate_len = 0;
		AmafBoard tamaf = AmafBoard(170);
		bool side;
		if (temp_engine->move_color == BLACK)
			side = 1;
		else
			side = 0;
		tamaf.set_up(side, 170);

		//TODO
		uctNode* chosenNode = temp_engine->treePolicy(root, temp_engine->games, &simulate_len, &tamaf, &node_history);//treePolicy's engine->games parameter no used?
		//cout << "size of node_history:" << node_history.size();

		if (!chosenNode)
		{
			cerr << "No chosenNode";
			break;
		}
		//cerr << chosenNode->color << "*" << tamaf.side;
		//if (temp_engine->go_board->on_board(I(chosenNode->pos), J(chosenNode->pos)) && temp_engine->go_board->get_board(I(chosenNode->pos), J(chosenNode->pos)) == EMPTY)
		//{
		//	cerr << chosenNode->pos;
		//	temp_engine->go_board->play_move(I(chosenNode->pos), J(chosenNode->pos), chosenNode->color);
		//	tamaf.play(chosenNode->pos, ++simulate_len);
		//}
		

		//if (chosenNode->color == BLACK && tamaf.side == 1)
		//	cerr << "???";
		//if (chosenNode->color == WHITE && tamaf.side == 0)
		//	cerr << "???";
		reward = temp_engine->defaultPolicy(temp_engine->go_board, OTHER_COLOR(chosenNode->color), &simulate_len, &tamaf);
		//cout << simulate_len << "here" << endl;

		++temp_engine->games;
		for (int i = 0; i < GoBoard::board_size2; ++i)
		{
			temp_engine->go_board->strings[i].clear();

			if (engine->go_board->strings[i].get_stones_number()>0 )
				temp_engine->go_board->strings[i] = engine->go_board->strings[i];
			temp_engine->go_board->board[i] = NULL;
		}
		for (int i = 0; i < GoBoard::board_size2; ++i)
		{
			for (int j = 0; j < engine->go_board->strings[i].get_stones_number(); ++j)
			{
				temp_engine->go_board->board[engine->go_board->strings[i].stones[j]] = &temp_engine->go_board->strings[i];
			}

		}
		temp_engine->go_board->empty_points_number = engine->go_board->empty_points_number;
		for (int i = 0; i < engine->go_board->empty_points_number; ++i)
		{
			temp_engine->go_board->empty_points[i] = engine->go_board->empty_points[i];
		}
		temp_engine->go_board->ko_i = engine->go_board->ko_i;
		temp_engine->go_board->ko_j = engine->go_board->ko_j;
		temp_engine->go_board->step = engine->go_board->step;
		temp_engine->go_board->last_point = engine->go_board->last_point;
		temp_engine->go_board->last_point2 = engine->go_board->last_point2;
		temp_engine->go_board->stones_on_board[BLACK] = engine->go_board->stones_on_board[BLACK];
		temp_engine->go_board->stones_on_board[WHITE] = engine->go_board->stones_on_board[WHITE];
		//for (int ii = 0; ii < GoBoard::board_size*GoBoard::board_size; ++ii)
		//{
		//	temp_engine->go_board->board[ii] = engine->go_board->board[ii];
		//	temp_engine->go_board->next_stone[ii] = engine->go_board->next_stone[ii];
		//}
		//temp_engine->go_board->step = engine->go_board->step;
		//temp_engine->go_board->ko_i = engine->go_board->ko_i;
		//temp_engine->go_board->ko_j = engine->go_board->ko_j;

		//go_board = store->copy_board();
		if (reward == -1)
		{
			cerr << "not finished";
			continue;
		}

		//temp_engine->backup(chosenNode, reward, blackExist, whiteExist, depth, &tamaf);

		if (chosenNode->color == root->color)
			reward = 1 - reward;
		//uctNode* test = chosenNode;
		//int len = node_history.size();
		//while (test)
		//{
		//	if (test != node_history[len - 1])
		//		cerr << "his error";
		//	test = test->lastMove;
		//	--len;
		//}
		temp_engine->back_up_results(reward, node_history, node_history.size(), side, &tamaf);


	}

	printf("%d\n", temp_engine->games);
	engine->roots[temp_p->thread_id] = root;
	delete temp_engine;
	return 0;
}

void GoEngine::uctSearch(int *pos, int color, int *moves, int num_moves)
{

	games = 0;
	HANDLE handles[THREAD_NUM];
	DWORD threadid[THREAD_NUM];
	param para[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; i++) {
		para[i].go_engine = this;
		para[i].thread_id = i;
		handles[i] = CreateThread(NULL, 0, GoEngine::ThreadFunc, &para[i], 0, &threadid[i]);

	}
	WaitForMultipleObjects(THREAD_NUM, handles, TRUE, INFINITE);
	int * votes = new int[GoBoard::board_size*GoBoard::board_size];
	int *visits = new int[GoBoard::board_size*GoBoard::board_size];
	memset(votes, 0, sizeof(int)*GoBoard::board_size*GoBoard::board_size);
	memset(visits, 0, sizeof(int)*GoBoard::board_size*GoBoard::board_size);

	//if (roots[0]->nextMove.size()>0)		// exist nextmove
	if (roots[0])
	{
		roots[0]->show_node();
		//for (int i = 0; i < roots[0]->nextMove.size(); ++i)
		//{
		//	if (roots[0]->nextMove[i]->play >100)
		//		cout << i << ":" << roots[0]->nextMove[i]->play<<endl;
		//}
		//find the votes for each nextmove to "votes" and add all the plays to "visits" 
		for (int i = 0; i < THREAD_NUM; ++i)//find every best move for each root. and add play for every root->nextmove to visits
		{
			if (roots[i] == NULL || roots[i]->nextMove.size() == 0)
			{
				continue;
			}
			int bestmove = -1;
			int mostvisit = -1;
			for (vector<uctNode*>::iterator it = roots[i]->nextMove.begin(); it != roots[i]->nextMove.end(); ++it)
			{
				if ((*it)->play > mostvisit)
				{
					bestmove = (*it)->pos;
					mostvisit = (*it)->play;
				}
				visits[(*it)->pos] += (*it)->play;
			}
			votes[bestmove] += 1;
		}
		//If only one best move according to the votes, then chose it
		//If there are more than one best moves according to the votes,for example, two nextmove both get two votes, then chose the most visited one.

		int have_same_votes = 0;//0 represents false
		int final_most_votes_move = -1;
		int final_most_votes = -1;
		int final_most_visits = -1;
		int final_most_visits_move = -1;
		for (int i = 0; i < GoBoard::board_size*GoBoard::board_size; ++i)
		{
			if (votes[i] > final_most_votes)
			{
				final_most_votes = votes[i];
				
				final_most_votes_move = i;
				have_same_votes = 0;
			}
			else
			{
				if (votes[i] == final_most_votes)
				{
					have_same_votes = 1;
				}
			}
			if (visits[i] > final_most_visits)
			{
				final_most_visits = visits[i];
				final_most_visits_move = i;
			}
		}

		if (have_same_votes)
		{
			*pos = final_most_visits_move;
		}
		else
		{
			*pos = final_most_votes_move;
		}
	}
	else
	{
		*pos = -1;
	}
	if ((*pos) == POS(go_board->ko_i, go_board->ko_j) || !go_board->legal_move(I(*pos), J(*pos), color))
	{
		(*pos) = -1;
	}
	
	delete []votes;
	delete []visits;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		delete roots[i];
	}
	//delete store;


}

void GoEngine::aiMove(int *pos, int color, int *moves, int num_moves)
{
	//aiMovePreCheck(pos, color, moves, num_moves);
	//if (*pos == -1)
	//aiMoveStart(pos, color);
	if (*pos == -1)
		uctSearch(pos, color, moves, num_moves);
}

void GoEngine::aiMoveStart(int *pos, int color)
{
	if (go_board->step < MAX_BEGINING)
	{
		int move;

		move = go_board->is_xiaomu_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
		move = go_board->is_anti_kakari_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
		move = go_board->is_anti_yijianjia_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
		move = go_board->is_anti_dian33_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
		move = go_board->is_star_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
		move = go_board->is_kakari_available(color, go_board->last_point);
		if (move != -1) { *pos = move; return; }
	}
	*pos = -1;
	return;
}

/* Generate a move. */
void GoEngine::generate_move(int *i, int *j, int color)
{

	move_color = color;
	int moves[MAX_BOARD * MAX_BOARD];
	int num_moves = 0;
	int ai, aj;
	int k;
	



	memset(moves, 0, sizeof(moves));
	for (ai = 0; ai < GoBoard::board_size; ai++)
	{
		for (aj = 0; aj < GoBoard::board_size; aj++)
		{
			/* Consider moving at (ai, aj) if it is legal and not suicide. */
			if (go_board->legal_move(ai, aj, color) && !go_board->suicide(ai, aj, color))
			{
				/* Further require the move not to be suicide for the opponent... */
				if (!go_board->suicide(ai, aj, OTHER_COLOR(color)))
					moves[num_moves++] = POS(ai, aj);
				else
				{
					/* ...however, if the move captures at least one stone,
					* consider it anyway.
					*/
					for (k = 0; k < 4; k++)
					{
						int bi = ai + go_board->deltai[k];
						int bj = aj + go_board->deltaj[k];
						if (go_board->on_board(bi, bj) && go_board->get_board(bi, bj) == OTHER_COLOR(color))
						{
							moves[num_moves++] = POS(ai, aj);
							break;
						}
						//if (go_board->on_board(bi,bj)&& go_board->get_board(bi, bj) == color && go_board->checkLiberty(bi, bj) == 1)
						//{
						//	moves[num_moves++] = POS(ai, aj);
						//	break;
						//}
					}
				}
			}
		}
	}
	/* Choose one of the considered moves randomly with uniform
	* distribution. (Strictly speaking the moves with smaller 1D
	* coordinates tend to have a very slightly higher probability to be
	* chosen, but for all practical purposes we get a uniform
	* distribution.)
	*/
	//int move = go_board->select_and_play(color);
	//{
	//	if (move == -1)
	//	{
	//		*i = -1;
	//		*j = -1;
	//	}
	//	else
	//	{
	//		*i = I(move);
	//		*j = J(move);
	//	}
	//	return;
	//}
	if (num_moves > 0)
	{
		int temp_pos = -1;
		aiMove(&temp_pos, color, moves, num_moves);
		if (temp_pos == -1)
		{
			(*i) = -1;
			(*j) = -1;
		}
		else
		{
			*i = I(temp_pos);
			*j = J(temp_pos);
		}
	}
	else
	{
		*i = -1;
		*j = -1;
		//go_board->show_board();
		return;
	}
}


/* Put free placement handicap stones on the board. We do this simply
* by generating successive black moves.
*/
void GoEngine::place_free_handicap(int handicap)
{
	int k;
	int i, j;

	for (k = 0; k < handicap; k++) {
		generate_move(&i, &j, BLACK);
		go_board->play_move(i, j, BLACK);
	}
}

//void GoEngine::aiMovePreCheck(int *pos, int color, int *moves, int num_moves)
//{
//	*pos = -1;
//	for (int i = 0; i < GoBoard::board_size; ++i)
//	{
//		for (int j = 0; j < GoBoard::board_size; ++j)
//		{
//			if (!go_board->on_board(i, j) || go_board->get_board(i, j) != OTHER_COLOR(color))
//				continue;
//			if (go_board->checkLiberty(i, j) == 1)
//			{
//				int ppos = go_board->findALiberty(i, j);
//				if (go_board->available(I(ppos), J(ppos), color))
//				{
//					*pos = ppos;
//					return;
//				}
//			}
//		}
//	}
//
//	int * store_board = new int[GoBoard::board_size*GoBoard::board_size];
//	int * store_next_stone = new int[GoBoard::board_size*GoBoard::board_size];
//	for (int i = 0; i<GoBoard::board_size*GoBoard::board_size; ++i)
//	{
//		store_board[i] = go_board->board[i];
//		store_next_stone[i] = go_board->next_stone[i];
//	}
//	int storeStep = go_board->step;
//	int storeko_i = go_board->ko_i;
//	int storeko_j = go_board->ko_j;
//	int other = OTHER_COLOR(color);
//	for (int i = max(0, I(go_board->last_point) - PRECHECKRANGE); i <= (GoBoard::board_size - 1, I(go_board->last_point) + PRECHECKRANGE); ++i)
//	{
//		for (int j = max(0,J(go_board->last_point)- PRECHECKRANGE); j <= (GoBoard::board_size - 1, J(go_board->last_point) + PRECHECKRANGE); ++j)
//		{
//			if (go_board->on_board(i, j) && go_board->get_board(i, j) == color)
//			{
//				if (go_board->checkLiberty(i, j) == 2)
//				{
//					int pos0 = POS(i, j);
//					int pos1 = pos0;
//					do{
//						int ai = I(pos1);
//						int aj = J(pos1);
//						for (int k = 0; k < 4; ++k)
//						{
//							int bi = ai + go_board->deltai[k];
//							int bj = aj + go_board->deltaj[k];
//							if (go_board->on_board(bi, bj) && go_board->get_board(bi, bj) == EMPTY && go_board->available(bi, bj, color))
//							{
//								go_board->play_move(bi, bj, color);
//								int tmpLiberty = go_board->checkLiberty(bi, bj);
//								if (tmpLiberty > 3)
//								{
//									*pos = POS(bi, bj);
//									for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//									{
//										go_board->board[ii] = store_board[ii];
//										go_board->next_stone[ii] = store_next_stone[ii];
//									}
//									go_board->step = storeStep;
//									go_board->ko_i = storeko_i;
//									go_board->ko_j = storeko_j;
//									return;
//								}
//								for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//								{
//									go_board->board[ii] = store_board[ii];
//									go_board->next_stone[ii] = store_next_stone[ii];
//								}
//								go_board->step = storeStep;
//								go_board->ko_i = storeko_i;
//								go_board->ko_j = storeko_j;
//							}
//						}
//						pos1 = go_board->next_stone[pos1];
//					} while (pos1 != pos0);
//				}
//				else if (go_board->checkLiberty(i, j) == 1)
//				{
//					int pos0 = POS(i, j);
//					int pos1 = pos0;
//					do{
//						int ai = I(pos1);
//						int aj = J(pos1);
//						for (int k = 0; k < 4; ++k)
//						{
//							int bi = ai + go_board->deltai[k];
//							int bj = aj + go_board->deltaj[k];
//							if (go_board->on_board(bi, bj) && go_board->get_board(bi, bj) == EMPTY && go_board->available(bi, bj, color))
//							{
//								go_board->play_move(bi, bj, color);
//								int tmpLiberty = go_board->checkLiberty(i, j);
//								if (go_board->get_board(i, j) == EMPTY || tmpLiberty > 2)
//								{
//									*pos = POS(bi, bj);
//									for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//									{
//										go_board->board[ii] = store_board[ii];
//										go_board->next_stone[ii] = store_next_stone[ii];
//									}
//									go_board->step = storeStep;
//									go_board->ko_i = storeko_i;
//									go_board->ko_j = storeko_j;
//									delete[]store_board;
//									delete[]store_next_stone;
//									return;
//								}
//								for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//								{
//									go_board->board[ii] = store_board[ii];
//									go_board->next_stone[ii] = store_next_stone[ii];
//								}
//								go_board->step = storeStep;
//								go_board->ko_i = storeko_i;
//								go_board->ko_j = storeko_j;
//							}
//						}
//						pos1 = go_board->next_stone[pos1];
//					} while (pos1 != pos0);
//				}
//			}
//			if (go_board->on_board(i, j) && go_board->get_board(i, j) == other)
//			{
//				if (go_board->checkLiberty(i, j) == 2)
//				{
//					int storei[2];
//					int storej[2];
//					int kk = 0;
//					for (int k = 0; k< 4; ++k)
//					{
//						int bi = i + go_board->deltai[k];
//						int bj = j + go_board->deltaj[k];
//						if (go_board->on_board(bi, bj) && go_board->get_board(bi, bj) == EMPTY && go_board->available(bi, bj, color) && !go_board->suicideLike(bi, bj, color))
//						{
//							storei[kk] = bi;
//							storej[kk] = bj;
//							++kk;
//						}
//					}
//					if (kk == 1)
//					{
//						*pos = POS(storei[0], storej[0]);
//						return;
//					}
//					if (kk == 0)
//						continue;
//					for (int k = 0; k < 2; ++k)
//					{
//						go_board->play_move(storei[k], storej[k], color);
//						/*for (int kkk = 0; kkk < 4; ++kkk)
//						{
//						int aai = storei[k] - deltai[kkk];
//						int aaj = storej[k] - deltaj[kkk];
//						if (aai==i || aaj==j)
//						continue;
//						if (on_board(aai, aaj) && get_board(aai, aaj) == other && !same_string(POS(i, j), POS(aai, aaj)) && checkLiberty(aai, aaj) == 1)
//						{
//						*pos = POS(storei[k], storej[k]);
//						for (int ii = 0; ii<board_size*board_size; ++ii)
//						{
//						board[ii] = store_board[ii];
//						next_stone[ii] = store_next_stone[ii];
//						}
//						step = storeStep;
//						ko_i = storeko_i;
//						ko_j = storeko_j;
//						delete[]store_board;
//						delete[]store_next_stone;
//						return;
//						}
//						}*/
//						go_board->play_move(storei[1 - k], storej[1 - k], other);
//						if (go_board->checkLiberty(i, j) <= 2)
//						{
//							*pos = POS(storei[k], storej[k]);
//							for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//							{
//								go_board->board[ii] = store_board[ii];
//								go_board->next_stone[ii] = store_next_stone[ii];
//							}
//							go_board->step = storeStep;
//							go_board->ko_i = storeko_i;
//							go_board->ko_j = storeko_j;
//							delete[]store_board;
//							delete[]store_next_stone;
//							return;
//						}
//						for (int ii = 0; ii<GoBoard::board_size*GoBoard::board_size; ++ii)
//						{
//							go_board->board[ii] = store_board[ii];
//							go_board->next_stone[ii] = store_next_stone[ii];
//						}
//						go_board->step = storeStep;
//						go_board->ko_i = storeko_i;
//						go_board->ko_j = storeko_j;
//					}
//				}
//			}
//		}
//	}
//	delete[]store_board;
//	delete[]store_next_stone;
//
//	*pos = -1;
//}