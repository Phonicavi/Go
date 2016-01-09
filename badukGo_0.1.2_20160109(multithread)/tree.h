#ifndef __TREE_H__
#define __TREE_H__
#include "amaf.h"
#include "Board.h"
#include "FastLog.h"
#include <cmath>


class Node{
private:
  int move;
  int visits, results;
  double r_visits, r_results;
  double p_visits, p_results;

  Node *child, *sibling;

public:
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
  Node *get_best_child() const;


};

class Tree{
private:
  Node *root[2]; 
  int size[2], maxsize, cur_root;
  const Board *board;
public:
  Tree(int maxsize, Board *board);
  ~Tree();
  void clear();
  void clear_cur_root();
  int promote(int new_root);
  Node *insert(Node *parent, int move, const Prior &prior);
  Node *insert(Node *parent, const Node *orig);
  void copy_all(Node *parent, const Node *orig);
  int expand(Node *parent, const int *moves, int nmoves, const Prior[]);
  Node *get_best() const{ return root[cur_root]->get_best_child(); }
  Node *get_root() const{ return root[cur_root]; }
  int get_size() const{ return size[cur_root];}
  void print() const;
  void print(Node *node, int threshold, int depth) const;
};
#endif
