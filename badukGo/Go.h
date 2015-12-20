#ifndef __GO_H__
#define __GO_H__ 

#include "Board.h"
#include "STree.h"
#include <vector>

#define INF 1<<30;


class Go{
private:
	/* 默认树的规模 */
	int sTree_size;
	/* 最多模拟盘数 */
	int max_playouts;
	/* random走了几步 */
	int rand_mvs;
	/* 这个是给amaf用的，用来动态调参数的本来 */
	int simulen;
	/* 这个用来查看有哪些棋局是无效的 */
	int discarded;

	STree tree;
	Amaf amaf;
	Board *main_board;


	mutable clock_t max_time, tick_tak;
	/* 模拟对战 */
	int play_random_simulation(bool method);
	/* 回溯更新节点信息 */
	void back_trace_ret(int ret, Node *node_hist[], int n_nodes, bool _side);


public:
	Go(Board *_b);
	void reset();
	void set_max_playouts(int _playouts){max_playouts = _playouts;}
	/* 生成一步棋，need_pass还没领悟什么意思，不过一般是false */
	int generate_move(bool need_pass);
	/* 打分，有点像compute_final_status，待调整 */
	double score(std::vector<int> *v);
	/* move一步之后就要换树根 */
	void report_move(int mv){tree.change_root(mv);}
};

#endif