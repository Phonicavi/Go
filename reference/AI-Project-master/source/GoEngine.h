#pragma once
#include "uctNode.h"
#include <time.h>
#include "GoBoard.h"
#include "constants.h"
#include <windows.h>
#include "amaf.h"

class GoEngine {
private:

	int games;
	int move_color;

	uctNode *roots[THREAD_NUM];
	uctNode *root;

public:
	clock_t fin_clock;
	GoBoard * go_board;
	~GoEngine();

	GoEngine * copy_engine(GoBoard *b);
	GoEngine(GoBoard *b);
	/*uctNode* treePolicy(GoBoard * temp_board);*/
	uctNode* treePolicy(uctNode* v, int games, int* sim, AmafBoard *tamaf, std::vector<uctNode*> *node_history);
	void uctSearch(int *pos, int color, int *moves, int num_moves);
	static DWORD WINAPI  ThreadFunc(LPVOID p);// originally static
	int POS(int i, int  j) { return ((i)* GoBoard::board_size + (j)); }
	static int  I(int pos) { return ((pos) / GoBoard::board_size); }
	static int  J(int pos) { return ((pos) % GoBoard::board_size); }
	uctNode* expand(uctNode* curNode, int* moves, int num_moves);
	uctNode* bestchild(uctNode* curNode);
	void calScore(uctNode* tmp);
	int defaultPolicy(GoBoard * temp, int color, int*simulate_len, AmafBoard* tamaf);
	//void backup(uctNode* v, int reward, bool* blackExist, bool* whiteExist, int simulate_len, AmafBoard* tamaf);
	void generate_move(int *i, int *j, int color);
	//void aiMovePreCheck(int *pos, int color, int *moves, int num_moves);
	void place_free_handicap(int handicap);
	void aiMove(int *pos, int color, int *moves, int num_moves);
	void aiMoveStart(int *pos, int color);

	void GoEngine::back_up_results(int result, std::vector<uctNode*> node_history, int nnodes, bool side, AmafBoard* tamaf);
};