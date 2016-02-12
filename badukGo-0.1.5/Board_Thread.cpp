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
#include "Go.h"
#include <assert.h>

#define BACKUP_NEED_CRITICAL_LIMIT_EMPTY 90
#define RELATE_PRUNE
#define LOCK_UCT_SELECT

void Board::copy_from(Board *board)
{
  side_bu = board->side;
  ko_point_bu = board->ko_point;
  size_bu = board->size, size2_bu = board->size2;
  komi_bu = board->komi;
  handicap_bu = board->handicap;
  empty_points_bu = board->empty_points;
  game_history = board->game_history;
  GroupFather = board->GroupFather;

  for (unordered_set<int>::iterator it = (GroupFather).begin(); it != (GroupFather).end(); ++it){
        groups_bu[*it] = board->groups[*it];
  }

  stones_on_board_bu[1] = board->stones_on_board[1];
  stones_on_board_bu[0] = board->stones_on_board[0];
  last_point_bu = board->last_point, last_point2_bu = board->last_point2;
  last_atari_black_bu = board->last_atari[0]?board->last_atari[0]->get_father():0;
  last_atari_white_bu = board->last_atari[1]?board->last_atari[1]->get_father():0;
  
  resume();
}


void Go::run_thread(int max_time_thread,Board *cur_board, AmafBoard *cur_amaf,int tid,int cur_step)
{
  Node * n = NULL;
 #ifdef STD_ERR_PRINT
  std::cerr << "here!!" << std::endl;
 #endif
  rand_movs[tid-1] = 0;
  int nnode_hist = 0;
  Node *node_history[3*MAXSIZE2];
  int result =2;
  int last[2];
  bool side = -1;
  int simul_len_thread;
  int ct;
  int pruntime = 0;
  int prePunc = 0;
  int debugTime = fin_clock;
  int pass;
  double aver_winrate;
  Node *root;
  Node *node;
  float score;
  aver_winrate = (lastWinRate[main_board->get_side()] + last2WinRate[main_board->get_side()])/2 ; 
  // std::cerr << "aver_winrate:" << aver_winrate<<std::endl;
  // std::cerr << "STOP_PRIORS_WINRATE_THERESHOLD: " << STOP_PRIORS_WINRATE_THERESHOLD<<std::endl;

  while (clock() - fin_clock < max_time_thread) {
 #ifdef STD_ERR_PRINT
  if (clock()-debugTime>0.5*CLOCKS_PER_SEC){
	debugTime = clock();
	std::cerr << "id:" << tid << " time:" << debugTime <<std::endl;
  }
 #endif

  pass = 0;
  simul_len_thread = 0;
  side = cur_board->get_side();
  root = tree.get_root();
	  	
  Node *node;


 #ifdef RELATE_PRUNE
  if (aver_winrate>STOP_THERESHOLD)
	  goto JUMP_R_PRUN;
  else if (aver_winrate > SOFT_THERESHOLD)
	  r_prun_alpha = SOFT_R_PRUN_ALPHA;
  else if (aver_winrate> LAZY_THERESHOLD)
	  r_prun_alpha = LAZY_R_PRUN_ALPHA;
		
  ct = (clock()-fin_clock)/CLOCKS_PER_SEC;
  if (cur_board->get_history_length() > BEGIN_PRUN && cur_board->get_history_length() < END_PRUN
      &&(tid == 1) && (!prePunc) && ((ct == 1 && pruntime == 0) || (ct ==2 && pruntime == 1)))
  {
	EnterCriticalSection(&TREE_CRITICAL);
	tree.do_r_prun(root->get_visits(),r_prun_alpha);
	LeaveCriticalSection(&TREE_CRITICAL);
	pruntime++;
   #ifdef STD_ERR_PRINT
	std::cerr << "prun_num:" << tree.relative_prun_num <<std::endl;
	std::cerr << "root_visits:" << root->get_visits() <<std::endl;
   #endif
	prePunc = tree.relative_prun_num;
	tree.relative_prun_num = 0;
  }
	JUMP_R_PRUN:;
#endif

	cur_amaf->set_up(cur_board->get_side(), cur_board->get_size());
	node = root;
	nnode_hist = 0;

	while (node->has_childs() && pass < 2) {
	  node_history[nnode_hist++] = node;
	 #ifdef LOCK_UCT_SELECT
	  EnterCriticalSection(&TREE_CRITICAL);
	 #endif
	  node = node->select_child();
	 #ifdef LOCK_UCT_SELECT
	  LeaveCriticalSection(&TREE_CRITICAL);
	 #endif
	  int move = node->get_move();
	  
	  if(move == Board::PASS) pass++;
	  else pass = 0;
	  
	  cur_board->play_move(move);
	      cur_amaf->play(move, ++simul_len_thread);
	}

	if (node->is_expand==0 &&(node->get_visits() >= EXPAND || node == root) ) {
	  node->is_expand = tid;
	  if (node->is_expand == tid){
		Prior priors[MAXSIZE2+1] = {{0,0}};
        int legal_moves[MAXSIZE2+1],nlegal;
        if(node == root){
          //====分4-10步，10-20步情况，20步后===///
	      if (cur_step >= STEPS_BOUNDARY_TWO)
	        nlegal = cur_board->legal_moves(legal_moves);
	      else if (cur_step > STEPS_BOUNDARY_ONE && cur_step < STEPS_BOUNDARY_TWO)
	        nlegal = cur_board->first_legal_moves(legal_moves,last,2,cur_step);
	      else if (cur_step >= STEPS_START_END && cur_step <= STEPS_BOUNDARY_ONE)
	        nlegal = cur_board->first_legal_moves(legal_moves,last,1,cur_step);
	      else
	        nlegal = cur_board->legal_moves(legal_moves);
	    }
		else{
	      nlegal = cur_board->legal_moves_origin(legal_moves);
	    }

     #ifdef NEED_PRIORS
	    if ((aver_winrate<STOP_PRIORS_WINRATE_THERESHOLD)&&(rand()&3)&&cur_board->get_history_length()>=8 && cur_board->get_empty_length()>20)
	           cur_board->init_priors(priors);
	   #endif
	   
		EnterCriticalSection(&TREE_CRITICAL);
		tree.expand(node, legal_moves, nlegal, priors); 
    LeaveCriticalSection(&TREE_CRITICAL);

	  }
	}

	node_history[nnode_hist++] = node;
	score = 0;

	result = play_random_game_thread(cur_board->get_history_length()>8,simul_len_thread,cur_board,cur_amaf,tid,score);
   #ifdef DEBUG_INFO
	cur_board->print_board();
	std::cerr << result << "\n";
   #endif

	cur_board->resume();
	if (result == -1) continue;
	if (side) result = 1-result,score = -score;

	EnterCriticalSection(&TREE_CRITICAL);
   #ifdef STD_ERR_PRINT
	if (nnode_hist > 1 && node_history[1] == NULL){
	  std::cerr<<"CAONIDAYE" <<std::endl;
	  assert(0);
    }
   #endif
	// if (nnode_hist > 1 && node_history[1] != NULL)
	//   {node_history[1]->update_score(score);}
	back_up_results_thread(result, node_history, nnode_hist, side,cur_amaf,score);
	  
    LeaveCriticalSection(&TREE_CRITICAL);
	}

   #ifdef STD_ERR_PRINT
	std::cerr << rand_movs[tid-1] << " " << tid << std::endl;
   #endif
}

int Go::play_random_game_thread(bool wiser, int simul_len_thread, 
			Board *cur_board, AmafBoard *cur_amaf,int tid,float &score){
  int pass = 0;
  int mercy;
  int size2 = cur_board->get_size2();
  if (!rand()%10)cur_board->shuffle_empty();
  while (pass < 2) {
    int move = wiser ? cur_board->play_wiser() : cur_board->play_random();
    cur_amaf->play(move, ++simul_len_thread);
	
    rand_movs[tid-1]++;
    if (move == Board::PASS) pass++;
    else pass = 0;
   #ifdef DEBUG_INFO
     cur_board->print_board();
   #endif

    int mercy = cur_board->mercy();
    if (mercy != -1) {
   	  score = (((1-mercy)<<1)-1)*size2/3;
      return 1-mercy;
    }

    if (simul_len_thread > 2*cur_board->get_size2()) {
      std::cerr << "WARNING: Simulation exceeded max length.\n";
      discarded++;
      return -1;
    }
  }
  
  return ((score = cur_board->score_count()) > 0);
}

void Go::back_up_results_thread(int result, Node *node_history[], int nnodes, bool side,AmafBoard *cur_amaf,float sc)
{
  int t = -1;
  for (int i = 0; i < nnodes; i++) {
  	if (node_history[i]->is_pruned) return;
    node_history[i]->update_score(t*sc);
    node_history[i]->set_results(1-result);
    node_history[i]->set_amaf(result, cur_amaf, side, i+1);
    side = !side;
    result = 1-result;
    t = -t;
  }
}

#ifdef NEED_PONDER
void Go::start_ponder(int step)
{
  if (step <= START_PONDER_STEP) return;
  is_ponder = true;
  std::cerr << "start pondering..." << std::endl;


  bool side = main_board->get_side();
  int last[2];
  Node *root = tree.get_root();
  rand_movs[0] = 0, discarded = 0;
  double aver_winrate = (lastWinRate[side] + last2WinRate[side])/2 ; 
  main_board->shuffle_empty();


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
      cid->step = step;
      cid->goo = this;
      cid->max_time_thread = 0;
      cid->tamaf = amaf_thread[i];
      slave_thread[i] = CreateThread(NULL,0,slave_runner_ponder,cid,0,NULL);
    }
    return;
}

void Go::run_thread_ponder(int max_time_thread,Board *cur_board, AmafBoard *cur_amaf,int tid,int cur_step)
{
  Node * n = NULL;
 #ifdef STD_ERR_PRINT
  std::cerr << "here!!" << std::endl;
 #endif
  rand_movs[tid-1] = 0;
  int nnode_hist = 0;
  Node *node_history[3*MAXSIZE2];
  int result =2;
  int last[2];
  bool side = -1;
  int simul_len_thread;
  int ct;
  int pruntime = 0;
  int prePunc = 0;
  int debugTime = fin_clock;
  int pass;
  double aver_winrate;
  Node *root;
  Node *node;
  float score;
  aver_winrate = (lastWinRate[main_board->get_side()] + last2WinRate[main_board->get_side()])/2 ; 
  // std::cerr << "aver_winrate:" << aver_winrate<<std::endl;
  // std::cerr << "STOP_PRIORS_WINRATE_THERESHOLD: " << STOP_PRIORS_WINRATE_THERESHOLD<<std::endl;

  while (is_ponder) {
 #ifdef STD_ERR_PRINT
  if (clock()-debugTime>0.5*CLOCKS_PER_SEC){
  debugTime = clock();
  std::cerr << "id:" << tid << " time:" << debugTime <<std::endl;
  }
 #endif

  pass = 0;
  simul_len_thread = 0;
  side = cur_board->get_side();
  root = tree.get_root();
  if (root->get_visits()>PONDER_PLAYOUTS) break;
      
  Node *node;


  cur_amaf->set_up(cur_board->get_side(), cur_board->get_size());
  node = root;
  nnode_hist = 0;

  while (node->has_childs() && pass < 2) {
    node_history[nnode_hist++] = node;
   #ifdef LOCK_UCT_SELECT
    EnterCriticalSection(&TREE_CRITICAL);
   #endif
    node = node->select_child();
   #ifdef LOCK_UCT_SELECT
    LeaveCriticalSection(&TREE_CRITICAL);
   #endif
    int move = node->get_move();
    
    if(move == Board::PASS) pass++;
    else pass = 0;
    
    cur_board->play_move(move);
        cur_amaf->play(move, ++simul_len_thread);
  }

  if (node->is_expand==0 &&(node->get_visits() >= EXPAND || node == root) ) {
    node->is_expand = tid;
    if (node->is_expand == tid){
    Prior priors[MAXSIZE2+1] = {{0,0}};
        int legal_moves[MAXSIZE2+1],nlegal;
        if(node == root){
          //====分4-10步，10-20步情况，20步后===///
        if (cur_step >= STEPS_BOUNDARY_TWO)
          nlegal = cur_board->legal_moves(legal_moves);
        else if (cur_step > STEPS_BOUNDARY_ONE && cur_step < STEPS_BOUNDARY_TWO)
          nlegal = cur_board->first_legal_moves(legal_moves,last,2,cur_step);
        else if (cur_step >= STEPS_START_END && cur_step <= STEPS_BOUNDARY_ONE)
          nlegal = cur_board->first_legal_moves(legal_moves,last,1,cur_step);
        else
          nlegal = cur_board->legal_moves(legal_moves);
      }
    else{
        nlegal = cur_board->legal_moves_origin(legal_moves);
      }

     #ifdef NEED_PRIORS
      if ((aver_winrate<STOP_PRIORS_WINRATE_THERESHOLD)&&(rand()&3)&&cur_board->get_history_length()>=8 && cur_board->get_empty_length()>20)
             cur_board->init_priors(priors);
     #endif
     
    EnterCriticalSection(&TREE_CRITICAL);
    tree.expand(node, legal_moves, nlegal, priors); 
    LeaveCriticalSection(&TREE_CRITICAL);

    }
  }

  node_history[nnode_hist++] = node;
  score = 0;

  result = play_random_game_thread(cur_board->get_history_length()>8,simul_len_thread,cur_board,cur_amaf,tid,score);
   #ifdef DEBUG_INFO
  cur_board->print_board();
  std::cerr << result << "\n";
   #endif

  cur_board->resume();
  if (result == -1) continue;
  if (side) result = 1-result,score = -score;

  EnterCriticalSection(&TREE_CRITICAL);
   #ifdef STD_ERR_PRINT
  if (nnode_hist > 1 && node_history[1] == NULL){
    std::cerr<<"CAONIDAYE" <<std::endl;
    assert(0);
    }
   #endif
  // if (nnode_hist > 1 && node_history[1] != NULL)
  //   {node_history[1]->update_score(score);}
  back_up_results_thread(result, node_history, nnode_hist, side,cur_amaf,score);
    
    LeaveCriticalSection(&TREE_CRITICAL);
  }

   #ifdef STD_ERR_PRINT
  std::cerr << rand_movs[tid-1] << " " << tid << std::endl;
   #endif
}

#endif