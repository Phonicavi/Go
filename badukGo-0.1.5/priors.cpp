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


void Board::init_priors(Prior priors[]) const
{
  const double EQUIV = size;
  priors[PASS].prior = 0.1*EQUIV, priors[PASS].equiv = EQUIV;

  for(int i = 0; i < empty_points.length(); ++i){    
    int p = empty_points[i];
       priors[p].prior = 0.5*EQUIV, priors[p].equiv = EQUIV;
    }
    for(int i = 0; i < empty_points.length(); ++i){    
    int p = empty_points[i];
    if(is_self_atari(p, side)){
      priors[p].prior = 0.2*EQUIV, priors[p].equiv = 2*EQUIV;
      continue;
    }  
    if(is_surrounded(p, side)){
      priors[p].prior = 0.3*EQUIV, priors[p].equiv = EQUIV;
      continue;
    }
    priors[p].prior = 0.5*EQUIV, priors[p].equiv = EQUIV;
    if(size > 11){
      if(distance2edge[p] == 0 && !is_stones_around(p, 4)){
        priors[p].prior = 0.1*EQUIV, priors[p].equiv = EQUIV;
      }
      else if(distance2edge[p] == 3 && !is_stones_around(p, 4)){
        priors[p].prior = 0.9*EQUIV, priors[p].equiv = EQUIV;
      }
    }
    
    GroupSet<4> neigh;
    int nneigh = neighbour_groups(p, neigh);
    for(int j = 0; j < nneigh; j++){
      if(neigh[j]->has_one_liberty()){
        if(neigh[j]->get_color() != side){
          priors[p].prior = 1.4*EQUIV, priors[p].equiv = 2*EQUIV;
          goto endloop;
        }
        else{
          priors[p].prior = 0.6*EQUIV, priors[p].equiv = EQUIV;
          goto endloop;
        }
      }
    }
    if(is_match_mogo_pattern(p, side)){
      priors[p].prior = 0.9*EQUIV, priors[p].equiv = EQUIV;
      continue;
    }
  endloop:;
  }
  
  if(last_point == 0) return;
  
  PointList<MAXSIZE2> list;
  // nakade_trick(last_point, list);
  // for(int i = 0; i < list.length(); i++){
  //   priors[list[i]].prior += 2*EQUIV, priors[list[i]].equiv += 2*EQUIV;
  // }
  // list.clear();
  capture_trick(last_point, list);
  for(int i = 0; i < list.length(); i++){
    priors[list[i]].prior += 3*EQUIV, priors[list[i]].equiv += 3*EQUIV;
  }
  list.clear();

  save_trick(last_point, list);
  for(int i = 0; i < list.length(); i++){
    priors[list[i]].prior += 2*EQUIV, priors[list[i]].equiv += 2*EQUIV;
  }
  list.clear();

  pattern_trick(last_point, list);
  for(int i = 0; i < list.length(); i++){
    priors[list[i]].prior += 2*EQUIV, priors[list[i]].equiv += 2*EQUIV;
  }
  list.clear();

  
  for(int i = 0; last_point && i < 4; i++){
    for(int j = 0; j < 4*(i+1); j++){
      int v =  within_manhattan[last_point][i][j];
      if(v) {
        priors[v].prior += (1.0-0.1*i)*EQUIV, priors[v].equiv += EQUIV;
      }
    }
  }
  
}
