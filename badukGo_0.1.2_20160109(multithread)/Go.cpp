#include <iostream>
#include <ctime>
#include "Go.h"

#define FIRST_LEVEL_EMPTY 70
#define SECOND_LEVEL_EMPTY 35
// #define CHANGE_THEARD_NUM


#define DEF_PLAYOUTS 3000000
#define DEF_TREESIZE 6000000

// extern HANDLE TREE_MUTEX;

Go::Go(Board *board):tree(DEF_TREESIZE, board), amaf(board->get_size())
{
  InitializeCriticalSection(&TREE_CRITICAL);
  SYSTEM_INFO sinfo;
  GetSystemInfo(&sinfo);
  SYS_THREAD_LIMIT = min((int)sinfo.dwNumberOfProcessors,6);
  real_thread_num = SYS_THREAD_LIMIT;
  // SYS_THREAD_LIMIT = 6;
   std::cerr << "why" << SYS_THREAD_LIMIT<< std::endl;
  main_board = board;
  max_playouts = DEF_PLAYOUTS;
  max_time = 2.99*CLOCKS_PER_SEC;
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


int Go::play_random_game(bool heavy)
{
  int pass = 0;
  int mercy;
  if (!rand()%3)main_board->shuffle_empty();
  while (pass < 2) {
    int move = heavy ? main_board->play_heavy() : main_board->play_random();
    amaf.play(move, ++simul_len);
   // ++simul_len;
    rand_movs[0]++;
    if (move == Board::PASS) pass++;
    else pass = 0;
#ifdef DEBUG_INFO
      main_board->print_board();
#endif
   // if (main_board->get_empty_length() < 60){
    int mercy = main_board->mercy();
    if (mercy != -1) {
      return 1-mercy;
    }
  // }
//#ifdef DEBUG_INFO
    if (simul_len > 2*main_board->get_size2()) {
      std::cerr << "WARNING: Simulation exceeded max length.\n";
      discarded++;
      return -1;
    }
//#endif
  }
    return (main_board->score_count() > 0);
}

int Go::generate_move(bool early_pass)
{

  // const double RESIGN_THRESHOLD = 0.10, PASS_THRESHOLD = 0.90;

  bool side = main_board->get_side();
  Node *root = tree.get_root();
  rand_movs[0] = 0, discarded = 0;

  fin_clock = clock();
  // main_board->backup();

#ifdef CHANGE_THEARD_NUM
  int empty_len = main_board->get_empty_length();
  if (empty_len > FIRST_LEVEL_EMPTY){
    real_thread_num = SYS_THREAD_LIMIT;
  }else if (empty_len > SECOND_LEVEL_EMPTY){
    real_thread_num = max(1,SYS_THREAD_LIMIT/3);
  }else real_thread_num = 1;

#endif

  if (real_thread_num > 1){
    for (int i=0; i<real_thread_num; ++i){
      struct id *cid = (struct id*) malloc(sizeof(struct id));
      cid->thread_id = i+1;
      cid->orig_bd = main_board;
      cid->cur_bd = thread_board[i];
      cid->goo = this;
      cid->max_time_thread = max_time;
      cid->tamaf = amaf_thread[i];
      slave_thread[i] = CreateThread(NULL,0,slave_runner,cid,0,NULL);
    }
    WaitForMultipleObjects(real_thread_num,slave_thread,true,INFINITE);
    for (int i=0; i<real_thread_num; ++i){
      CloseHandle(slave_thread[i]);
    }
  }

  else{
      main_board->backup();

      while (clock() - fin_clock < max_time) {
        Node *node_history[3*MAXSIZE2];
        int nnode_hist = 0, pass = 0;
        simul_len = 0;
        amaf.set_up(main_board->get_side(), main_board->get_size());

        Node *node = root;



        while (node->has_childs() && pass < 2) {
          node_history[nnode_hist++] = node;
          node = node->select_child();
          int move = node->get_move();
          if(move == Board::PASS) pass++;
          else pass = 0;
          main_board->play_move(move);
          amaf.play(move, ++simul_len);
          //++simul_len;
        }
        if (node->get_visits() >= EXPAND || node == root) {
          Prior priors[MAXSIZE2+1] = {{0,0}};
          int legal_moves[MAXSIZE2+1];
          int nlegal = main_board->legal_moves(legal_moves);
          if ((rand()&3)&&main_board->get_history_length()>8 && main_board->get_empty_length()>20)
           main_board->init_priors(priors);
          tree.expand(node, legal_moves, nlegal, priors); //TODO: break if expand fails.
        }

       // ReleaseMutex(TREE_MUTEX);


        node_history[nnode_hist++] = node;
        int result;
          // if (main_board->get_history_length()<8)
          result = play_random_game(main_board->get_history_length()>8);
         // else 
         // result = play_random_game(HEAVY);//Black wins.
    #ifdef DEBUG_INFO
          main_board->print_board();
          std::cerr << result << "\n";
    #endif
         // main_board->restore();
          main_board->resume();
        if (result == -1) continue;
        if (side) result = 1-result;
       // WaitForSingleObject(TREE_MUTEX,INFINITE);
       back_up_results(result, node_history, nnode_hist, side);

    #ifdef DEBUG_INFO
          tree.print();
    #endif
      }
  }
  // for (int i=0; i<real_thread_num; ++i){
  //    WaitForSingleObject(slave_thread[i],INFINITE);
  // }
  // WaitForMultipleObjects(real_thread_num,slave_thread,true,INFINITE);
  Node *best = tree.get_best();
  print_PV();
  if (best->get_move() == Board::PASS) return Board::PASS;
  //if (best->get_value(1) < RESIGN_THRESHOLD) return -1;
  //if (early_pass && best->get_value(1) >= PASS_THRESHOLD && !root->get_move()) return Board::PASS;
  std::cerr << best->get_value(1) <<std::endl;


  return best->get_move();
}

void Go::back_up_results(int result, Node *node_history[], int nnodes, bool side)
{
  for (int i = 0; i < nnodes; i++) {
    node_history[i]->set_results(1-result); //Because root node belongs to opponent.
    node_history[i]->set_amaf(result, amaf, side, i+1);
    side = !side;
    result = 1-result;
  }
}

void Go::print_PV() const
{
  fin_clock = clock() - fin_clock;
  tree.print();
  int nplayouts = tree.get_root()->get_visits();
  int rand_movettl = 0;
  for (int i=0; i<MAX_THREAD_LIMIT; ++i) rand_movettl += rand_movs[i];
  std::cerr << "#Playouts: " << nplayouts
            << ", history_length: " << main_board->get_history_length()
            << ", empty_length: " << main_board->get_empty_length()
            << ", average length: " << (rand_movettl)/nplayouts
            << ", discarded: " << discarded << ", playouts/sec: "
            << (float)nplayouts/fin_clock*CLOCKS_PER_SEC << "\n";
}

float Go::score(std::vector<int> *dead)
{
  const int PLAYOUTS = 5000;
  int score = 0;
  int score_table[MAXSIZE2+1] = {0};
  for (int i = 0; i < PLAYOUTS; i++) {
    simul_len = 0;
    play_random_game(LIGHT);
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
  for (int i = 0; i < max; i++) {
    simul_len = 0;
    play_random_game(LIGHT);
    std::cerr << "restoring\n";
    main_board->restore();
  }
}


DWORD WINAPI slave_runner(void *args){

  struct id * cid = (id*)args;
  srand(time(NULL) * cid->thread_id+1);
  cid->cur_bd->copy_from(cid->orig_bd);
  cid->goo->run_thread(cid->max_time_thread,cid->cur_bd,cid->tamaf,cid->thread_id);
  return 0;
}