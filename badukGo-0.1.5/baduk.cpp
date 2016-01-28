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
#include "Board.h"

int Board::random_choose(const LList &list, bool(Board::*Play)(int, bool) const) const
{
  if (list.length() == 0) return 0;
  int i = rand() % list.length();
  int cnt = 0;
  for ( ; (++cnt)<20; i=rand() % list.length() ) {
    int point = list[i];
    if ((this->*Play)(point, side)) {
      return point;
    }
  }
  for (int j = i; j<list.length();++j) {
    int point = list[j];
    if ((this->*Play)(point, side)) {
      return point;
    }
  } 
  

  for (int j = 0; j < i; ++j) {
    int point = list[j];
    if ((this->*Play)(point, side)) {
      return point;
    }
  }
  return PASS;
}

bool Board::random_method(int point, bool side) const
{  
  if (is_false_eye(point, side)) return false;
  if (!is_legal(point, side)) return false;
  return true;
}

bool Board::wiser_method(int point, bool side) const
{  
  if (is_false_eye(point, side)) return false;
  if (!is_legal(point, side)) return false;
  if (is_self_atari(point, side)) return false;
  if (is_ladder(point,side)) return false;
  return true;
}

int Board::play_random()
{
  return play_move(random_choose(empty_points, &Board::random_method));
}

int Board::play_wiser()
{
  if (int move = last_atari_trick()) {
    return play_move(move);
  }


  if (last_point) {
    PointSet<MAXSIZE2> list;
    nakade_trick(last_point, list);
    if (int move = random_choose(list, &Board::wiser_method)) {
      return play_move(move);
    }
    list.clear();
    save_trick(last_point, list);
    if (int move = random_choose(list, &Board::wiser_method)) {
      return play_move(move);
    }
   
    list.clear();
    pattern_trick(last_point, list);
    if (int move = random_choose(list, &Board::wiser_method)) {
      return play_move(move);
    }
    list.clear(); 
    #define CAPTURE_HEURISTICS 
  #ifdef CAPTURE_HEURISTICS  
    list.clear();
    capture_trick(last_point, list);
    if (int move = random_choose(list, &Board::wiser_method)) {
      return play_move(move);
    }
  #endif
  }


  return play_random();
}

int Board::last_atari_trick() const
{
  if (Group *group = last_atari[!side]) {
    int move = group->get_the_lib(0);
    if (is_legal(move, side) &&
        !is_self_atari(move, side) &&
        grow_liberties(move, group)) {
      return move;
    }
  }
  
  PointList<MAXSIZE2> save;
  if (last_atari[side]) {
    atari_escapes(last_atari[side], save);
    if (int move = random_choose(save, &Board::wiser_method)) {
      return move;
    }
  }
  return 0;
}

void Board::nakade_trick(int point, LList &list) const
{
  for (int i = 0; i < 8; i++) {
    if (int v = eight_around[point][i]) {
      if (belongedGroup[v] == 0 && (creates_eyes(v, 0) > 1 || creates_eyes(v, 1) > 1)) {
        list.add(v);
      }
    }
  }
}

void Board::capture_trick(int point, LList &list) const
{
  for (int i = 0; i < 8; i++) {
    Group *group = belongedGroup[eight_around[point][i]];
    if (group && group->get_color() != side && group->has_one_liberty()) {
      int lib = group->get_the_lib(0);
      if (grow_liberties(lib, group)) {
          list.add(lib);
      }
    }
}
}

void Board::save_trick(int point, LList &list) const
{
  if (belongedGroup[point] && belongedGroup[point]->get_libs_num() == 2) {
    for (int i = 0; i < 2; i++) {
      int lib = belongedGroup[point]->get_the_lib(i);
      if (grow_liberties(lib, belongedGroup[point])) {
        list.add(lib);
      }
    }
  }
  GroupSet<4> neigh;
  int nneigh = neighbour_groups(point, neigh);
  for (int j = 0; j < nneigh; j++) {
    if (neigh[j]->get_color() == side && neigh[j]->get_libs_num() == 2) { // <2?
      atari_escapes(neigh[j], list);
    }
  }
}

void Board::pattern_trick(int point, LList &list) const
{
  for (int i = 0; i < 8; i++) {
    if (int v = eight_around[point][i]) {
      if (belongedGroup[v] == 0 && is_match_mogo_pattern(v, side)) {
        list.add(v);
      }
    }
  }
}

bool Board::is_stones_around(int point, int distance) const
{
  for (int i = 0; i < distance; i++) {
    for (int j = 0; j < 4*(i+1); j++) {
      int v =  within_manhattan[point][i][j];
      if (is_occupied(v)) return true;
    }
  }
  return false;
}

int Board::get_total_liberties(int point, bool color, LList *liberties, int enough=0, const Group *exclude=0) const
{
  PointSet<MAXSIZE2> libs;
  if (liberties) point_liberties(point, *liberties);
  point_liberties(point, libs);
  if (enough && libs.length() > enough) return libs.length();
  
  GroupSet<4> neighbours;
  int nneigh = neighbour_groups(point, neighbours);
  for (int i = 0; i < nneigh; i++) {
    const Group *curr_neigh = neighbours[i];
    if (curr_neigh != exclude) {
      if (curr_neigh->get_color() == color) {
        for (Group::LibItr lib(curr_neigh); lib; ++lib) {
          if (*lib != point) {
            if (liberties) liberties->add(*lib);
            libs.add(*lib);
            if (enough && libs.length() > enough) return libs.length();
          }
        }
      } else if (curr_neigh->get_libs_num() == 1) {
        for (Group::StnItr st(curr_neigh); st; ++st) {
          for (int j = 0; four_side[*st][j]; j++) {
            if (four_side[*st][j] == point) {
              libs.add(*st);
              if (enough && libs.length() > enough) return libs.length();
            } else if (belongedGroup[four_side[*st][j]] &&
                       belongedGroup[four_side[*st][j]]->get_color() == color) {
              for (int k = 0; k < nneigh; k++) {
                if (belongedGroup[four_side[*st][j]] == neighbours[k]) {
                  libs.add(*st);
                  if (enough && libs.length() > enough) return libs.length();
                }
              }
            }
          }
        }
      }
    }
  }
  return libs.length();
}

bool Board::grow_liberties(int point, const Group *group) const
{
  int curr_liberties = 1;
  if (group) curr_liberties = group->get_libs_num();
  int nlibs = get_total_liberties(point, group->get_color(), 0, curr_liberties, group);
  return nlibs > curr_liberties;
}

bool Board::is_self_atari(int point, bool color) const
{
  return (get_total_liberties(point, color, 0, 1) == 1);
}

int Board::atari_last_liberty(int point, bool color) const
{
  PointSet<MAXSIZE2> liberties;
  if (get_total_liberties(point, color, &liberties, 1) == 1) return liberties[0]; //Maybe 0!
  return -1;
}

int Board::atari_escapes(const Group *group, LList &escapes) const
{
  for (Group::LibItr lib(group); lib; ++lib) {
    if (grow_liberties(*lib, group)) {
      escapes.add(*lib);
    }
  }
  GroupSet<MAXSIZE2/3> neighbours;
  int nneigh = neighbour_groups(group, !group->get_color(), group->get_libs_num(), neighbours);
  for (int i = 0; i < nneigh; i++) {
    for (Group::LibItr lib(neighbours[i]); lib; ++lib) {
      if (grow_liberties(*lib, group)) {
        escapes.add(*lib);
      }
    }
  }
  return escapes.length();
}

bool Board::is_ladder(int point, bool color) const
{
  if (get_total_liberties(point, color, 0) != 2) return false;
  if (neighbour_groups(point, !color, 2, 0)) return false;
  PointList<5> liberties;
  point_liberties(point, liberties);
  for (int i = 0; i < liberties.length(); i++) {
    PointList<5> secondary_libs;
    int delta[2] = {0};
    if (point_liberties(liberties[i], secondary_libs) == 4) continue;
  
    delta[0] = liberties[i] - point;
    for (int j = 0; j < secondary_libs.length(); j++) {
      if (secondary_libs[j] != point && secondary_libs[j] != liberties[i] + delta[0]) {
        delta[1] = secondary_libs[j] - liberties[i];
        break;
      }
    }
    if (delta[1] == 0) return true;
    int p = point, act = 0;
    while (distance2edge[p] > 1) {
      p = p + delta[act];
      if (belongedGroup[p]) {
        if (belongedGroup[p]->get_color() == color) break;
        else return true;
      }
      if (belongedGroup[p + delta[act]]) {
        if (belongedGroup[p + delta[act]]->get_color() == color) break;
        else return true;
      }
      act = 1-act;
    }
    if (distance2edge[p] < 2) return true;
  }
  return false;
}

int Board::creates_eyes(int point, bool color) const
{
  int neyes = 0;
  for (int i = 0; four_side[point][i]; i++) {
    if (is_true_eye(four_side[point][i], color, point)) {
      neyes++;
    }
  }
  return neyes;
}





bool Board::is_match_mogo_pattern(int point, bool side) const
{    
  if (point == 1 || point == size || point == size2 || point == size*(size-1)+1) return false;  //filter corners.
  
  if (point > size && point % size && point <= size*(size-1) && point % size != 1) {
    const int *vic = eight_around[point];
    for (int i = 1; i < 8; i+=2) {
      if (belongedGroup[vic[i]]) {
        bool adj_color = belongedGroup[vic[i]]->get_color();
        if (belongedGroup[vic[i-1]] && belongedGroup[vic[i-1]]->get_color() != adj_color) {
          if (belongedGroup[vic[i+2]] == 0 && belongedGroup[vic[i+6]] == 0) {
            if (belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() != adj_color) {
              return true;  
            }
            if (belongedGroup[vic[i+1]] == 0 && belongedGroup[vic[i+4]] == 0) {
              return true;  
            }
          }
          if (belongedGroup[vic[i+2]] == 0 && belongedGroup[vic[i+4]] == 0 &&
              belongedGroup[vic[i+6]] && belongedGroup[vic[i+6]]->get_color() != adj_color) {
              return true;  
          }
          if (belongedGroup[vic[i+6]] && belongedGroup[vic[i+6]]->get_color() == adj_color) {
            if ((belongedGroup[vic[i+2]] || belongedGroup[vic[i+4]] == 0 || belongedGroup[vic[i+4]]->get_color() == adj_color) &&
                (belongedGroup[vic[i+4]] || belongedGroup[vic[i+2]] == 0 || belongedGroup[vic[i+2]]->get_color() == adj_color)) {
              return true;  
            }
          }
        }
        if (belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() != adj_color) {
          if (belongedGroup[vic[i+2]] == 0 && belongedGroup[vic[i+6]] == 0) {
            if (belongedGroup[vic[i-1]] == 0 && belongedGroup[vic[i+4]] == 0) {
              return true;  
            }
          }
          if (belongedGroup[vic[i+4]] == 0 && belongedGroup[vic[i+6]] == 0 &&
              belongedGroup[vic[i+2]] && belongedGroup[vic[i+2]]->get_color() != adj_color) {
              return true; 
          }
        }
        if (belongedGroup[vic[i+2]] && belongedGroup[vic[i+6]] && belongedGroup[vic[i+2]]->get_color() != adj_color &&
            belongedGroup[vic[i+6]]->get_color() != adj_color &&
            (belongedGroup[vic[i+4]] == 0 || belongedGroup[vic[i+4]]->get_color() == adj_color) &&
            (belongedGroup[vic[i+3]] == 0 || belongedGroup[vic[i+3]]->get_color() == adj_color) &&
            (belongedGroup[vic[i+5]] == 0 || belongedGroup[vic[i+5]]->get_color() == adj_color)) {
          return true;  
        }
        if (adj_color != side && belongedGroup[vic[i-1]] && belongedGroup[vic[i-1]]->get_color() == side &&
            belongedGroup[vic[i+2]] == 0 && belongedGroup[vic[i+4]] == 0 && belongedGroup[vic[i+6]] == 0 &&
            belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() == adj_color) {
          return true;  
        }
        if (adj_color != side && belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() == side &&
            belongedGroup[vic[i+2]] == 0 && belongedGroup[vic[i+4]] == 0 && belongedGroup[vic[i+6]] == 0 &&
            belongedGroup[vic[i-1]] && belongedGroup[vic[i-1]]->get_color() == adj_color) {
          return true; 
        }
      }
    }
  } else {
    for (int i = 1; i < 8; i+=2) {
      const int *vic = eight_around[point];
      if (vic[i]) {
        if (belongedGroup[vic[i]]) {
          if (vic[i+2] && belongedGroup[vic[i+2]] && belongedGroup[vic[i+2]]->get_color() != belongedGroup[vic[i]]->get_color() &&
             (vic[i+6]==0 || belongedGroup[vic[i+6]] == 0 || belongedGroup[vic[i+6]]->get_color() != belongedGroup[vic[i]]->get_color())) {
            return true;  
          }
          if (vic[i+6] && belongedGroup[vic[i+6]] && belongedGroup[vic[i+6]]->get_color() != belongedGroup[vic[i]]->get_color() &&
             (vic[i+2]== 0 || belongedGroup[vic[i+2]] == 0 || belongedGroup[vic[i+2]]->get_color() != belongedGroup[vic[i]]->get_color())) {
            return true;  
          }
          if (belongedGroup[vic[i]]->get_color() == side) {
            if ((vic[i+1] && belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() != side)) {
              return true; 
            }
            if ((vic[i-1] && belongedGroup[vic[i-1]] && belongedGroup[vic[i-1]]->get_color() != side)) {
              return true; 
            }
          }
          if (belongedGroup[vic[i]]->get_color() != side) {
            if (vic[i+1] && belongedGroup[vic[i+1]] && belongedGroup[vic[i+1]]->get_color() == side) {
              if (vic[i+2] == 0 || belongedGroup[vic[i+2]] == 0 || belongedGroup[vic[i+2]]->get_color() == side) {
                return true; 
              }
              if (vic[i+2] && belongedGroup[vic[i+2]] && belongedGroup[vic[i+2]]->get_color() != side
                && vic[i+6] && belongedGroup[vic[i+6]] && belongedGroup[vic[i+6]]->get_color() == side) {
                return true;
              }
            }
            if (vic[i-1] && belongedGroup[vic[i-1]] && belongedGroup[vic[i-1]]->get_color() == side) {
              if (vic[i+6] == 0 || belongedGroup[vic[i+6]] == 0 || belongedGroup[vic[i+6]]->get_color() == side) {
                return true; 
              }
              if (vic[i+6] && belongedGroup[vic[i+6]] && belongedGroup[vic[i+6]]->get_color() != side &&
                  vic[i+2] && belongedGroup[vic[i+2]] && belongedGroup[vic[i+2]]->get_color() == side) {
                return true;
              }
            }
          }
        } else if ((vic[i+6] && belongedGroup[vic[i+6]] && vic[i-1] && belongedGroup[vic[i-1]] &&
                    belongedGroup[vic[i+6]]->get_color() != belongedGroup[vic[i-1]]->get_color())
               || (vic[i+2] && belongedGroup[vic[i+2]] && vic[i+1] && belongedGroup[vic[i+1]] &&
                    belongedGroup[vic[i+2]]->get_color() != belongedGroup[vic[i+1]]->get_color())) {
          return true;  
        }
      }
    }
  }
  return false;
}
