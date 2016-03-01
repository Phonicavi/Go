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
#include <iostream>
#include <ctime>
#include "Go.h"

const double TIME_TOLERANCE = 0.01;
const double TIME_UP_LIMIT_SYSTEM = 3;
const double TIME_LIMIT = TIME_UP_LIMIT_SYSTEM-TIME_TOLERANCE;
// #define CHANGE_THEARD_NUM


Go::Go(Board *board):tree(DEF_TREESIZE, board), amaf(board->get_size())
{

  lastWinRate[0] = lastWinRate[1] = last2WinRate[0] = last2WinRate[1] = 0.0;
  r_prun_alpha = AGRESSIVE_R_PRUN_ALPHA;
#ifdef _RWLOCK_
  InitializeSRWLock(&TREE_SRW);
#else
  InitializeCriticalSection(&TREE_CRITICAL);
#endif
  SYSTEM_INFO sinfo;
  GetSystemInfo(&sinfo);
  SYS_THREAD_LIMIT = max(min((int)sinfo.dwNumberOfProcessors,MAX_THREAD_LIMIT),4);
  real_thread_num = SYS_THREAD_LIMIT;
  main_board = board;
  max_playouts = DEF_PLAYOUTS;
  max_time = TIME_LIMIT*CLOCKS_PER_SEC;
  //max_time = INFINITE;
  tree_size = DEF_TREESIZE;
  memset(rand_movs,0,sizeof rand_movs);
  for (int i=0; i<MAX_THREAD_LIMIT; ++i){
    thread_board[i] = new Board(board->get_size());
    amaf_thread[i] = new AmafBoard(board->get_size());
  }
}

void Go::reset()
{
  memset(rand_movs,0,sizeof rand_movs);
  tree.clear();
  amaf.set_up(main_board->get_side(), main_board->get_size());
}

void Go::set_playouts(int playouts)
{
  max_playouts = playouts;
  max_time = INFINITE;
}


int Go::play_random_game(bool wiser,float &score)
{
  int pass = 0;
  int mercy;
  int size2 = main_board->get_size2();
  if (!rand()%10)main_board->shuffle_empty();
  while (pass < 2) {
    int move = wiser ? main_board->play_wiser() : main_board->play_random();
    amaf.play(move, ++simul_len);
    rand_movs[0]++;
    if (move == Board::PASS) pass++;
    else pass = 0;
#ifdef DEBUG_INFO
      main_board->print_board();
#endif
    int mercy = main_board->mercy();
    if (mercy != -1) {
      score = (((1-mercy)<<1)-1)*size2/3;
      return 1-mercy;
    }
    if (simul_len > 2*main_board->get_size2()) {
      std::cerr << "WARNING: Simulation exceeded max length.\n";
      discarded++;
      return -1;
    }
  }
    return ((score = main_board->score_count()) > 0);
}

int Go::generate_move(bool early_pass,int cur_step)
{
  bool side = main_board->get_side();
    int last[2];
  Node *root = tree.get_root();
  std::cerr <<"root_visit_by_pondering: "<< root->get_visits() << std::endl;
  rand_movs[0] = 0, discarded = 0;
  double aver_winrate = (lastWinRate[side] + last2WinRate[side])/2 ; 
  main_board->shuffle_empty();

  fin_clock = clock();

#ifdef CHANGE_THEARD_NUM
  int empty_len = main_board->get_empty_length();
  if (empty_len > FIRST_LEVEL_EMPTY){
    real_thread_num = SYS_THREAD_LIMIT;
  }else if (empty_len > SECOND_LEVEL_EMPTY){
    real_thread_num = min(4,SYS_THREAD_LIMIT);
  }else real_thread_num = 1;
#else

#endif

  for (int i=0; i<real_thread_num; ++i){
    struct id *cid = (struct id*) malloc(sizeof(struct id));
    cid->thread_id = i+1;
    cid->orig_bd = main_board;
    cid->cur_bd = thread_board[i];
    cid->step = cur_step;
    cid->goo = this;
    cid->max_time_thread = max_time;
    cid->tamaf = amaf_thread[i];
    slave_thread[i] = CreateThread(NULL,0,slave_runner,cid,0,NULL);
  }
  WaitForMultipleObjects(real_thread_num,slave_thread,true,INFINITE);
  for (int i=0; i<real_thread_num; ++i){
    CloseHandle(slave_thread[i]);
  }
  

  Node *best = tree.get_best(aver_winrate);
  last2WinRate[side] = lastWinRate[side];
  lastWinRate[side] = best->get_value(1);
  std:cerr << "winrate: " << lastWinRate[side] << " " << last2WinRate[side] << std::endl;
#ifdef STD_ERR_PRINT
  print_PV();
#endif
  if (best->get_move() == Board::PASS) return Board::PASS;
  std::cerr << best->get_value(1) <<std::endl;
  std::cerr << "root visit:" << root->get_visits() << std::endl;
  std::cerr << "best score:" << best->get_score() << std::endl;
  return best->get_move();
}

void Go::back_up_results(int result, Node *node_history[], int nnodes, bool side)
{
  for (int i = 0; i < nnodes; i++) {
    node_history[i]->set_results(1-result); 
    node_history[i]->set_amaf(result, amaf, side, i+1);
    side = !side;
    result = 1-result;
  }
}

void Go::print_PV() const
{
  #ifdef STD_ERR_PRINT
  fin_clock = clock() - fin_clock;
  bool side = main_board->get_side();

  tree.print((last2WinRate[side] + lastWinRate[side])/2);
  int nplayouts = tree.get_root()->get_visits();
  int rand_movettl = 0;
  for (int i=0; i<MAX_THREAD_LIMIT; ++i) rand_movettl += rand_movs[i];
  std::cerr << "#Playouts: " << nplayouts
            << ", history_length: " << main_board->get_history_length()
            << ", empty_length: " << main_board->get_empty_length()
            << ", average length: " << (rand_movettl)/nplayouts
            << ", discarded: " << discarded << ", playouts/sec: "
            << (float)nplayouts/fin_clock*CLOCKS_PER_SEC << "\n";
  main_board->print_goban();
  #endif
}

float Go::score(std::vector<int> *dead)
{
  const int PLAYOUTS = 5000;
  int score = 0;
  float tmp;
  int score_table[MAXSIZE2+1] = {0};
  for (int i = 0; i < PLAYOUTS; i++) {
    simul_len = 0;
    play_random_game(LIGHT,tmp);
    main_board->score_area(score_table);
    main_board->restore();
  }
  for (int i = 1; i <= main_board->get_size2(); i++) {
    if (score_table[i] > PLAYOUTS/2) score_table[i] = 1;
    else if (score_table[i] < -PLAYOUTS/2) score_table[i] = -1;
    else score_table[i] = 0;

    if (dead && main_board->get_value(i)
        && score_table[i] !=  main_board->get_value(i)) {
      dead->insert(dead->end(), i);
    }
  }

  for (int i = 1; i <= main_board->get_size2(); i++) {
    score += score_table[i];
  }
  return score - main_board->get_komi();
}

void Go::perft(int max)
{
  
}


DWORD WINAPI slave_runner(void *args){

  struct id * cid = (id*)args;
  srand(cid->thread_id*time(NULL)+1);
  cid->cur_bd->copy_from(cid->orig_bd);
  cid->goo->run_thread(cid->max_time_thread,cid->cur_bd,cid->tamaf,cid->thread_id,cid->step);
  return 0;
}
#ifdef NEED_PONDER
DWORD WINAPI slave_runner_ponder(void *args){

  struct id * cid = (id*)args;
  srand(cid->thread_id*time(NULL)+1);
  cid->cur_bd->copy_from(cid->orig_bd);
  cid->goo->run_thread_ponder(cid->max_time_thread,cid->cur_bd,cid->tamaf,cid->thread_id,cid->step);
  return 0;
}
#endif