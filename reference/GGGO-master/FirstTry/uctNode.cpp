#include "uctNode.h"
#include <fstream>

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
//playresult only +
void uctNode::result(int r, bool* blackExist, bool* whiteExist)
{
	uctNode *p = this;
	uctNode * tmp;
	int color = p->color;
	while (p)
	{
		tmp = p;
		++(p->play);
		if (p->color == color)
			p->playResult += r;
		p = p->lastMove;
		//new
		if (!p)
			break;
		if (p->color == 1) //white
		{
			//child is black
			for (int i = 0; i < p->nextMove.size(); ++i)
			{
				if (blackExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
				{
					++(p->nextMove[i]->amafPlay);
					if(p->nextMove[i]->color == color)
						p->nextMove[i]->amafPlayResult += r;
				}
			}
		}
		else //black
		{
			//child is white
			for (int i = 0; i < p->nextMove.size(); ++i)
			{
				if (whiteExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
				{
					++(p->nextMove[i]->amafPlay);
					if (p->nextMove[i]->color == color)
						p->nextMove[i]->amafPlayResult += r;
				}
			}
		}

	}
}

//playResult can be -/+
//void uctNode::result(int r, bool* blackExist, bool* whiteExist)
//{
//	uctNode *p = this;
//	uctNode * tmp;
//	while (p)
//	{
//		tmp = p;
//		++(p->play);
//		p->playResult += r;
//		p = p->lastMove;
//		//new
//		if (!p)
//			break;
//		if (p->color == 1) //white
//		{
//			//child is black
//			for (int i = 0; i < p->nextMove.size(); ++i)
//			{
//				if (blackExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
//				{
//					++(p->nextMove[i]->amafPlay);
//					p->nextMove[i]->amafPlayResult += r;
//				}
//			}
//		}
//		else //black
//		{
//			//child is white
//			for (int i = 0; i < p->nextMove.size(); ++i)
//			{
//				if (whiteExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
//				{
//					++(p->nextMove[i]->amafPlay);
//					p->nextMove[i]->amafPlayResult += r;
//				}
//			}
//		}
//
//	}
//}