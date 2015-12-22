#ifndef UCTNODE_H_INCLUDED
#define UCTNODE_H_INCLUDED

#include "vector"

class uctNode{
public:
	int play;
	int playResult;
	int amafPlay;
	int amafPlayResult;
	int color;
	int pos;
	bool opened;
	double score;
	double amafScore;
	std::vector<uctNode*> nextMove;
	uctNode* lastMove;
	uctNode(int p, int c, uctNode *last);
	~uctNode();
	void addPos(uctNode* p);
	void result(int r, bool* blackExist, bool* whiteExist);
	uctNode* copy();
};

#endif