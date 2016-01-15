
#ifndef __GO_H__
#define __GO_H__
#include <vector>
#include "Board.h"
#include "amaf.h"
#include "tree.h"

#include <process.h>
#include <windows.h>
#include <winbase.h>
#define MAX_THREAD_LIMIT 6
#define EXPAND 8
#define NEED_PRIORS
#ifdef NEED_PRIORS
  #define STOP_PRIORS_WINRATE_THERESHOLD 1
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

  int get_best_move() const;
  int play_random_game(bool wiser, float &score);
  int play_random_game_thread(bool wiser, int simul_len_thread,
                Board *cur_board, AmafBoard *cur_amaf, int tid, float &score);

  void back_up_results(int result, Node *node_history[], int nnodes, bool side);
  void back_up_results_thread(int result, Node *node_history[], 
                        int nnodes, bool side,AmafBoard *cur_amaf);
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
  void reset();
  void set_playouts(int playouts);
  void run_thread(int max_time_thread, Board *cur_board, 
              AmafBoard *cur_amaf, int tid,int);
  float score(std::vector<int> *dead);
  int generate_move(bool early_pass,int);
  void perft(int max);
  void report_move(int move) { tree.promote(move); }




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
#endif



#endif
