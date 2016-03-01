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
#ifndef __TREE_H__
#define __TREE_H__
#include "amaf.h"
#include "Board.h"

#include <cmath>
#define RELATE_PRUNE
#ifdef RELATE_PRUNE
  #define BEGIN_PRUN 15
  #define END_PRUN 130
  #define AGRESSIVE_R_PRUN_ALPHA 0.8
  #define SOFT_R_PRUN_ALPHA 0.75
  #define LAZY_R_PRUN_ALPHA 0.7
  #define SOFT_THERESHOLD 0.75
  #define LAZY_THERESHOLD 0.8
  #define STOP_THERESHOLD 0.9
  #define R_PRUN_RAVE_DISCOUNT 0.99
#endif

#define CHANGE_BEST_POLICY

class Node{
private:
  int move;
  int visits, results;
  double r_visits, r_results;
  double p_visits, p_results;

  Node *child, *sibling;
  float node_score;
public:
  int is_expand;
  int is_pruned;

  void update_score(const float score){node_score += score;}
  float get_score() const {return node_score/visits;}
  void reset();
  void copy_values(const Node *orig);
  void add_child(Node *child);
  void set_move(int, const Prior &prior);
  void set_results(int result);
  void set_amaf(int result, const AmafBoard &amaf, bool side, int depth);
  void set_amaf(int result, AmafBoard * amaf, bool side, int depth);
  int get_move() const{ return move; }
  int get_results() const{ return results; };
  int get_visits() const{ return visits; };
  double get_r_results() const{ return r_results; };
  double get_r_visits() const{ return r_visits; };
  Node *get_child() const{ return child; }
  Node *get_sibling() const{ return sibling; }
  void print(int boardsize) const;

  bool has_childs() const{ return child != 0; };
  double get_value(int parent_visits) const;
  Node *select_child() const;
  Node *get_best_child(double winrate) const;
  Node *get_best_child() const;
  void set_child(Node *chd){child = chd;}
  void set_sibling(Node *sib){sibling = sib;}

  inline double get_r_prun_winrate(double alpha) const;
  void do_r_prun(int ttl_visits);

  


};

class Tree{
private:
  Node *root[2]; 
  int size[2], maxsize, cur_root;
  const Board *board;
public:
  int relative_prun_num;

  Tree(int maxsize, Board *board);
  ~Tree();
  void clear();
  void clear_cur_root();
  int promote(int new_root,int step);
  Node *insert(Node *parent, int move, const Prior &prior);
  Node *insert(Node *parent, const Node *orig);
  void copy_all(Node *parent, const Node *orig);
  int expand(Node *parent, const int *moves, int nmoves, const Prior[]);
  Node *get_best() const{ return root[cur_root]->get_best_child(); }
  Node *get_best(double winrate) const{ return root[cur_root]->get_best_child(winrate); }
  Node *get_root() const{ return root[cur_root]; }
  int get_size() const{ return size[cur_root];}
  void print(double winrate) const;
  void print(Node *node, int threshold, int depth) const;
  void do_r_prun(int ttl_visits, double alpha);

};
#endif
