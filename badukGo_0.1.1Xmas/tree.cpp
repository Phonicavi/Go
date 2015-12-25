#include "tree.h"
#include <cmath>

void Node::reset()
{
  child = 0;
  sibling = 0;
  visits = 0;
  results = 0;
  r_visits = 0;
  r_results = 0;
  move = 0;
}

void Node::copy_values(const Node *orig)
 {
  child = 0;
  sibling = 0;
  visits = orig->visits;
  results = orig->results;
  r_visits = orig->r_visits;
  r_results = orig->r_results;
  p_visits = orig->p_visits;
  p_results = orig->p_results;
  move = orig->move;
 }

void Node::add_child(Node *child)
{
  if (!this->child)  this->child = child;
  else {
    Node *next;
    for (next = this->child; next->sibling; next = next->sibling) {
    }
    next->sibling = child;
  }
}

void Node::set_move(int mv, const Prior &prior)
{
  move = mv;
  r_results = prior.prior;
  r_visits = prior.equiv;
  p_results = prior.prior;
  p_visits = prior.equiv;
}

void Node::set_results(int result)
{
  visits++;
  results += result;
}

void Node::set_amaf(int result, const AmafBoard &amaf, bool side, int depth)
{
  const double discount = 0; //0.0001;
  for (Node *next = child; next; next = next->sibling) {
    next->r_results += result * amaf.value(next->move, depth, side, discount);
    next->r_visits += amaf.value(next->move, depth, side, discount);
  }
}

double Node::get_value(double parent_visits) const
{
  const double BIAS = 1.0/3000, UCTK = 0.0;
  if (visits) {
    if (r_visits) {
      double beta = r_visits/(r_visits + visits + r_visits*visits*BIAS);
      return (1.0-beta)*results/visits + beta*r_results/r_visits;
    } else {
      return results/visits;
    }
  } else if (r_visits) {
      return r_results/r_visits;
  }
  return 1.0;   //first player urgency.
}

Node *Node::select_child() const
{
  double best = -100,tmp;
  Node *selected = 0;
  for (Node *next = child; next; next = next->sibling){
    if ((tmp = next->get_value(visits)) > best || tmp == best && rand()&1){
      best = tmp;
      selected = next;
    }
  }
  return selected;
}

Node *Node::get_best_child() const
{
  //return select_child();
    Node *best = NULL;
  double best_v = 0;
  double best_w;
  double cur_v;
  double cur_w;
  for (Node * next = child; next; next = next->sibling){
    cur_v = next->r_visits+next->visits;
    cur_w = next->r_results + results;
    if (cur_v > best_v || cur_v == best_v && cur_w > best_w){
      best_v = cur_v;
      best_w = cur_w;
      best = next;
    }else if (cur_v == best_v && best_w == cur_w && rand()&1){
      best = next;
    }
  }
  return best;
}

void Node::print(int boardsize) const
{
  std::string mv;
  coord_to_char(move, mv, boardsize);
    std::cerr.precision(4);
    std::cerr << std::right << std::setw(4) << mv
     << ": " << std::right << std::setw(6) << results/visits
     << "/" << std::left << std::setw(5) << visits
     << "[ RAVE: " << std::right<< std::setw(6) << r_results/r_visits
     << "/" << std::left << std::setw(5) << r_visits
     << " Prior: " << std::right << std::setw(6) << p_results/p_visits
     << "/" << std::left << std::setw(5) << p_visits << "]\n";
}

Tree::Tree(int maxsize, Board *board)
{
  cur_root = 0;
  this->maxsize = maxsize;
  this->board = board;
  root[0] = new Node[maxsize];
  root[1] = new Node[maxsize];
  root[0]->reset();
  root[1]->reset();
  size[0] = 1;
  size[1] = 1;
}

Tree::~Tree()
{
  delete[] root[0];
  delete[] root[1];
}

void Tree::clear()
{
  cur_root = 0;
  root[0]->reset();
  root[1]->reset();
  size[0] = 1;
  size[1] = 1;
}

void Tree::clear_cur_root()
{
  size[cur_root] = 1;
  root[cur_root]->reset();
}

Node *Tree::insert(Node *parent, int move, const Prior &prior)
{
  if (size[cur_root] < maxsize) {
    Node *child = root[cur_root] + size[cur_root]++;
    child->reset();
    parent->add_child(child);
    child->set_move(move, prior);
    return child;
  } else {
    std::cerr << "WARNING: Out of memory!\n";
    return 0;
  }
}

Node *Tree::insert(Node *parent, const Node *orig)
{
  if (size[1-cur_root] < maxsize) {
    Node *child = root[1-cur_root] + size[1-cur_root]++;
    child->copy_values(orig);
    parent->add_child(child);
    return child;
  } else {
    std::cerr << "WARNING: Out of memory!\n";
    return 0;
  }
}

void Tree::copy_all(Node *parent, const Node *orig)
{
  for (Node *node = orig->get_child(); node; node = node->get_sibling()) {
    Node *child = insert(parent, node);
    if (child) copy_all(child, node);
  }
}

int Tree::promote(int new_root)
{

  clear();
  return cur_root;
}

int Tree::expand(Node *parent, const int *moves, int nmovs, const Prior priors[])
{
  //if (parent->has_childs()) return 0; //No need to expand, but we check before calling.
  for (int i = 0; i < nmovs; i++) {
    if (insert(parent, moves[i], priors[moves[i]]) == 0) {  //Out of memory.
      return -1;
    }
  }
  return 0;
}

void Tree::print(Node *node, int threshold, int depth) const
{
  for (int i = 0; i < depth; i++) std::cerr << "  ";
  std::cerr << "|->";
  node->print(board->get_size());
  
  Node *childs[MAXSIZE2] = {0};
  int len = 0;
  for (Node *child = node->get_child(); child; child=child->get_sibling()) {
    if (child->get_visits() >= threshold) childs[len++] = child;
  }

  while (true) {
    int best = -1, visits = 0;
    for (int i = 0; i < len; i++) {
      if (childs[i] && childs[i]->get_visits() > visits) {
        best = i;
        visits = childs[best]->get_visits();
      }
    }
    if (best < 0) break;
    if (childs[best]->get_move()) print(childs[best], threshold, depth+1);
    childs[best] = 0;
  }
}

void Tree::print() const
{
  int threshold = root[cur_root]->get_visits()/30;
  std::string sp = "  ";
  std::cerr << "Active tree: " << cur_root << " " << get_size() <<" nodes.\n";
  print(root[cur_root], threshold, 0);

  Node *best = get_best();
  std::cerr << "#Expected: " << best->get_value(1) << ", visits "
            << best->get_visits() << " PV:\n";

  bool side = !board->get_side();
  for (Node *n = root[cur_root]->get_best_child(); n; n = n->get_best_child()) {
      side ? std::cerr << "B" :  std::cerr << "W";
      n->print(board->get_size());
      side = !side;
  }
  std::cerr << "\n";
}
