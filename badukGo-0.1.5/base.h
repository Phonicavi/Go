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
#ifndef __BASE_H__
#define __BASE_H__
#include <string>
#include <sstream>
#include "FastLog.h"
#include <iostream>

//#define DEBUG_INFO
#define DEF_PLAYOUTS 3000000
#define DEF_TREESIZE 10000000
#define ASTRAY_RED_FRAME() rand()
#define ALIE_STRIKE(a) srand(a)

#define NEED_PONDER
#ifdef NEED_PONDER
  #define PONDER_PLAYOUTS 200000
  #define START_PONDER_STEP 10
  #define PONDER_THREAD 4
#endif
#define START_REUSE_SUBTREE 10

#define MAXSIZE  13
#define MAXSIZE2  169
#define STEPS_START_END 3     //开局要走多少步固定位置
#define STEPS_BOUNDARY_TWO 20   //多少步以内忽略棋牌最外圈两层点
#define STEPS_BOUNDARY_ONE 10   //多少步以内忽略棋牌最外圈一层点
// #define STD_ERR_PRINT

const char COORDINATES[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'};

static int TWO_EXIST = 0;
static int ONE_EXIST = 0;
static int board_map[MAXSIZE2 + 1] = {0};

static void init_board_map(){
    int gap = MAXSIZE * (MAXSIZE - 1);
    //最外层
    for (int i = 1; i <= MAXSIZE; ++i)
      board_map[i] = board_map[i + gap] = 1;
    for (int i = 1; i <= 157; i += MAXSIZE)
      board_map[i] = board_map[i + 12] = 1;

    //倒数外二层
    for (int i = 15; i <= 25; ++i)
      board_map[i] = board_map[i + 130] = 2;
    for (int i = 15; i <= 145; i += MAXSIZE)
      board_map[i] = board_map[i + 10] = 2;
}

static void coord_to_char(int coord, std::string &response, int size){
  if(coord == 0){
    response.append("pass");
  }
  else{
    int y = (coord - 1)/size + 1;
    int x = (coord - 1) % size;
    std::string auxstring;
    std::stringstream auxstream;
    auxstream << COORDINATES[x];
    auxstream >> auxstring;
    response.append(auxstring);

    auxstream.clear();
    auxstream << y;
    auxstream >> auxstring;

    response.append(auxstring);
  }
}
#endif
