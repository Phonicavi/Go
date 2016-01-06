#ifndef UCTNODE_H_INCLUDED
#define UCTNODE_H_INCLUDED

#include "vector"
#include "amaf.h"
class uctNode{
public:
	int play;
	int playResult;
	double amafPlay;
	double amafPlayResult;
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
	//void result(int r, bool* blackExist, bool* whiteExist, int simul, AmafBoard* tamaf);
	uctNode* copy();
	void show_node();
	void show_node(uctNode *node, int threshold, int depth);

	void uctNode::set_results(int result);
	void uctNode::set_amaf(int result, const AmafBoard &amaf, bool side, int depth);
};

#endif