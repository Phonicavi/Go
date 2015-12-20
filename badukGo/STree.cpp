#include "STree.h"

void
Node::reset(){
	move = 0;
	r_visits = r_results = visits = results = 0;
	childs = 0;
	siblings = 0;
}

void
Node::copy_value_from(Node *src){
	move = src->move;
	r_results = src->r_results;
	r_visits = src->r_visits;
	visits = src->visits;
	results = src->results;
	childs = 0;
	siblings = 0;
}

void
Node::add_child(Node *_child){
	if (this->childs == 0){
		this->childs = _child;
	}else{
		Node *next = this->childs;
		for (;next->siblings;next = next->siblings);
		next->siblings = _child;
	}
}

void
Node::set_amaf(int ret, const Amaf &amaf, bool _side, int _depth){
	const double discount = 0;
	double a;
	for (Node *next = childs; next; next = next->siblings){
		next->r_results += ret*(a = amaf.Amaf_value(next->move, _depth,_side,discount));
		next->r_visits += a;
	}
}


double
Node::get_value(double parent_visits){
	double RAVE_EQUIV = 2200;
	double UCT_K = 0.0 //0.44?
	if (visits){
		if (r_visits){
			double beta = r_visits/(r_visits+visits+r_visits*visits/RAVE_EQUIV);
			return (1.0-beta)*results/visits+beta*r_results/r_visits;
		}else{
			return results/visits + UCT_K*sqrt(log(parent_visits)/visits);
		}
	}else if(r_visits) return r_results/r_visits;
	return 1.0;
}

Node*
Node::uct_select(){
	double best = -100,tmp;
	Node *selected = 0;
	for (Node *next = childs; next; next = next->siblings){
		if ((tmp = next->get_value(visits)) > best){
			best = tmp;
			selected = next;
		}
	}
	return best;
}

Node*
Node::get_best_child(){
	Node *best = NULL;
	double best_v = 0;
	double best_w;
	double cur_v;
	double cur_w;
	for (Node * next = child; next; next = next->siblings){
		cur_v = next->r_visits+next->visits;
		cur_w = next->r_results + results;
		if (cur_v > best_v || cur_v == best_v && cur_w > best_w){
			best_v = cur_v;
			best_w = cur_w;
			best = next;
		}else if {cur_v == best_v && best_w == cur_w && rand()^1}{
			best = next;
		}
	}
	return best;
}



//============Tree==================

STree::STree(int max_size, Board *_board){
	this->max_sz = max_size;
	this->board = _board;
	cur_root = 0;
	root[0] = new Node[max_size];
	root[1] = new NOde[max_size];
	root[0]->reset();
	root[1]->reset();
	size[0] = 1;
	size[1] = 1;
}

STree::~STree(){
	delete [] root[0];
	delete [] root[1];
}

void
STree::clear(){
	cur_root = 0;
	root[0]->reset();
	root[1]->reset();
	size[0] = 1;
	size[1] = 1;
}


void 
STree::clear_current(){
	size[cur_root] = 1;
	root[cur_root]->reset();
}

Node *
STree::insert(Node *pa, int mv, Ppriority &Pp){
	if (size[cur_root] < max_sz){
		Node *child = root[cur_root] + size[cur_root]++;
		child->reset();
		pa->add_child(child);
		child->set_move(move);
		return child;
	}else{
		std::cerr<<"Error:out of memory!"<< std::endl;
		return 0;
	}
}

Node *
STree::insert(Node *pa, Node *orig){
	if (size[cur_root] < max_sz){
		Node *child = root[cur_root] + size[cur_root]++;
		pa->add_child(child);
		child->copy_value_from(orig);
		return child;
	}else{
		std::cerr<<"Error:out of memory!"<< std::endl;
		return 0;
	}
}

void
STree::copy_all(Node *pa, Node *orig){
	for (Node *node = orig->get_childs(); node; node = node->get_siblings()){
		Node *child = insert(pa,node);
		if (child) copy_all(child,node);
	}
}

int 
STree::change_root(int new_root_mv){
	for (Node *node = root[cur_root]->get_childs();node;node = node->get_siblings()){
		if (node->get_move() == new_root_mv){
			root[!cur_root]->copy_value_from(node);
			copy_all(root[!cur_root],node);
			clear_current();
			cur_root = !cur_root;
			return cur_root;
		}
	}

	clear();
	return cur_root;
}

int
STree::expand(Node *pa, int *moves, int num , Ppriority &Pps[]){
	for (int i=0; i<num;++i){
		if (insert(pa,moves[i],Pps[moves[i]]) == 0){
			return -1;  // Out of mem
		}
	}
	return 0;
}






