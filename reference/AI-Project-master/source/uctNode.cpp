#include "uctNode.h"
#include "constants.h"
#include <fstream>
#include <iostream>
#include <iomanip>

uctNode* uctNode::copy()
{
	uctNode* tmp = new uctNode(pos, color, lastMove);
	tmp->play = play;
	tmp->playResult = playResult;
	tmp->score = score;
	tmp->opened = opened;
	for (int i = 0; i < nextMove.size(); ++i)
	{
		tmp->addPos(nextMove[i]);
	}
	return tmp;
}

uctNode::uctNode(int p, int c, uctNode* last)
{
	play = 0;
	playResult = 0;
	pos = p;
	color = c;
	score = 0.0;
	amafPlay = 0;
	amafPlayResult = 0;
	amafScore = 0.0;
	lastMove = last;
	opened = false;
}

void uctNode::addPos(uctNode* p)
{
	nextMove.push_back(p);
}

uctNode::~uctNode()
{
	for (int i = 0; i < nextMove.size(); ++i)
	{
		delete nextMove[i];
	}
}

void uctNode::set_results(int result)
{
	play++;
	playResult += result;
}

void uctNode::set_amaf(int result, const AmafBoard &amaf, bool side, int depth)
{
	const double discount = 0.0001; //0.0001;
	for (int i = 0; i < nextMove.size(); ++i) 
	{
		uctNode *next = nextMove[i];
		//if (next->color == BLACK && side == 0)
		//	std::cerr << "error";
		//if (next->color == WHITE && side == 1)
		//	std::cerr << "error";
		next->amafPlayResult += result * amaf.value(next->pos, depth, side, discount);
		next->amafPlay += amaf.value(next->pos, depth, side, discount);
	}
}

void uctNode::show_node(uctNode *node, int threshold,int depth)
{
	if (depth > 4)
		return;
	for (int i = 0; i < depth; ++i) std::cerr << " ";
	std::cerr << "|->";
	std::cerr.precision(4);
	std::cerr << std::right << std::setw(4) << node->pos <<
		": " << std::right << std::setw(6) << node->playResult*1.0 / node->play
		<< "/" << std::left << std::setw(5) << node->play
		<< "[ RAVE: " << std::right << std::setw(6) << node->amafPlayResult*1.0 / node->amafPlay <<
		"/" << std::left << std::setw(5) << node->amafPlay << "]\n";
	
	uctNode *children[MAX_BOARD2] = { 0 };
	int len = 0;

	for (int i = 0; i < node->nextMove.size(); ++i)
	{
		if (node->nextMove[i]->play > threshold)
			children[len++] = node->nextMove[i];
	}
	while (1)
	{
		int best = -1, visits = 0;
		for (int i = 0; i < len; ++i)
		{
			if (children[i] && children[i]->play>visits)
			{
				best = i;
				visits = children[i]->play;
			}
		}
		if (best < 0)break;
		if (children[best]->pos>0)
			show_node(children[best], threshold, depth + 1);
		children[best] = 0;
	}
}
void uctNode::show_node()
{
	int threshold = play / 300;
	show_node(this, threshold, 0);
}
