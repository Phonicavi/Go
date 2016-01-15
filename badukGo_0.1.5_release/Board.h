#ifndef __BOARD_H__
#define __BOARD_H__

#include "group.h"
#include "amaf.h"
#include <unordered_set>

using namespace std;

struct Prior{
  double prior;
  double equiv;
};

class Board{
private:
  static const bool BLACK = 0, WHITE = 1;
   unordered_set<int> GroupFather;

  bool IS_REAL_GAME = 0;

  bool side_bu;
  int ko_point_bu;
  int size_bu, size2_bu;
  float komi_bu;
  int handicap_bu;

  Group groups_bu[MAXSIZE2+1];
  EmptyPointSet<MAXSIZE2+1> empty_points_bu;
  int stones_on_board_bu[2];
  int last_point_bu, last_point2_bu;
  int last_atari_black_bu;
  int last_atari_white_bu;
  bool side;
  int ko_point;
  int size, size2;
  float komi;
  int handicap;
  
  Group *belongedGroup[MAXSIZE2+1]; 
  Group groups[MAXSIZE2+1];
  int stones_on_board[2];
  Group *last_atari[2];  
  int last_point, last_point2;

  int four_side[MAXSIZE2+1][5];
  int four_corner[MAXSIZE2+1][5];
  int eight_around[MAXSIZE2+1][16];
  int distance2edge[MAXSIZE2+1];
  int within_manhattan[MAXSIZE2+1][4][20];

  EmptyPointSet<MAXSIZE2+1> empty_points;
  PointList<3*MAXSIZE2> game_history;


  void init_four_side();
  void init_four_corner();
  void init_eight_around();
  void init_distance();
  void init_manhattan();
  void reset();

  //special positions:
  bool is_tengen(int point) const;
  bool is_star_point(int point) const;
  bool is_point_3_3(int point) const;
  bool is_point_5_5(int point) const;
  bool is_point_3_4(int point) const;
  bool is_point_4_5(int point) const;
  bool is_point_3_5(int point) const;
  bool is_point_4_6(int point) const;


  //4-neighbours iterating methods:
  int point_liberties(int point) const;
  int point_liberties(int point, LList &liberties) const;
  int neighbour_groups(int point, GroupSet<4> &neighbours) const;
  int neighbour_groups(int point, GroupSet<4> &neighbours, Group *Greatgroup) const;
  int neighbour_groups(int point, bool color, int max_liberties,
                       GroupSet<MAXSIZE2/3> *neighbours) const;
  int neighbour_groups(const Group *group, bool color, int max_liberties,
                       GroupSet<MAXSIZE2/3> &neighbours) const;
  int neighbours_size(int point, bool color) const;
  int neighbours_in_atari(int point, bool color, const GroupSet<4> &neighbours) const;
  
  //Playing methods:
  int drop_stone(int point, bool color);
  int handle_neighbours(int point);
  void merge_neighbour(int point, Group *neighbour);
  void merge_neighbour(int point,GroupSet<4> * groupset, Group * Greatgroup);
  void erase_neighbour(Group *neighbour);
  inline void remove_empty(int point);
  
  bool is_surrounded(int point, bool color, int consider_occupied=0) const;
  bool is_true_eye(int point, bool color, int consider_occupied=0) const;
  bool is_false_eye(int point, bool color) const;
  bool is_legal(int point, bool color) const;
  
  //Heuristics:
  bool is_stones_around(int, int) const;
  int get_total_liberties(int, bool, LList*, int, const Group*) const;
  int atari_escapes(const Group*, LList&) const;
  bool grow_liberties(int, const Group*) const;
  bool is_self_atari(int, bool) const;
  int atari_last_liberty(int, bool) const;
  bool is_ladder(int, bool) const;
  int creates_eyes(int,bool) const;

  int random_choose(const LList&, bool(Board::*)(int, bool) const) const;
  bool random_method(int, bool) const;
  bool wiser_method(int, bool) const;
  int last_atari_trick() const;
  void nakade_trick(int, LList&) const;
  void capture_trick(int, LList&) const;
  void save_trick(int, LList&) const;
  void pattern_trick(int, LList&) const;
  bool is_match_mogo_pattern(int,bool) const;
  
public:
  static const int PASS = 0;
  Board(int size = 13);
  int get_history_length()const {return game_history.length();}

  int get_empty_length(){return empty_points.length();}
  void clear();
  void restore();
  void set_komi(float newkomi) { komi = newkomi; }
  int set_size(int new_size);
  bool set_position(const LList &moves);
  bool set_position(const Board *original);

  void shuffle_empty() { empty_points.shuffle(); }
  int play_move(int point);
  int play_move(int point, bool color);
  int play_random();
  int play_wiser();
  
  float get_komi() const { return komi; }
  bool get_side() const { return side; }
  int get_size() const { return size; }
  int get_size2() const { return size2; }
  int get_last_point() const { return last_point; }
  bool is_occupied(int point) const { return belongedGroup[point] != 0; }
  int get_value(int point) const;  
  int legal_moves(int moves[]) const;
  int legal_moves_origin(int moves[]) const;
  float score_count() const;
  void score_area(int point_list[]) const;
  int mercy() const;
  void init_priors(Prior priors[]) const;
  void print_goban() const;
  void record_goban();

  int  print_libs_of(int point, int libs[]);
  int  print_group_of(int point, int st[]);


  void backup();
  void resume();
  void copy_from(Board *board);
  int first_legal_moves(int moves[],int last[],int flag,int cur_step) const;
  int get_last2() {if(!last_point2) last_point2 = 127; return last_point2;}

};

#endif
