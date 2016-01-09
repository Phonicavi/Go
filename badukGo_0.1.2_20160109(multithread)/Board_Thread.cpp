#include "Board.h"
#include "Go.h"

#define BACKUP_NEED_CRITICAL_LIMIT_EMPTY 90

void Board::copy_from(Board *board){
   //std::cerr<<"begin backup" <<std::endl;
  side_bu = board->side;
  ko_point_bu = board->ko_point;
  size_bu = board->size, size2_bu = board->size2;
  komi_bu = board->komi;
  handicap_bu = board->handicap;
  empty_points_bu = board->empty_points;
  game_history = board->game_history;
  GroupFather = board->GroupFather;
  // std::cerr<<"here backup" <<std::endl;
  for (unordered_set<int>::iterator it = (GroupFather).begin(); it != (GroupFather).end(); ++it){
      //  std::cerr<<"backup_father " << *it <<std::endl;//
        groups_bu[*it] = board->groups[*it];
    }
  // std::cerr<<"here1 backup" <<std::endl;
  stones_on_board_bu[1] = board->stones_on_board[1];
  stones_on_board_bu[0] = board->stones_on_board[0];
  last_point_bu = board->last_point, last_point2_bu = board->last_point2;
  last_atari_black_bu = board->last_atari[0]?board->last_atari[0]->get_father():0;
  last_atari_white_bu = board->last_atari[1]?board->last_atari[1]->get_father():0;
  // std::cerr<<"end backup" <<std::endl;
  resume();
}


void Go::run_thread(int max_time_thread,Board *cur_board, AmafBoard *cur_amaf,int tid){
std::cerr << "here!!" << std::endl;
	  rand_movs[tid-1] = 0;
	  int nnode_hist = 0;
	  Node *node_history[3*MAXSIZE2];
	  int result =2;
	  bool side = -1;
	  int simul_len_thread;
	  while (clock() - fin_clock < max_time_thread) {
	    

	    int pass = 0;
	    simul_len_thread = 0;
	    side = cur_board->get_side();
	  	Node *root = tree.get_root();
	  	cur_amaf->set_up(cur_board->get_side(), cur_board->get_size());


	    Node *node = root;
	    nnode_hist = 0;
	     memset(node_history,0,sizeof node_history);


	    

	    // if (nnode_hist != 0)back_up_results_thread(result, node_history, nnode_hist, side,cur_amaf);
	    

	     //EnterCriticalSection(&TREE_CRITICAL);
	    while (node->has_childs() && pass < 2) {
	      node_history[nnode_hist++] = node;
	      node = node->select_child();
	      
	      int move = node->get_move();
	      if(move == Board::PASS) pass++;
	      else pass = 0;
	      cur_board->play_move(move);
	      // simul_len_thread;
	      cur_amaf->play(move, ++simul_len_thread);
	    }


	    EnterCriticalSection(&TREE_CRITICAL);
	    if (node->get_visits() >= EXPAND || node == root) {
	      Prior priors[MAXSIZE2+1] = {{0,0}};
	      int legal_moves[MAXSIZE2+1];
	      int nlegal = cur_board->legal_moves(legal_moves);
	      if ((rand()&3)&&cur_board->get_history_length()>8 && cur_board->get_empty_length()>20)
	       cur_board->init_priors(priors);
	      tree.expand(node, legal_moves, nlegal, priors); //TODO: break if expand fails.
	    }

	    LeaveCriticalSection(&TREE_CRITICAL);


	    node_history[nnode_hist++] = node;
	      // if (cur_board->get_history_length()<8)
	      result = play_random_game_thread(cur_board->get_history_length()>8,simul_len_thread,cur_board,cur_amaf,tid);
	     // else 
	     // result = play_random_game_thread(HEAVY,simul_len_thread,cur_board,cur_amaf,tid);//Black wins.
	#ifdef DEBUG_INFO
	     cur_board->print_board();
	      std::cerr << result << "\n";
	#endif
	     // cur_board->restore();
	      cur_board->resume();
	    if (result == -1) continue;
	    if (side) result = 1-result;
	  // if (main_board->get_empty_length() > BACKUP_NEED_CRITICAL_LIMIT_EMPTY)
	  	EnterCriticalSection(&TREE_CRITICAL);
	  back_up_results_thread(result, node_history, nnode_hist, side,cur_amaf);
	  // if (main_board->get_empty_length() > BACKUP_NEED_CRITICAL_LIMIT_EMPTY)
	  	LeaveCriticalSection(&TREE_CRITICAL);
	}
	 // if (nnode_hist != 0)back_up_results_thread(result, node_history, nnode_hist, side,cur_amaf);
	std::cerr << rand_movs[tid-1] << " " << tid << std::endl;
}

int Go::play_random_game_thread(bool heavy, int simul_len_thread, Board *cur_board, AmafBoard *cur_amaf,int tid){
  int pass = 0;
  int mercy;
  if (!rand()%5)cur_board->shuffle_empty();
  while (pass < 2) {
    int move = heavy ? cur_board->play_heavy() : cur_board->play_random();
    cur_amaf->play(move, ++simul_len_thread);
    // ++simul_len_thread;
    rand_movs[tid-1]++;
    if (move == Board::PASS) pass++;
    else pass = 0;
#ifdef DEBUG_INFO
     cur_board->print_board();
#endif
   // if (cur_board->get_empty_length() < 60){
    int mercy = cur_board->mercy();
    if (mercy != -1) {
      return 1-mercy;
    }
  // }
//#ifdef DEBUG_INFO
    if (simul_len_thread > 2*cur_board->get_size2()) {
      std::cerr << "WARNING: Simulation exceeded max length.\n";
      discarded++;
      return -1;
    }
//#endif
  }
    return (cur_board->score_count() > 0);
}

void Go::back_up_results_thread(int result, Node *node_history[], int nnodes, bool side,AmafBoard *cur_amaf)
{
  for (int i = 0; i < nnodes; i++) {
    node_history[i]->set_results(1-result); //Because root node belongs to opponent.
    node_history[i]->set_amaf(result, cur_amaf, side, i+1);
    side = !side;
    result = 1-result;
  }
}
