#ifndef AMAFH
#define AMAFH
#include "size.h"
#include <iostream>
#include <iomanip>

class AmafBoard {
private:
	int board[MAXSIZE2 + 1];
	int size;
	

public:
	bool side;
	AmafBoard(int sz)
	{
		size = sz <= MAXSIZE ? sz : MAXSIZE;
		side = 0;
		clear();
	}

	void clear()
	{
		for (int i = 0; i <= size*size; i++) {
			board[i] = 0;
		}
	}

	void set_up(bool sd, int sz)
	{
		size = sz;
		side = sd;
		clear();
	}

	int play(int coord, int depth)
	{
		if (board[coord] == 0) {
			board[coord] = side ? -depth : depth;
			side = !side;
			return 0;
		}
		else {
			side = !side;
			return -1;
		}
	}

	double value(int coord, int depth, bool side, double discount) const
	{
		if (coord == 0) return 0.0;
		int val = side ? -board[coord] : board[coord];
		if (val >= depth) {
			return 1.0 - discount*val;
		}
		else {
			return 0;
		}
	}

	void print() const {
		for (int y = size - 1; y > -1; y--) {
			std::cerr << "  |";
			for (int x = 1; x < size + 1; x++) {

				if (board[size*y + x] < 0) {
					std::cerr << std::setw(3) << board[size*y + x] << "|";
				}
				else if (board[size*y + x] > 0) {
					std::cerr << std::setw(3) << board[size*y + x] << "|";
				}
				else {
					std::cerr << "   |";
				}
			}
			std::cerr << "|" << "\n";
		}
		std::cerr << "pass: " << board[0] << "\n";
	}
};
#endif
