#ifndef __STREE_H__
#define __STREE_H__ 

#include "Board.h"

class Node{
private:
	int move;
	/* 纯uct结果 */
	double visits, results;
	/* rave结果 */
	double r_visits, r_results;
	/* 树采用儿子兄弟链保存 */
	Node * childs, *siblings;

public:
	Node(){childs = NULL, siblings = NULL;}
	~Node(){};
	void reset();
	/* 拷贝src的信息，不拷贝儿子兄弟指针 */
	void copy_value_from(Node *src);
	/* 为当前节点增加儿子 */
	void add_child(Node *child);
	/* 设置当前节点信息 */
	void set_move(int mv){move = mv;}
	void set_result(int ret){++visits; results += ret;}
	void set_amaf(int ret, const Amaf &amaf, bool _side, int _depth);

	/* 返回当前节点信息 */
	int get_move(){return move;}
	double get_results(){return results;}
	double get_r_results(){return r_results;}
	double get_visits(){return visits;}
	double get_r_visits(){return r_visits;}
	Node *get_childs(){return childs;}
	Node *get_siblings(){return siblings;}

	bool has_childs(){return childs != 0;}
	double get_value(double);

	/* 选择应该展开哪个节点 */
	Node *uct_select();

	/* 选择最佳节点，genmove用的 */
	Node *get_best_child();


};


class STree{
private:
	/* 两个树根轮流用 */
	Node *root[2];
	int size[2],cur_root;
	int max_sz;

	const Board *board;

public:
	STree(int maxsize, Board *_board);
	~STree();
	/* 返回最佳节点 */
	Node *get_best(){return root[cur_root]->get_best_child();}
	/* 返回当前树根 */
	Ndoe *get_root(){return root[cur_root];}
	/* 返回当前树规模 */
	int get_size(){return size[cur_root];}





	void clear();
	void clear_current();
	/* 换一个树根， 将当前root的一个儿子节点变成另一个root的根 */
	int change_root(int new_root_mv);
	/* 在pa上插入儿子节点 */
	Node *insert(Node *pa, int mv, Ppriority &Pp);
	/* 在pa上插入儿子节点，信息来源于orig */
	Node *insert(Node *pa, Node *orig);

	/* 递归拷贝，把orig整个拷贝给pa */
	void copy_all(Node *pa, Node *orig);
	/* 展开节点，通过moves数组 */
	int expand(Node *pa, int *moves, int num, Ppriority &Pps[]);


};

#endif