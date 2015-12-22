#pragma once
#include "uctNode.h"
#include <time.h>
#include "GoBoard.h"
#include "constants.h"
#include <windows.h>

class GoEngine {
public:
	GoBoard * go_board;
	~GoEngine();
	int games;
	int move_color;

	uctNode *roots[THREAD_NUM];
	uctNode *root;
	clock_t fin_clock;
	CRITICAL_SECTION cs;


	GoEngine * copy_engine(GoBoard *b);
	GoEngine(GoBoard *b);
	/*uctNode* treePolicy(GoBoard * temp_board);*/
	uctNode* treePolicy(uctNode* v, int games);
	void uctSearch(int *pos, int color, int *moves, int num_moves);
	static DWORD WINAPI  ThreadFunc(LPVOID p);// originally static
	int POS(int i, int  j) { return ((i)* GoBoard::board_size + (j)); }
	static int  I(int pos) { return ((pos) / GoBoard::board_size); }
	static int  J(int pos) { return ((pos) % GoBoard::board_size); }
	uctNode* expand(uctNode* curNode, int* moves, int num_moves);
	uctNode* bestchild(uctNode* curNode);
	void calScore(uctNode* tmp);
	int defaultPolicy(GoBoard * temp, int color, bool* blackExist, bool* whiteExist);
	void backup(uctNode* v, int reward, bool* blackExist, bool* whiteExist);
	void generate_move(int *i, int *j, int color);
	void aiMovePreCheck(int *pos, int color, int *moves, int num_moves);
	void place_free_handicap(int handicap);
	void aiMove(int *pos, int color, int *moves, int num_moves);
};