#include <ctime>
#include "gtp.h"
#include "zobrist.h"
// #include "Mutex.h"
#include "Go.h"
#include <fstream>
#include <random>
#include <ctime>



int main()
{
	// std::ofstream out("record.txt");

	// out<<""<<endl;
	// out.close();
	// // return 0;
  // TREE_MUTEX = CreateMutex(NULL,false,NULL);
  srand(unsigned(time(NULL)));
	// std::mt19937 rand(time(0));
  //init_genrand64(16669666165875248481ULL);

  GTP gtp;
  //gtp.perft(100000);
  // std::cerr << "bf loop" << std::endl;
  gtp.GTP_loop();
  // std::cerr << "af loop" << std::endl;
  return 0;
}
