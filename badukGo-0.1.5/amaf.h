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
#ifndef __AMAF_H__
#define __AMAF_H__
#include "base.h"
#include <iomanip>

class AmafBoard{
 private:
  int board[MAXSIZE2+1];
  int size;
  bool side;

 public:
  AmafBoard(int sz = 13)
  {
    size = sz <= MAXSIZE ? sz : MAXSIZE; 
    side = 0;
    clear();
  }
  
  void clear()
  {
    for (int i = 0; i <= size*size; i++) {
      board[i] = 0;
    }
  }

  void set_up(bool sd, int sz)
  {
    size = sz;
    side = sd;
    clear();
  }

  int play(int coord, int depth)  
  {
    if (board[coord] == 0) {
      board[coord] = side ? -depth : depth;
      side = !side;
      return 0;
    } else {
      side = !side; 
      return -1;
    }
  }

  double value(int coord, int depth, bool side, double discount) const
  {
    if (coord == 0) return 0.0;
    int val = side ? -board[coord] : board[coord];
    if (val >= depth) {
      return 1.0-val*discount;
    } else {
      return 0;
    }
  }

  void print() const{
    for (int y = size - 1; y > -1; y--) {
      std::cerr << "  |";
      for (int x = 1; x < size+1; x++) {
        
        if (board[size*y + x] < 0) {
          std::cerr << std::setw(3) << board[size*y + x] << "|";
        } else if(board[size*y + x] > 0) {
          std::cerr << std::setw(3) << board[size*y + x] << "|";
        } else {
          std::cerr << "   |";
        }
      }
      std::cerr << "|" << "\n";
    }
    std::cerr << "pass: " << board[0] << "\n";
  }
};
#endif
