/**********************************************************************************************
* Copyright (C) 2016 <BadukGo Project>                                                        *
* All rights reserved                                                                         *
*                                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a copy of             *
* this software and associated documentation files (the “Software”), to deal in the Software  *
* without restriction, including without limitation the rights to use, copy, modify, merge,   *
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons  *
* to whom the Software is furnished to do so, subject to the following conditions:            *
*                                                                                             *
* The above copyright notice and this permission notice shall be included in all copies or    *
* substantial portions of the Software.                                                       *
*                                                                                             *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,         *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR    *
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR        *
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER      *
* DEALINGS IN THE SOFTWARE.                                                                   *
**********************************************************************************************/
#include "tree.h"
#include <cmath>
const double BIAS = 0.00033333333333, UCTK = 0.3, K=2000.0;
const double discount = 0.0;
#ifdef CHANGE_BEST_POLICY
  const double CHANGE_POLICY_WINRATE_THERESHOLD = 0.93;
  const double CHANGE_POLICY_WINRATE_THERESHOLD2 = 0.30;
  const double CHANGE_POLICY_WINRATE_DIFF_THERESHOLD = 0.99;
  const double CHANGE_POLICY_VISIT_DIFF_THERESHOLD = 0.3;
  const double CHANGE_POLICY_R_VISIT_DIFF_THERESHOLD = 0.35;
#endif 

FastLog flog(10);

void Node::reset()
{
  child = 0;
  sibling = 0;
  visits = 0;
  results = 0;
  r_visits = 0;
  r_results = 0;
  move = 0;
  is_expand = 0;
  is_pruned = 0;
  node_score = 0;
}

void Node::copy_values(const Node *orig)
 {
  child = 0;
  sibling = 0;
  is_expand = 0;
  is_pruned = orig->is_pruned;
  visits = orig->visits;
  results = orig->results;
  r_visits = orig->r_visits;
  r_results = orig->r_results;
  p_visits = orig->p_visits;
  p_results = orig->p_results;
  move = orig->move;
  node_score = orig->node_score;
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
  for (Node *next = child; next; next = next->sibling) {
    next->r_results += result * amaf.value(next->move, depth, side, discount);
    next->r_visits += amaf.value(next->move, depth, side, discount);
  }
}
void Node::set_amaf(int result, AmafBoard * amaf, bool side, int depth)
{
  for (Node *next = child; next; next = next->sibling) {
    next->r_results += result * amaf->value(next->move, depth, side, discount);
    next->r_visits += amaf->value(next->move, depth, side, discount);
  }
}

double Node::get_value(int parent_visits) const
{
  if(is_pruned != 0) return -1;

  if (visits) {
    if (r_visits) {
     double beta = r_visits/(r_visits + visits + r_visits*visits*BIAS);
      return (1.0-beta)*results/visits + beta*r_results/r_visits;
    } else {
      return ((double)results)/visits;
    }
  } 
  else if (r_visits) {
    
      return r_results/r_visits;
  }
  return (rand()*1000+10000);
}

Node *Node::select_child() const
{
  double best = -100,tmp;
  int bestnum = 0;
  Node *bestarray[MAXSIZE2];
  for (Node *next = child; next; next = next->sibling){
    if ((tmp = next->get_value(visits)) > best){
      best = tmp;
      bestnum = 1;
      bestarray[0] = next;
    }else if(tmp == best){
      bestarray[bestnum++] = next;
    }
  }
  return bestnum == 1?bestarray[0]:(bestarray[rand()%bestnum]);
}

Node *Node::get_best_child() const
{
  Node *best = NULL;
  double best_v = 0;
  double best_w;
  double cur_v;
  double cur_w;
  for (Node * next = child; next; next = next->sibling){
    cur_v = next->r_visits+next->visits;
    cur_w = next->r_results + results;
    if (cur_v > best_v || (cur_v == best_v && cur_w > best_w)){
      best_v = cur_v;
      best_w = cur_w;
      best = next;
    }
  }
  return best;
}

Node *Node::get_best_child(double winrate) const
{
  Node *best = NULL;
  double best_v = 0;
  double best_w;
  double cur_v;
  double cur_w;
  for (Node * next = child; next; next = next->sibling){
    cur_v = next->r_visits+next->visits;
    cur_w = next->r_results + results;
    if (cur_v > best_v || (cur_v == best_v && cur_w > best_w)){
      best_v = cur_v;
      best_w = cur_w;
      best = next;
    }
  }
#ifdef CHANGE_BEST_POLICY
    if (winrate < CHANGE_POLICY_WINRATE_THERESHOLD 
      && winrate > CHANGE_POLICY_WINRATE_THERESHOLD2) return best;
  #ifdef STD_ERR_PRINT
    std::cerr << "Winrate: " << winrate << std::endl;
  #endif
    Node *pre_best  =best;
    
    const double best_visit = (double)best->visits;
    const double best_r_visit = best->r_visits;
    const double best_winrate = ((double)best->results/best_visit);
    bool flag = 0;
    float best_score = best_visit>0?(best->node_score/best_visit):0,tmp_score;
  #ifdef STD_ERR_PRINT
    std::cerr << "best_visit: " << best_visit << " best_winrate: "<<best_winrate<<" best_score: "<<best_score<<std::endl;
  #endif
    for(Node *next = child; next; next = next->sibling){
      if (next == pre_best || next->move == 0) continue;
      if ((next->visits > CHANGE_POLICY_VISIT_DIFF_THERESHOLD*best_visit)
        && ((tmp_score = (next->node_score/next->visits)) > best_score)
        && (next->r_visits > best_r_visit*CHANGE_POLICY_R_VISIT_DIFF_THERESHOLD)
        &&(((double)next->results)/next->visits>best_winrate*CHANGE_POLICY_WINRATE_DIFF_THERESHOLD))
      {
      #ifdef STD_ERR_PRINT
          std::cerr << "NOW_BEST: ";
          std::string _mv;
          coord_to_char(next->move,_mv,13);
          std::cerr << _mv<<": " << next->visits << " " <<((double)next->results)/next->visits<<" "<<tmp_score<<std::endl;
      #endif 
          flag = 1;
          best = next;
          best_score =tmp_score;
      }
    }
  #ifdef STD_ERR_PRINT
    if (flag) {
      std::cerr << "CHANGE_BEST_BY_POLICY!" << std::endl;
      std::cerr << "PRE_BEST: ";
      std::string mv;
      coord_to_char(pre_best->move,mv,13);
      std::cerr<<mv<<"\n";
      mv = "";
      std::cerr << "NOW_BEST: ";
      coord_to_char(best->move,mv,13);
      std::cerr << mv<<"\n";

    }
  #endif

#endif
  return best;
}

double Node::get_r_prun_winrate(double alpha) const
{
  return visits?(1- alpha*(1-(double(results))/(visits))):(
          r_visits?(1- alpha*(1-R_PRUN_RAVE_DISCOUNT*r_results/r_visits)):0
          );
}

void Tree::do_r_prun(int ttl_visits,double alpha)
{
   Node *root = get_root();
   double miu[MAXSIZE2];
   double max_miu = -1.0;
   int max_miu_id = -1;
   int max_visits = -1;
   int n_childs = 0;
   for (Node *next = root->get_child();next;next = next->get_sibling()){
     miu[n_childs] = next->get_r_prun_winrate(alpha);
     if (next->get_visits() > max_visits){
      max_miu_id = n_childs;
      max_visits = next->get_visits();
     }
     ++n_childs;
   }
  #ifdef STD_ERR_PRINT
    std::cerr << "num_child: " << n_childs << std::endl;
  #endif
    max_miu = miu[max_miu_id];
    int i=0;
    Node *pre = 0;

   for (Node *next = root->get_child();next;next = next->get_sibling(),++i){
    if (next->is_pruned) {
      pre = next;
      continue;
      }
      if (max_miu > miu[i] && next->get_visits() + 8/(pow(max_miu-miu[i],2)*log(ttl_visits)) < max_visits){
        next->is_pruned = 1;
        ++relative_prun_num;
        if (pre != 0){
          pre->set_sibling(next->get_sibling());
          continue;
        }
         
      }
      pre = next;
   }
}



void Node::print(int boardsize) const
{
#ifdef STD_ERR_PRINT
  std::string mv;
  coord_to_char(move, mv, boardsize);
    std::cerr.precision(4);
    std::cerr << std::right << std::setw(4) << mv
     << " score: " << std::left << std::setw(6) << ((double)node_score)/visits
     << ": " << std::right << std::setw(6) << ((double)results)/visits
     << "/" << std::left << std::setw(5) << visits
     << "[ RAVE: " << std::right<< std::setw(6) << r_results/r_visits
     << "/" << std::left << std::setw(5) << r_visits
     << " Prior: " << std::right << std::setw(6) << p_results/p_visits
     << "/" << std::left << std::setw(5) << p_visits << "]\n";
#endif
}

Tree::Tree(int maxsize, Board *board)
{
  cur_root = 0;
  relative_prun_num = 0;
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
  relative_prun_num = 0;
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
  for (int i = 0; i < nmovs; i++) {
    if (insert(parent, moves[i], priors[moves[i]]) == 0) {
      return -1;
    }
  }
  return 0;
}

void Tree::print(Node *node, int threshold, int depth) const
{
#ifdef STD_ERR_PRINT
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
#endif
}

void Tree::print(double winrate) const
{
#ifdef STD_ERR_PRINT
  int threshold = root[cur_root]->get_visits()/30;
  std::string sp = "  ";
  std::cerr << "Active tree: " << cur_root << " " << get_size() <<" nodes.\n";

  print(root[cur_root], threshold, 0);

  Node *best = get_best(winrate);
  std::cerr << "#Expected: " << best->get_value(1) << ", visits "
            << best->get_visits() << " PV:\n";

  bool side = !board->get_side();
  for (Node *n = root[cur_root]->get_best_child(winrate); n; n = n->get_best_child()) {
      side ? std::cerr << "B" :  std::cerr << "W";
      n->print(board->get_size());
      side = !side;
  }

  std::cerr << "\nscore:\n" ;

  for (Node *n = root[cur_root]->get_child(); n ; n=n->get_sibling()){
      n->print(board->get_size());
  }
  std::cerr << "\n";
#endif
}

