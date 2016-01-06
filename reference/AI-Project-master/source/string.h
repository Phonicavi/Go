#ifndef GROUPH
#define GROUPH
#include <algorithm>
#include <iostream>
#include "constants.h"

class String {
private:
	int color;
	int stones_number;
	int liberties_number;

public:
	int stones[MAX_BOARD2];
	int liberties[MAX_BOARD2 * 2 / 3];
	String();

	int get_color()  { return color; }
	int get_stones_number() const { return stones_number; }
	int get_stone(int i) const { return stones[i]; }
	const int *get_stones() const { return stones; }

	int get_liberties_number() const { return liberties_number; }
	int get_liberty(int i) const { return liberties[i]; }
	const int *get_liberties() const { return liberties; }
	void clear();
	int String::remove_liberty(int lib);
	int add_liberty(int lib);
	void merge_string(String *attached);
	void set_up(int point, int c, const int *liberties, int num_liberties);
};

#endif
