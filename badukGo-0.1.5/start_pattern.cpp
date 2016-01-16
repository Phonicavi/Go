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


#define set_4(x) size*(x-1)+x,size*(size-x)+x,size*(x-1)+size-(x-1),size*(size-x)+size-(x-1)
#define set_8(a,b) size*(a-1)+b,size*(b-1)+a,size*(size-a)+b,size*(size-b)+a,size*(a-1)+size-(b-1),size*(b-1)+size-(a-1),size*(size-a)+size-(b-1),size*(size-b)+size-(a-1)
#define center size/2+1
#define set_ce(x) size*(x-1)+center,size*(size-x)+center,size*(center-1)+x,size*(center-1)+size-(x-1)

// first-step: star_point*4 = 4
#define recommend_1 {set_4(4)}
// first-2: star_point*4 + 3-4_point*8 = 12
#define recommend_2 {set_4(4),set_8(3,4)}
// first-3: nine(9)[without:5-5] + 3-6_point = 40
#define recommend_3 {set_4(3),set_4(4),set_8(3,4),set_8(3,5),set_8(3,6),set_8(4,5)}
// first-4: line[3, 4, 5] more = 72
#define recommend_4 {set_4(3),set_4(4),set_4(5),set_8(3,4),set_8(3,5),set_8(4,5),set_8(3,6),set_8(4,6),set_8(5,6),set_ce(3),set_ce(4),set_ce(5)}


inline bool is_in_set(int point, int set[], int length)
{
  for (int i=0; i<length; ++i) {
    if (point==set[i])
      return true;
  }
  return false;
}

int Board::legal_moves(int moves[]) const
{
  int nlegal = 0;
  int step = this->get_history_length();
  if (step<=1) {
    int recommend[4] = recommend_1;
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_in_set(point, recommend, 4) && is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  } else if (step<=3) {
    int recommend[12] = recommend_2;
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_in_set(point, recommend, 12) && is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  } else if (step<=5) {
    int recommend[40] = recommend_3;
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_in_set(point, recommend, 40) && is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  } else if (step<=7) {
    int recommend[72] = recommend_4;
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_in_set(point, recommend, 72) && is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  } else {
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  }
  return nlegal;
}

int Board::legal_moves_origin(int moves[]) const
{
  int nlegal = 0;
    for (int i = 0; i < empty_points.length(); i++) {
      int point = empty_points[i];
      if (is_legal(point, side) && !is_true_eye(point,side)) {
        moves[nlegal++] = point;
      }
    }
    moves[nlegal++] = PASS;
  return nlegal;
}



/**special positions
 * tengen 天元
 * star_position 星位
 * 3-3 point 三三
 * 5-5 point 五五
 * 3-4 point 小目
 * 4-5 point 高目
 * 3-5 point 目外
 * 4-6 point 超高目
 *
 */
bool Board::is_tengen(int point) const
{
  return point==(size2/2+1);
}

bool Board::is_star_point(int point) const
{
  int x=4;
  int set[4] = {set_4(x)};
  return is_in_set(point, set, 4);
}

bool Board::is_point_3_3(int point) const
{
  int x=3;
  int set[4] = {set_4(x)};
  return is_in_set(point, set, 4);
}

bool Board::is_point_5_5(int point) const
{
  int x=5;
  int set[4] = {set_4(x)};
  return is_in_set(point, set, 4);
}

bool Board::is_point_3_4(int point) const
{
  int a=3, b=4;
  int set[8] = {set_8(a,b)};
  return is_in_set(point, set, 8);
}

bool Board::is_point_4_5(int point) const
{
  int a=4, b=5;
  int set[8] = {set_8(a,b)};
  return is_in_set(point, set, 8);
}

bool Board::is_point_3_5(int point) const
{
  int a=3, b=5;
  int set[8] = {set_8(a,b)};
  return is_in_set(point, set, 8);
}

bool Board::is_point_4_6(int point) const
{
  int a=4, b=6;
  int set[8] = {set_8(a,b)};
  return is_in_set(point, set, 8);
}

