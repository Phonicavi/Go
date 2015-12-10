/* main.cpp */
#include <iostream>
#include <cstring>
#include "Board.h"
#include "Node.h"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        cout << "Help! I need somebody. Help! Not just anybody. Help!!" << endl;
        exit(0);
    }

    Board board;

    srand(time(0));
    int moves = 0;
    int x = 0;
    int y = 0;

    while(moves < 30) {
	moves++;
        board.UCTSearch(10000);
        cout << board << "------------------" << endl;
        cin >> x;
	cin >> y;
	while(!board.makePlayerMove(x,y)) {
		cout << "Illegal play!" << endl << "Try a different move" << endl;
		cin >> x;
		cin >> y;

	}

        cout << board << "------------------" << endl;
    }

    board.influence();
    cout << "OK" << endl;
    cout << "And the winner is... player " << board.getWinner() << endl;
    return 0;
}
