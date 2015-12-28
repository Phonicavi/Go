#ifndef ZOBRISTH
#define ZOBRISTH

#include "mersenne.h"
#include "base.h"

class Zobrist{
private:
  unsigned long long zob_key;
  unsigned long long zob_side;
  unsigned long long zob_points[2][MAXSIZE2];
  unsigned long long zob_ko[MAXSIZE2];
  unsigned long long zob_history[6];
  int active;
public:
  Zobrist();
  unsigned long long get_key() const;
  void set_key(unsigned long long);
  void reset();
  void toggle_side();
  void toggle_ko(int);
  void update(int, bool);
  void update(const int*, const int);
  void record_key();
  void clear_history();
  bool check_history(unsigned long long) const;
};
#endif
