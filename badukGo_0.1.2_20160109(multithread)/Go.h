/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/
#ifndef __GO_H__
#define __GO_H__
#include <vector>
#include "zobrist.h"
#include "Board.h"
#include "amaf.h"
#include "tree.h"
#include <process.h>
#include <windows.h>
#define MAX_THREAD_LIMIT 6
#define EXPAND 8


// #ifndef MUTEX
// #define MUTEX 
// HANDLE TREE_MUTEX;
// #endif



class Go{
 private:
  const bool HEAVY = true, LIGHT = false;

  CRITICAL_SECTION TREE_CRITICAL;

  HANDLE slave_thread[MAX_THREAD_LIMIT];




  int SYS_THREAD_LIMIT;
  int real_thread_num;

  
  Board *main_board;
  Board *thread_board[MAX_THREAD_LIMIT];
  AmafBoard *amaf_thread[MAX_THREAD_LIMIT];
  int tree_size, max_playouts;
  int rand_movs[MAX_THREAD_LIMIT], simul_len, discarded;
  Tree tree;
  AmafBoard amaf;
  mutable clock_t fin_clock, max_time;

  int get_best_move() const;
  int play_random_game(bool heavy);
  int play_random_game_thread(bool heavy, int simul_len_thread,Board *cur_board, AmafBoard *cur_amaf, int tid);

  void back_up_results(int result, Node *node_history[], int nnodes, bool side);
  void back_up_results_thread(int result, Node *node_history[], int nnodes, bool side,AmafBoard *cur_amaf);
  void print_PV() const;


 public:

  Go(Board *board);
  ~Go(){};
  void reset();
  void set_playouts(int playouts);
  void run_thread(int max_time_thread, Board *cur_board, AmafBoard *cur_amaf, int tid);
  // void set_times(int main_time, int byo_time, int stones);
  // void set_times(int time_left, int stones);
  float score(std::vector<int> *dead);
  int generate_move(bool early_pass);
  void perft(int max);
  void report_move(int move) { tree.promote(move); }




};



struct id{
  int thread_id;
  Board *orig_bd;
  Board *cur_bd;
  Go *goo;
  int max_time_thread;
  AmafBoard * tamaf;
};

#ifndef SLAVE_RUNNER
#define SLAVE_RUNNER 
DWORD WINAPI slave_runner(void *args);
#endif



#endif
