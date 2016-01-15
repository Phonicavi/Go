#include "Board.h"
#include <fstream>
#include <assert.h>
#define komi_int 7

Board::Board(int newsize)
{
  size = (newsize <= MAXSIZE) ? newsize : size;
  size2 = size*size;
  komi = 0.5;
  last_point2 = 0;
  clear();
  init_four_side();
  init_four_corner();
  init_eight_around();
  init_distance();
  init_manhattan();
  GroupFather.clear();
  IS_REAL_GAME =0;
}

//store the current situation
void Board::backup(){
  side_bu = side;
  ko_point_bu = ko_point;
  size_bu = size, size2_bu = size2;
  komi_bu = komi;
  handicap_bu = handicap;
  empty_points_bu = empty_points;
  for (unordered_set<int>::iterator it = GroupFather.begin(); it != GroupFather.end(); ++it)
        groups_bu[*it] = groups[*it];

  stones_on_board_bu[1] = stones_on_board[1];
  stones_on_board_bu[0] = stones_on_board[0];
  last_point_bu = last_point, last_point2_bu = last_point2;
  last_atari_black_bu = last_atari[0]?last_atari[0]->get_father():0;
  last_atari_white_bu = last_atari[1]?last_atari[1]->get_father():0;
}

//resume the situation from backup
void Board::resume(){
  side = side_bu;
  ko_point = ko_point_bu;
  size = size_bu, size2 = size2_bu;
  komi = komi_bu;
  handicap = handicap_bu;

  for (int i = 0; i <= size2; i++) {
    belongedGroup[i] = 0;
  }
  last_atari[0] = 0;
  last_atari[1] = 0;
  for (unordered_set<int>::iterator it = GroupFather.begin(); it != GroupFather.end(); ++it){
        groups[*it] = groups_bu[*it];
        for (Group::StnItr st(&groups[*it]); st; ++st){
          belongedGroup[*st] = &groups[*it];
          if (*it == last_atari_black_bu) last_atari[0] = &groups[*it];
          else if (*it == last_atari_white_bu) last_atari[1] = &groups[*it];
        }
  }
  stones_on_board[1] = stones_on_board_bu[1];
  stones_on_board[0] = stones_on_board_bu[0];
  last_point_bu = last_point, last_point2_bu = last_point2;
  empty_points = empty_points_bu;
}

void Board::reset()
{
  side = BLACK;
  ko_point = 0;
  stones_on_board[BLACK] = 0, stones_on_board[WHITE] = 0;
  last_atari[BLACK] = 0, last_atari[WHITE] = 0;
  last_point = 0, last_point2 = 0;

  for (int i = 0; i <= size2; i++) {
    if (Group *point = belongedGroup[i]) {
      point->clear();
    }
    belongedGroup[i] = 0;
  }
  
  empty_points.clear();
  for (int j = 0; j < size2; j++) {
    empty_points.add(j+1);
  }
}

void Board::clear()
{
  reset();
  handicap = 0;
  game_history.clear();
}

void Board::restore()
{
  reset();
  for (int i = 0; i < game_history.length(); i++) {
    if (game_history[i]) {
      drop_stone(game_history[i], side);
      side = !side;
    } else {
      side = !side;
    }
  }
}

void Board::init_four_side()
{
  for (int k = 1; k <= size2; k++) {
    int nadj = 0;
    if (k <= size*(size-1)) {  //N
      four_side[k][nadj++] =  k + size;
    }
    if (k % size) {  //E
      four_side[k][nadj++] =  k + 1;
    }
    if (k > size) {  //S
      four_side[k][nadj++] = k - size;
    }
    if (k % size != 1) {  //W
      four_side[k][nadj++] = k -1;
    }
    four_side[k][nadj] = 0;
  }
}

void Board::init_four_corner()
{

  for (int k = 1; k <= size2; k++) {
    int ndiag = 0;
    if (k <= size*(size-1) && k % size != 1) {  //NW
      four_corner[k][ndiag++] =  k + size - 1;
    }
    if (k <= size*(size-1) && k % size) {  //NE
      four_corner[k][ndiag++] =  k + size + 1;
    }
    if (k > size && k % size) {  //SE
      four_corner[k][ndiag++] = k - size +1;
    }
    if (k > size && k % size != 1) {  //SW
      four_corner[k][ndiag++] = k - size -1;
    }
    four_corner[k][ndiag] = 0;
  }
}

//Precomputed 8 neighbours, to be used in pattern matching. Order matters.
void Board::init_eight_around()
{
  for (int i = 0; i <= size2; i++) {
    for (int j = 0; j < 16; j++) {
      eight_around[i][j] = 0;
    }
  }
  //Duplicate to calculate rotations,avoiding run-time modulo division:
  for (int i = 0; i < 2; i++) {
    for (int k = 1; k <= size2; k++) {
      if (k <= size*(size-1)) {
        if (k % size != 1) {  //NW
          eight_around[k][0+8*i] =  k + size - 1;
        }
        eight_around[k][1+8*i] =  k + size;  //N
      }
      if (k % size ) {
        if (k <= size*(size-1)) {  //NE
          eight_around[k][2+8*i] =  k + size + 1;
        }
        eight_around[k][3+8*i] =  k + 1;  //E
      }
      if (k > size) {
        if (k % size) {  //SE
          eight_around[k][4+8*i] = k - size +1;
        }
        eight_around[k][5+8*i] = k - size;  //S
      }
      if (k % size != 1) {
        if (k > size) {  //SW
          eight_around[k][6+8*i] = k - size -1;
        }
        eight_around[k][7+8*i] = k - 1;  //W
      }
    }
  }
}

void Board::init_distance()
{
  for (int p = 0; p <= size2; p++) {
    int d = (p-1)%size < (p-1)/size ? (p-1)%size : (p-1)/size;
    if (size - (p-1)%size - 1 < d) {
      d = size - (p-1)%size - 1;
    }
    if (size - (p-1)/size - 1 < d) {
      d = size - (p-1)/size - 1;
    }
    distance2edge[p] = d;
  }
}

void Board::init_manhattan()
{
  for (int p = 1; p <= size2; p++) {
    for (int dis = 1; dis <= 4; dis++) {
      int len = 0;
      for (int d = 0; d <= dis; d++) {
        if ((p-1)%size >= d && size-(p-1)/size-1 >= (dis-d) ) {
          within_manhattan[p][dis-1][len++] = p - d + (dis-d)*size;
        } else {
          within_manhattan[p][dis-1][len++] = 0;
        }
        if (d != dis) {
          if ((p-1)%size >= d && (p-1)/size >= (dis-d)) {
            within_manhattan[p][dis-1][len++] = p - d - (dis-d)*size;
          } else {
            within_manhattan[p][dis-1][len++] = 0;
          }
        }
        if (d != 0) {
          if (size-(p-1)%size-1 >= d && size-(p-1)/size-1 >= (dis-d)) {
            within_manhattan[p][dis-1][len++] = p + d + (dis-d)*size;
          } else {
            within_manhattan[p][dis-1][len++] = 0;
          }
          if (d != dis) {
            if (size-(p-1)%size-1 >= d && (p-1)/size >= (dis-d)) {
              within_manhattan[p][dis-1][len++] = p + d - (dis-d)*size;
            } else {
              within_manhattan[p][dis-1][len++] = 0;
            }
          }
        }
      }
    }
  }
}

int Board::set_size(int newsize)
{
  size = (newsize <= MAXSIZE) ? newsize : size;
  size2 = size*size;
  init_four_side();
  init_four_corner();
  init_eight_around();
  init_distance();
  init_manhattan();
  clear();
  
  return size;
}

inline void Board::remove_empty(int point)
{
  empty_points.remove(point);
}


int Board::drop_stone(int point, bool color)
{
  if (belongedGroup[point] != 0) 
    print_goban();

  PointSet<5> liberties;
  point_liberties(point, liberties);

  groups[point].set_up(point, color, liberties);
  belongedGroup[point] = &(groups[point]);
  if (IS_REAL_GAME)
	  GroupFather.insert(point);
  
  remove_empty(point);
  stones_on_board[color]++;

  ko_point = handle_neighbours(point);

  if (belongedGroup[point]->get_libs_num() == 0) {    //suicide.
      erase_neighbour(belongedGroup[point]);
  } else if (belongedGroup[point]->has_one_liberty()) { //self-atari.
    last_atari[color] = belongedGroup[point] ;
  }
  last_point2 = last_point;
  last_point = point;
  return point;
}

int Board::handle_neighbours(int point)
{
  int captured_lone = 0, ncapt_lone = 0;
  GroupSet<4> neighbours;
  Group * great_gr = belongedGroup[point];
  int nneigh = neighbour_groups(point, neighbours,great_gr);
  GroupSet<4> need_merge;
  need_merge.add(belongedGroup[point]);

  for (int i = 0; i < nneigh; i++) {
    Group *current_neigh = neighbours[i];
    if (current_neigh->get_color() == belongedGroup[point]->get_color()) {
      need_merge.add(current_neigh);
    } else {
      if (current_neigh->has_one_liberty()) {
        if (current_neigh->get_stones_num() == 1) {
          ncapt_lone++;
          captured_lone = current_neigh->get_the_stone(0);
        }
        erase_neighbour(current_neigh);
      } else {
        current_neigh->erase_liberties(point);
        if (current_neigh->has_one_liberty()) {
          last_atari[current_neigh->get_color()] = current_neigh;
        }
      }
    }
  }

  if (need_merge.length()>1)
    merge_neighbour(point,&need_merge,great_gr);

  if (ncapt_lone == 1 && belongedGroup[point]->get_stones_num()== 1) {
    return captured_lone;  //new ko point.
  } else {
    return 0;
  }
}

void Board::merge_neighbour(int point, Group *neigh)
{
  Group *group = belongedGroup[point];
  neigh->erase_liberties(point);
  group->attach_group(neigh);
  for (Group::StnItr st(neigh);  st; ++st) {
    belongedGroup[*st] = group;
  }
  if (IS_REAL_GAME){
	  unordered_set<int>::iterator it;
	  if ((it = GroupFather.find(neigh->get_father())) != GroupFather.end())
			GroupFather.erase(it);
  }
  
  neigh->clear();
  if (neigh == last_atari[neigh->get_color()]) {
    last_atari[neigh->get_color()] = 0;
    //merged group may still be in atari, but this case is handled in 'drop_stone()'.
  }
}

void Board::merge_neighbour(int point,GroupSet<4> * groupset, Group * Greatgroup){
    int len =groupset->length();
    Group *cur_gr;
    for (int i=0; i<len; ++i){
      cur_gr = groupset->get_by_index(i);
      cur_gr->erase_liberties(point);
      if (cur_gr!=Greatgroup){
        Greatgroup->attach_group(cur_gr);
        for (Group::StnItr st(cur_gr); st; ++st){
          belongedGroup[*st] = Greatgroup;
        }
        if (IS_REAL_GAME){
            unordered_set<int>::iterator it;
            if ((it = GroupFather.find(cur_gr->get_father())) != GroupFather.end())
              GroupFather.erase(it);
        }
        cur_gr->clear();
      }
      if (cur_gr == last_atari[cur_gr->get_color()]){
        last_atari[cur_gr->get_color()] =0;
      }
    }
}

void Board::erase_neighbour(Group *neigh)
{
  for (Group::StnItr st(neigh);  st; ++st) {
    belongedGroup[*st] = 0;
    stones_on_board[neigh->get_color()]--;
    empty_points.add(*st);

    GroupSet<4> meta_neigh;
    int nmeta = neighbour_groups(*st, meta_neigh);
    for (int k = 0; k < nmeta; k++) {
      meta_neigh[k]->add_liberties(*st);
      if (meta_neigh[k] == last_atari[meta_neigh[k]->get_color()]) {
        last_atari[meta_neigh[k]->get_color()] = 0;
      }
    }
  }

  if (neigh == last_atari[neigh->get_color()]) {
    last_atari[neigh->get_color()] = 0;
  }
  if (IS_REAL_GAME){
  unordered_set<int>::iterator it;
  if ((it = GroupFather.find(neigh->get_father())) != GroupFather.end())
        GroupFather.erase(it);
  }
  neigh->clear();
}

//this move isn't stored in game_history.
int Board::play_move(int point)
{
  if (point) 
    drop_stone(point, side);
  side = !side;
  
  return point;
}

int Board::play_move(int point, bool color)
{

  IS_REAL_GAME = 1;
  if (side != color)        //two consecutive moves of the same color are
    game_history.add(PASS); //represented by a pass inbetween.
  

  if (point > 0) {
    if (!is_occupied(point) && is_legal(point, color)) {
      drop_stone(point, color);
    } else {
      IS_REAL_GAME = 0;
      return  -1;
    }
  } else 
    ko_point = 0;
  
  side = !color;
  game_history.add(point);
  IS_REAL_GAME = 0;

  return point;
}

bool Board::set_position(const LList &moves)
{
  for (int i = 0; i < moves.length(); i++) {
    if (moves[i]) {
      if (!belongedGroup[moves[i]] && is_legal(moves[i], side)) {
        drop_stone(moves[i], side);
        game_history.add(moves[i]);
        side = !side;
      } else {
		   return false;
      }
    } else {
		side = !side;
    }
  }
  return true;
}

bool Board::set_position(const Board *original)
{
  return set_position(original->game_history);
}

int Board::point_liberties(int point, LList &liberties) const
{  
  // liberties must be of size > 4.
  for (int i = 0; int adj=four_side[point][i]; i++) {
    if (belongedGroup[adj] == 0) {
      liberties.add(adj);
    }
  }
  return liberties.length();
}

int Board::point_liberties(int point) const
{
  int nlibs = 0;
  for (int i = 0; four_side[point][i]; i++) {
    if (belongedGroup[four_side[point][i]] == 0) nlibs++;
  }
  return nlibs;
}

int Board::neighbour_groups(int point, GroupSet<4> &neighbours) const
{
  for (int i = 0; four_side[point][i]; i++) {
    neighbours.add(belongedGroup[four_side[point][i]]);
  }
  return neighbours.length();
}

int Board::neighbour_groups(int point, GroupSet<4> &neighbours, Group *Greatgroup) const
{
  int maxLen = 1;
  for (int i = 0; four_side[point][i]; i++) {
    neighbours.add(belongedGroup[four_side[point][i]]);
    if (belongedGroup[four_side[point][i]]->get_stones_num() > maxLen){
      maxLen = belongedGroup[four_side[point][i]]->get_stones_num();
      Greatgroup = belongedGroup[four_side[point][i]];
    }
  }
  return neighbours.length();
}

int Board::neighbour_groups(int point, bool color, int max_liberties,
                            GroupSet<MAXSIZE2/3> *neighbours) const
{
  int nneigh = 0;
  for (int i = 0; four_side[point][i]; i++) {
    Group *current_group = belongedGroup[four_side[point][i]];
    if (current_group && current_group->get_color() == color
                     && current_group->get_libs_num() <= max_liberties) {
      if (neighbours) neighbours->add(current_group);
      nneigh++;
    }
  }
  return nneigh;
}

int Board::neighbour_groups(const Group *group, bool color, int max_liberties,
                            GroupSet<MAXSIZE2/3> &neighbours) const
{
  for (Group::StnItr st(group); st; ++st) {
    neighbour_groups(*st, color, max_liberties, &neighbours);
  }
  return neighbours.length();
}

int Board::neighbours_size(int point, bool color) const
{
  int nstones = 0;
  GroupSet<4> neighbours;
  int nneigh = neighbour_groups(point, neighbours);
  for (int i = 0; i < nneigh; i++) {
    const Group *curr_neigh = neighbours[i];
    if (curr_neigh->get_color() == color) {
      nstones += curr_neigh->get_stones_num();
    }
  }
  return nstones;
}

int Board::neighbours_in_atari(int point, bool color, const GroupSet<4> &neighbours) const
{
  int natari = 0;
  for (int i = 0; i < neighbours.length(); i++) {
    if (neighbours[i]->get_color() !=color && neighbours[i]->has_one_liberty()) {
      natari++;
    }
  }
  return natari;
}

bool Board::is_surrounded(int point, bool color, int consider_occupied) const
{
  if (belongedGroup[point] != 0) return false;
  for (int i = 0; int adj=four_side[point][i]; i++) {
    if (adj == consider_occupied) continue;
    if (belongedGroup[adj] == 0 || belongedGroup[adj]->get_color() != color) {
      return false;
    }
  }
  return true;
}

bool Board::is_true_eye(int point, bool color, int consider_occupied) const
{
  int i, ncontrolled = 0;
  
  if (!is_surrounded(point, color, consider_occupied))
	  return false;
  
  for (i = 0; int diag=four_corner[point][i]; i++) {
    if (belongedGroup[diag]) {
      if (belongedGroup[diag]->get_color() == color) {
          ncontrolled++;
      }
    } else {
      if (is_surrounded(diag, color, consider_occupied)) {
        ncontrolled++;
      }
    }
  }
  if (i == 4) {
    if (ncontrolled > 2) return true;
  } else if (ncontrolled == i) {
    return true;
  }
  
  return false;
}

#define FALSE_EYES
#ifdef FALSE_EYES
bool Board::is_false_eye(int point, bool color) const
{
  if (!is_surrounded(point, color)) return false;
  int nopponent = 0;
  for (int i = 0; i < 4; i++) {
    if (int diag = four_corner[point][i]) {
      if (belongedGroup[diag] && belongedGroup[diag]->get_color() != color) {
        nopponent++;
      }
    } else {
      nopponent++;
      break;
    }
  }
  return nopponent < 2;
}

#else
bool Board::is_false_eye(int point, bool color) const
{
  if (!is_surrounded(point, color))
	  return false;
  for (int i = 0; int adj=four_side[point][i]; i++) {
    if (belongedGroup[adj]->has_one_liberty())
		return false;
  }
  
  return true;
}
#endif

bool Board::is_legal(int point, bool color) const
{  
  if (point == ko_point) return false;
  if (point_liberties(point) > 0) return true;

  GroupSet<4> neighbours;
  int nneigh = neighbour_groups(point, neighbours);
  for (int i = 0; i < nneigh; i++) {
    if (neighbours[i]->get_color() == color && !neighbours[i]->has_one_liberty()) {
      return true;
    }
  }
  return neighbours_in_atari(point, color, neighbours) > 0;
}

//Value 1 for Black, -1 for White, 0 for empty.
int Board::get_value(int point) const   
{
  if (belongedGroup[point]) {
    return belongedGroup[point]->get_color() ? -1:1;
  }
  
  return 0;
}

float Board::score_count() const
{
  int ret = 0;
  for (int i = 1; i <= size2; i++) {
    if (belongedGroup[i]) {
      ret += (-(belongedGroup[i]->get_color()<<1)+1);
    } else {
		  if (is_surrounded(i, BLACK)) ++ret;
		  else if (is_surrounded(i, WHITE)) --ret;
    }

  }
  
  return ret-komi;
}

 void Board::score_area(int point_list[]) const
{
  for (int i = 1; i <= size2; i++) {
    if (belongedGroup[i]) {
      if (belongedGroup[i]->get_color()) point_list[i]--;
      else point_list[i]++;
    } else {
      if (is_surrounded(i, WHITE)) point_list[i]--;
      else if (is_surrounded(i, BLACK)) point_list[i]++;
    }
  }
}

int Board::mercy() const
{
  const int v = size2/3;
  for (int s = 0; s < 2; s++) {
    if (stones_on_board[s] - stones_on_board[1-s] > v) {
      return s;
    }
  }
  
  return -1;
}

void Board::print_goban() const
{
  std::cerr << "   ";
  for (int i = 0; i<size; i++) std::cerr << "--";
  std::cerr << "\n";
  for (int y = size - 1; y > -1; y--) {
    std::cerr << "  |";
    for (int x = 1; x < size+1; x++) {
      if (belongedGroup[size*y + x]) {
        if (belongedGroup[size*y + x]->get_color()) std::cerr << "#";
        else std::cerr << "o";
        if (size*y + x == last_point) std::cerr << "!";
        else std::cerr << " ";
      } else {
          std::cerr << ". ";
      }
    }
    std::cerr << "|" << y+1 << "\n";
  }
  std::cerr << "   ";
  for (int i = 0; i<size; i++) std::cerr << "--";
  std::cerr << "\n  ";
  for (int i = 0; i<size; i++) std::cerr << " " << COORDINATES[i];
  std::cerr << "\nMoves: " << game_history.length() << " Side: "
      << side << " Komi: " << komi << " empty: " << empty_points.length();
  int lat1 = (last_atari[BLACK] ? last_atari[BLACK]->get_the_stone(0): 0);
  int lat2 = (last_atari[WHITE] ? last_atari[WHITE]->get_the_stone(0) : 0);
  std::cerr << " last atari: black " << lat1<< " white " << lat2 << "\n";
}


void Board::record_goban()
{
  ofstream ss("record.txt",ios::app);
  ss << "   ";
  for (int i = 0; i<size; i++) ss << "--";
  ss << "\n";
  for (int y = size - 1; y > -1; y--) {
    ss << "  |";
    for (int x = 1; x < size+1; x++) {
      if (belongedGroup[size*y + x]) {
        if (belongedGroup[size*y + x]->get_color()) ss  << "#";
        else ss  << "o";
        if (size*y + x == last_point) ss << "!";
        else ss << " ";
      } else {
          ss << ". ";
      }
    }
    ss  << "|" << y+1 << "\n";
  }
  ss  << "   ";
  for (int i = 0; i<size; i++)ss  << "--";
  ss << "\n  ";
  for (int i = 0; i<size; i++)ss << " " << COORDINATES[i];
  ss << "\nMoves: " << game_history.length() << " Side: "
      << side << " Komi: " << komi << " empty: " << empty_points.length();
  int lat1 = (last_atari[BLACK] ? last_atari[BLACK]->get_the_stone(0): 0);
  int lat2 = (last_atari[WHITE] ? last_atari[WHITE]->get_the_stone(0) : 0);
  ss << " last atari: black " << lat1<< " white " << lat2 << "\n";
  ss << "   ";
  for (int i = 0; i<size; i++) ss << "--";
  ss<< "\n";
    for (int y = size - 1; y > -1; y--) {
    ss << "  |";
    for (int x = 1; x < size+1; x++) {
      ss << (belongedGroup[size*y+x]?belongedGroup[size*y+x]->get_libs_num():0) << " ";
    }
    ss  << "|" << y+1 << "\n";
  }
  ss.close();
}

int  Board::print_libs_of(int point, int libs[])
{
  int fa = belongedGroup[point]->get_father();
  int num_lib = belongedGroup[fa]->get_libs_num();
  int cnt = 0;
  for (Group::LibItr lit(belongedGroup[fa]); lit; ++lit){
      libs[cnt++] = *lit;
  }
  
  return num_lib;
}

int  Board::print_group_of(int point, int st[])
{
  int fa = belongedGroup[point]->get_father();
  int num_st = belongedGroup[fa]->get_stones_num();
  int cnt = 0;
  for (Group::StnItr lit(belongedGroup[fa]); lit; ++lit){
      st[cnt++] = *lit;
  }
  
  return num_st;
}

int Board::first_legal_moves(int moves[],int last[],int flag,int cur_step) const
{
  int nlegal = 0;
  int star_point[4] = {127,49,43,121};

  if (!flag){   //flag == 0,不需要模拟,主要面向前几步
      int temp_board[MAXSIZE2 + 1] = {0};
      for (int i = 0; i < empty_points.length(); ++i)
         temp_board[empty_points[i]] = 1;

      if (cur_step == -1){
        for (int i = 0;i < 4; ++i)
            if(temp_board[star_point[i]] && is_legal(star_point[i],side)){
                moves[nlegal++] = star_point[i];
                break;
            }
      }

      else if(cur_step == 0){
          for (int i = 0; i < 4; ++i)
            if(star_point[i] == last[0]){
                if(temp_board[star_point[(i + 1)%4]])
                    moves[nlegal++] = star_point[(i + 1)%4];
                else if (temp_board[star_point[(i + 3)%4]])
                    moves[nlegal++] = star_point[(i + 3)%4];
                else{
                    for(int k = 0; k < 8; ++k)
                        if (temp_board[eight_around[star_point[(i + 1)%4]][k]])
                            moves[nlegal++] = eight_around[star_point[(i + 1)%4]][k];
                }
                break;
            }
      }else if(cur_step == 1){
          int mid_point = (last[0] + last[1]) / 2;
          for(int i = 0; i < 8; ++i)
            if (temp_board[eight_around[mid_point][i]])
                moves[nlegal++] = eight_around[mid_point][i];
      }
	  
      if(nlegal == 0){
          for (int i = 0; i < empty_points.length(); i++) {
            int point = empty_points[i];
            if (is_legal(point, side)) {
              moves[nlegal++] = point;
            }
          }
      }
  } // need simulation, mainly to the previous 20 steps
  else if (flag == 2){
    if(ONE_EXIST)
        return legal_moves(moves);
    for (int i = 0; i < empty_points.length(); i++) {
        int point = empty_points[i];
        if (board_map[point] != 1 && is_legal(point, side)) {
          moves[nlegal++] = point;
        }
      }
    moves[nlegal++] = PASS;
  }// need simulation, mainly to the previous 10 steps
  else if (flag == 1){
    if(TWO_EXIST)
        return legal_moves(moves);
    for (int i = 0; i < empty_points.length(); i++) {
        int point = empty_points[i];
        if (!board_map[point] && is_legal(point, side)) {
          moves[nlegal++] = point;
        }
      }
    moves[nlegal++] = PASS;
  }
  return nlegal;
}

