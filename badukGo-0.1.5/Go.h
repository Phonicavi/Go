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
#ifndef __GO_H__
#define __GO_H__
#include <vector>
#include "Board.h"
#include "amaf.h"
#include "tree.h"

#include <process.h>
#include <windows.h>
#include <winbase.h>

#define FIRST_LEVEL_EMPTY 70
#define SECOND_LEVEL_EMPTY 35
#define MAX_THREAD_LIMIT 6
#define EXPAND 8
#define NEED_PRIORS
#ifdef NEED_PRIORS
  #define STOP_PRIORS_WINRATE_THERESHOLD 0.87
#endif
const int FIRST_R_PRUN_TIME = 1;
const int SECOND_R_PRUN_TIME = 2;

class Go{
 private:
  const bool HEAVY = true, LIGHT = false;
  int SYS_THREAD_LIMIT;
  int real_thread_num;
  double r_prun_alpha;
  double lastWinRate[2],last2WinRate[2];
  CRITICAL_SECTION TREE_CRITICAL;
  HANDLE slave_thread[MAX_THREAD_LIMIT];
  Board *main_board;
  Board *thread_board[MAX_THREAD_LIMIT];
  AmafBoard *amaf_thread[MAX_THREAD_LIMIT];
  int tree_size, max_playouts;
  int rand_movs[MAX_THREAD_LIMIT], simul_len, discarded;
  Tree tree;
  AmafBoard amaf;
  mutable clock_t fin_clock, max_time;
#ifdef NEED_PONDER
  bool is_ponder;
#endif

  int get_best_move() const;
  int play_random_game(bool wiser, float &score);
  int play_random_game_thread(bool wiser, int simul_len_thread,
                Board *cur_board, AmafBoard *cur_amaf, int tid, float &score);

  void back_up_results(int result, Node *node_history[], int nnodes, bool side);
  void back_up_results_thread(int result, Node *node_history[], 
                        int nnodes, bool side,AmafBoard *cur_amaf,float sc);
  void print_PV() const;


 public:

  Go(Board *board);
  ~Go(){
    DeleteCriticalSection(&TREE_CRITICAL);
    for (int i=0; i<MAX_THREAD_LIMIT; ++i){
      delete  thread_board[i];
      delete  amaf_thread[i];
    }
  };
#ifdef NEED_PONDER
  void start_ponder(int step);
  void stop_ponder(){
    if (!is_ponder) return;
    std::cerr << "end pondering" << std::endl;
    is_ponder = false;
    WaitForMultipleObjects(real_thread_num,slave_thread,true,INFINITE);
    for (int i=0; i<real_thread_num; ++i){
      CloseHandle(slave_thread[i]);
    }
  }
  void run_thread_ponder(int max_time_thread, Board *cur_board, 
              AmafBoard *cur_amaf, int tid,int);
#endif
  void reset();
  void set_playouts(int playouts);
  void run_thread(int max_time_thread, Board *cur_board, 
              AmafBoard *cur_amaf, int tid,int);
  float score(std::vector<int> *dead);
  int generate_move(bool early_pass,int);
  void perft(int max);
  void report_move(int move,int step) { tree.promote(move,step); }




};



struct id{
  int thread_id;
  Board *orig_bd;
  Board *cur_bd;
  Go *goo;
  int step;
  int max_time_thread;
  AmafBoard * tamaf;
};

#ifndef SLAVE_RUNNER
#define SLAVE_RUNNER 
DWORD WINAPI slave_runner(void *args);
  #ifdef NEED_PONDER
    DWORD WINAPI slave_runner_ponder(void *args);
  #endif
#endif



#endif
