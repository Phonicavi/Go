#include "GoBoard.h"
/*bool Goban::fast_ladder(int point, bool color) const
{
  if (total_liberties(point, color, 0) != 2) return false;
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
    while (distance_to_edge[p] > 1) {
      p = p + delta[act];

      if (points[p]) {
        if (points[p]->get_color() == color) break;
        else return true;
      }
      if (points[p + delta[act]]) {
        if (points[p + delta[act]]->get_color() == color) break;
        else return true;
      }
      act = 1-act;
    }
    if (distance_to_edge[p] < 2) return true;
  }
  return false;
}*/


/*int Goban::atari_last_liberty(int point, bool color) const
{
  PointSet<MAXSIZE2> liberties;
  if (total_liberties(point, color, &liberties, 1) == 1) return liberties[0]; //Maybe 0!
  return -1;
}

bool GoBoard::bad_self_atari(int point, bool color) const
{
  int last_lib = atari_last_liberty(point, color);
  if (last_lib == -1) return false;
#ifdef DEBUG_INFO
  std::cerr << "self_atari at " << point << "\n";
#endif
    if (total_liberties(last_lib, !color, 0) < 2) {
#ifdef DEBUG_INFO
      std::cerr << "snapback\n";
#endif  
      return false;
    }
    if (creates_eyes(point, !color) && !creates_eyes(last_lib, !color)) {
#ifdef DEBUG_INFO
        std::cerr << "throw-in\n";
#endif
      return false;
    }

  if (nakade_shape(point, color)) {
#ifdef DEBUG_INFO
    std::cerr << "nakade\n";
#endif
    return false;
  }
  return true;
}
*/

/*bool GoBoard::is_self_atari(int point, int color) //  is_self_atari that is , add this point, the group of the move will be in atari(only one liberty)
{

  return (total_liberties(point, color, 0, 1) == 1);
}*/


bool GoBoard::is_virtual_eye(int point, int color) 
{
  if (!is_surrounded(point, color)) return false;
  int nopponent = 0;
  int ai = I(point);
  int aj = J(point);
  bool at_edge = false;
  for (int i = 0; i < 4; i++) {
        int bi = ai + diag_i[i];
        int bj = aj + diag_j[i];
        if (!on_board(bi,bj))
        {
            at_edge = true;
            continue;
        }
        if( get_board(bi,bj) == OTHER_COLOR(color) ){
            nopponent++;
        }
  }
  if(at_edge)
    ++nopponent;
  return nopponent < 2;
}

bool GoBoard::heavy_policy(int point, int  color)
{
  if (is_virtual_eye(point, color)) return false;
  if (!available(I(point),J(point), color)) return false;
 // if (is_self_atari(point, color)) return false;
  //if (fast_ladder(point,color)) return false;  I don't understand what is fast_ladder, the implementation is below.
  //if (bad_self_atari(point,color)) return false; do not understand.... refer the source code.
  return true;
}


int GoBoard::is_star_available (int color, int last_moves)
{
  int star[4] = {3*board_size+3,3*board_size+9,9*board_size+3,9*board_size+9}; //stars position
  int mid[4] = {3*board_size+6,6*board_size+3,6*board_size+9,9*board_size+6}; //middle of stars

  //Occupy Stars
  if (board[star[0]] == EMPTY && heavy_policy(star[0],color)) return star[0];
  if (board[star[1]] == EMPTY && heavy_policy(star[1],color)) return star[1];
  if (board[star[2]] == EMPTY && heavy_policy(star[2],color)) return star[2];
  if (board[star[3]] == EMPTY && heavy_policy(star[3],color)) return star[3];

  //Occupy Mids
  if(board[star[0]] != EMPTY && board[star[1]] != EMPTY)
  {
    if(board[star[0]] == color && board[star[1]] == color
       && board[mid[0]] == EMPTY && heavy_policy(mid[0],color))
      return mid[0];
  }

  if(board[star[0]] != EMPTY && board[star[2]] != EMPTY)
  {
    if(board[star[0]] == color && board[star[2]] == color
       && board[mid[1]] == 0 && heavy_policy(mid[1],color))
      return mid[1];
  }

  if(board[star[1]] != 0 && board[star[3]] != 0)
  {
    if(board[star[1]] == color && board[star[3]] == color
       && board[mid[2]] == 0 && heavy_policy(mid[2],color))
      return mid[2];
  }

  if(board[star[2]] != 0 && board[star[3]] != 0)
  {
    if(board[star[2]] == color && board[star[3]] == color
       && board[mid[3]] == 0 && heavy_policy(mid[3],color))
      return mid[3];
  }
  return -1;
}
