#include "Go.h"
#include <ctime>

#define TREESIZE 5000000
#define PLAYOUTS 30000

#define EXPAND 8
#define RESIGN_THRESHOLD 0.1
#define PASS_THRESHOLD 0.9


Go::Go(Board *_b):tree(TREESIZE,_b),amaf(b->get_size()){
	main_board = _b;
	max_playouts = PLAYOUTS;
	max_time = 3*CLOCK_PER_SEC;
	sTree_size = TREESIZE;
	rand_mvs = 0;
}

void
Go::reset(){
	rand_mvs = 0;
	tree.clear();
	amaf.Amaf_set(main_board->get_side(),main_board->get_size());
}

int
Go::play_random_simulation(){    //return 1 stands blacks wins
	int pass = 0;
	main_board->shuffle_emptys();
	while(pass<2){
		int mv = method? main_board->wiser_play():main_board->random_play();
		amaf.Amaf_play(mv,++simulen);
		++rand_mvs;
		if (move == PASS)++pass;
		else pass = 0;
	}

	int mercy = main_board->mercy();
	if (mercy != -1) return 1-mercy;
	if (simulen > 2*main_board->get_size_2()){
		++discarded;
		return -1;
	}

	return (main_board->count_score()>0)?1:0;
}


int 
Go::generate_move(){bool need_pass}{
	bool now_side = main_board->get_side();
	Node* now_root = tree->get_root();
	rand_mvs = 0;
	discarded = 0;
	tick_tak = clock();
	while(root->get_visits() < max_playouts && clock() - tick_tak < max_time){
		Node *node_hist[3*MAXSIZE_2];
		int num_node = 0,pass = 0;
		simulen = 0;
		amaf.Amaf_set(main_board->get_side(), main_board->get_size());
		Node *node = root;
		while(node->has_childs() && pass < 2){
			node_hist[num_node++] = node;
			node = node->uct_select();
			int mv = node->get_move();
			if (mv == PASS) ++pass;
			else pass = 0;
			main_board->play_move(mv);
			amaf.Amaf_play(mv,++simulen);
		}

		if (node->get_visits() >= EXPAND || node == root){
			Ppriority Pps[MAXSIZE_2+1] = {{0,0}};
			int legal_mvs[MAXSIZE_2+1];
			int n_mvs = main_board->legal_moves(legal_mvs);
			tree.expand(node, legal_mvs,n_mvs,Pps);
		}

		node_hist[num_node++] = node;
		int is_wins = play_random_simulation(WISER);
		main_board->restore(); // 这里可以并行？
		if (is_wins == -1) continue;
		if (now_side) is_wins = 1-is_wins;
		back_trace_ret(is_wins,node_hist,num_node,now_side);
	}

	Node *best_mv = tree.get_best();
	if (best_mv == PASS) return PASS;
	if (best_mv->get_value(1) < RESIGN_THRESHOLD) return -1;
	if (need_pass && best_mv->get_value(1) >= PASS_THRESHOLD 
		&& !now_root->get_move()) return PASS;
	return best_mv->get_move();

}


void 
Go::back_trace_ret(int ret, Node *node_hist[], int n_nodes, bool _side){
	for (int i=0; i<n_nodes; ++i){
		node_hist[i]->set_result(1-ret);
		node_hist[i]->set_amaf(ret,amaf,_side,i+1);
		side = !side;
		ret = 1-ret;
	}
}





