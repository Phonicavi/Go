#include <iostream>
#include <ctime>
#include "Go.h"

#define DEF_PLAYOUTS 3000000
#define DEF_TREESIZE 6000000

Go::Go(Board *goban):tree(DEF_TREESIZE, goban), amaf(goban->get_size())
{
  main_goban = goban;
  max_playouts = DEF_PLAYOUTS;
  max_time = 3*CLOCKS_PER_SEC;
  //max_time = INFINITE;
  tree_size = DEF_TREESIZE;
  rand_movs = 0;
}

void Go::reset()
{
  rand_movs = 0;
  tree.clear();
  amaf.set_up(main_goban->get_side(), main_goban->get_size());
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
  //if (rand()%100)main_goban->shuffle_empty();
  while (pass < 2) {
    int move = heavy ? main_goban->play_heavy() : main_goban->play_random();
    amaf.play(move, ++simul_len);
    rand_movs++;
    if (move == Board::PASS) pass++;
    else pass = 0;
#ifdef DEBUG_INFO
      main_goban->print_goban();
#endif
   if (main_goban->get_empty_length() < 60){
    int mercy = main_goban->mercy();
    if (mercy != -1) {
      return 1-mercy;
    }
  }
//#ifdef DEBUG_INFO
    if (simul_len > 2*main_goban->get_size2()) {
      std::cerr << "WARNING: Simulation exceeded max length.\n";
      discarded++;
      return -1;
    }
//#endif
  }
    return (main_goban->score_count() > 0) ? 1:0;
}

int Go::generate_move(bool early_pass)
{
  const int EXPAND = 8;
  // const double RESIGN_THRESHOLD = 0.10, PASS_THRESHOLD = 0.90;

  bool side = main_goban->get_side();
  Node *root = tree.get_root();
  rand_movs = 0, discarded = 0;
  fin_clock = clock();
   main_goban->backup();

  while (clock() - fin_clock < max_time) {
    Node *node_history[3*MAXSIZE2];
    int nnode_hist = 0, pass = 0;
    simul_len = 0;
    amaf.set_up(main_goban->get_side(), main_goban->get_size());
    Node *node = root;
    while (node->has_childs() && pass < 2) {
      node_history[nnode_hist++] = node;
      node = node->select_child();
      int move = node->get_move();
      if(move == Board::PASS) pass++;
      else pass = 0;
      main_goban->play_move(move);
      amaf.play(move, ++simul_len);
    }
    if (node->get_visits() >= EXPAND || node == root) {
      Prior priors[MAXSIZE2+1] = {{0,0}};
      int legal_moves[MAXSIZE2+1];
      int nlegal = main_goban->legal_moves(legal_moves);
      if (main_goban->get_history_length()>10)
       main_goban->init_priors(priors);
      tree.expand(node, legal_moves, nlegal, priors); //TODO: break if expand fails.
    }
    node_history[nnode_hist++] = node;
    int result;
    //  if (main_goban->get_history_length()<8)
    //  result = play_random_game(LIGHT);
    // else 
     result = play_random_game(HEAVY);//Black wins.
#ifdef DEBUG_INFO
      main_goban->print_goban();
      std::cerr << result << "\n";
#endif
     // main_goban->restore();
      main_goban->resume();
    if (result == -1) continue;
    if (side) result = 1-result;
   back_up_results(result, node_history, nnode_hist, side);
#ifdef DEBUG_INFO
      tree.print();
#endif
  }
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
  std::cerr << "#Playouts: " << nplayouts
            << ", average length: " << rand_movs/nplayouts
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
    main_goban->score_area(score_table);
    main_goban->restore();
  }
  for (int i = 1; i <= main_goban->get_size2(); i++) {
    if (score_table[i] > PLAYOUTS/2) score_table[i] = 1;
    else if (score_table[i] < -PLAYOUTS/2) score_table[i] = -1;
    else score_table[i] = 0;

    if (dead && main_goban->get_value(i)
        && score_table[i] !=  main_goban->get_value(i)) {
      dead->insert(dead->end(), i);
    }
  }

  for (int i = 1; i <= main_goban->get_size2(); i++) {
    score += score_table[i];
  }
  return score - main_goban->get_komi();
}

void Go::perft(int max)
{
  for (int i = 0; i < max; i++) {
    simul_len = 0;
    play_random_game(LIGHT);
    std::cerr << "restoring\n";
    main_goban->restore();
  }
}
