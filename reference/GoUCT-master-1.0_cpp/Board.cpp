/* Board.cpp */

#include "Board.h"

/* Board class */
Board::Board() {
	this->player = 1;
	this->root = new Node(-1, -1);
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			this->b[i][j] = 0;
		}
	}
	this->movements = 0;
	this->moku1 = 0;
	this->moku2 = 6.5;
	this->ko = false;
}

Board::~Board() {
	delete(this->root);
}

/* Changes the current player */
void Board::changePlayer() {
	this->player = 3 - this->player;
}

/* Select the child from a Node with the most number of simulations */
Node* Board::getBestChild(Node& root) {
	Node* child = root.getChild();
	Node* best_child = NULL;
	int best_visits = -1;
	int best_wins = -1;
	while(child != NULL) {
		if(child->getVisits() >= best_visits && child->getWins() > best_wins) {
			best_child = child;
			best_visits = child->getVisits();
			best_wins = child->getWins();
		} else if(child->getVisits() == best_visits &&
				child->getWins() == best_wins && rand()%2 == 0) {
			best_child = child;
			best_visits = child->getVisits();
			best_wins = child->getWins();
		}
		child = child->getSibling();
	}

	// Unattach previous best sibling for memory free
	child = root.getChild();
	if(child == best_child) root.setChild(NULL);
	else {
		while(child->getSibling() != best_child) child = child->getSibling();
		child->setSibling(NULL);
	}
	return best_child;
}

/* Select the best child from a Node based on UCT value */
Node* Board::UCTSelect(Node& node) {
	Node* res = NULL;
	Node* next = node.getChild();
	double best_uct = 0;

	while(next != NULL) {
		double uctvalue;
		if(next->getVisits() > 0) {
			double winrate = next->getWinRate();
			double uct = 0.44 * sqrt(log(node.getVisits()) / next->getVisits());
			uctvalue = winrate + uct;
		}
		else {
			uctvalue = 10000 + 1000*(rand()%1000);
		}

		if(uctvalue > best_uct) {
			best_uct = uctvalue;
			res = next;
		} else if(uctvalue == best_uct && rand()%2 == 0) {
			best_uct = uctvalue;
			res = next;
		}

		next = next->getSibling();
	}

	return res;
}

/* Plays a game simulation and returns who was the winner saving the wins and
 * games on the tree nodes */
int Board::playSimulation(Node& node) {
	int randomresult = 0;
	if(node.getChild() == NULL && node.getVisits() < 10) {
		randomresult = playRandomGame();
	}
	else {
		if(node.getChild() == NULL) createChildren(node);

		Node* next = UCTSelect(node);
		makeMove(next->getX(), next->getY());

		int res = playSimulation(*next);
		randomresult = 1-res;
	}

	node.update(1-randomresult);
	return randomresult;
}

/* Makes simulations and plays the best move possible */
void Board::UCTSearch(int time) {
	if(this->root->getChild() == NULL)
		createChildren(*this->root);

	Board clone;
	for(int i = 0; i < time; i++) {
		clone.copyStateFrom(this);
		clone.playSimulation(*this->root);
	}

	Node* n = getBestChild(*this->root);
	Node* todelete = this->root;
	this->root = n;
	delete(todelete);
	delete(this->root->getSibling());
	this->root->setSibling(NULL);
	std::cout << "(" << n->getX() << "," << n->getY() << ")" << std::endl;
	makeMove(n->getX(), n->getY());
}

/* Creates all possible moves from root Node */
bool Board::createChildren(Node& root) {
	Node* last = NULL;
	bool ret = false;
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			if(isLegalPlay(i, j)) {
				ret = true;
				if(last == NULL) {
					root.setChild(new Node(i,j));
					last = root.getChild();
				}
				else {
					last->setSibling(new Node(i,j));
					last = last->getSibling();
				}
			}
		}
	}

	return ret;
}

/* Checks if the move is legal */
bool Board::isLegalPlay(int x, int y) {
	bool suicide;
	bool kills = false;

	// Check if there is a stone already
	if(this->b[x][y] != 0) return false;
	// Check if suicide play and ko
	this->b[x][y] = this->player;
	if(this->ko == true) {
		if((koX == 0 || b[koX-1][koY] == this->player)
		 && (koY == BOARD_SIZE-1 || b[koX][koY+1] == this->player)
		 && (koX == BOARD_SIZE-1 || b[koX+1][koY] == this->player)
		 && (koY == 0 || b[koX][koY-1] == this->player)) {
			this->b[x][y] = 0;
			return false;
		}
	}
	suicide = isDead(x, y);
	// Check if kills any opponent stones
	kills = ((x == 0 || this->player != 3-b[x-1][y] ? false : isDead(x-1,y))
		|| (y == BOARD_SIZE-1 || this->player != 3-b[x][y+1] ? false : isDead(x,y+1))
		|| (x == BOARD_SIZE-1 || this->player != 3-b[x+1][y] ? false : isDead(x+1,y))
		|| (y == 0 || this->player != 3-b[x][y-1] ? false : isDead(x,y-1)));

	this->b[x][y] = 0;
	return ((!suicide) || kills);
}

/* Copies the board state */
void Board::copyStateFrom(const Board* orig) {
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			this->b[i][j] = orig->b[i][j];
		}
	}
	this->player = orig->player;
	this->moku1 = orig->moku1;
	this->moku2 = orig->moku2;
	this->movements = orig->movements;
	this->ko = orig->ko;
	this->koX = orig->koX;
	this->koY = orig->koY;
}

/* Plays a move on the board with the coordinates (x,y) */
void Board::makeMove(int x, int y) {
	if(isLegalPlay(x,y)) {
		int captures = 0;
		this->ko = false;
		this->b[x][y] = this->player;
		this->changePlayer();
		// Checks and removes if move captures any stone
		// Checks north
		if(x > 0 && b[x-1][y] == this->player && isDead(x-1,y)) {
			captures += removeGroup(x-1,y);
		}
		// Checks east
		if(y < BOARD_SIZE-1 && b[x][y+1] == this->player && isDead(x,y+1)) {
			captures += removeGroup(x,y+1);
		}
		// Checks south
		if(x < BOARD_SIZE-1 && b[x+1][y] == this->player && isDead(x+1,y)) {
			captures += removeGroup(x+1,y);
		}
		// Checks west
		if(y > 0 && b[x][y-1] == this->player && isDead(x,y-1)) {
			captures += removeGroup(x,y-1);
		}
		if(captures == 1) {
			this->ko = true;
			this->koX = x;
			this->koY = y;
		}
		if(this->player == 1) moku2 += captures;
		else moku1 += captures;
		this->movements++;
	}
}

/* Random chooses a position on the board to play. Tries until a legal move
 * is found */
void Board::makeRandomMove() {

	int x = rand()%BOARD_SIZE;
	int y = rand()%BOARD_SIZE;
	while(!isLegalPlay(x, y)) {
		x = rand()%BOARD_SIZE;
		y = rand()%BOARD_SIZE;
	}

	this->makeMove(x, y);
}

/* Implements human player moves */
bool Board::makePlayerMove(int x, int y) {

	/* Outside the board */
	if((x < 0) || (x >= BOARD_SIZE) || (y < 0) || (y >= BOARD_SIZE)) {
		return false;
	}

	if(isLegalPlay(x,y)) {
		int captures = 0;
		this->ko = false;
		this->b[x][y] = this->player;
		this->changePlayer();
		// Checks and removes if move captures any stone
		// Checks north
		if(x > 0 && b[x-1][y] == this->player && isDead(x-1,y)) {
			captures += removeGroup(x-1,y);
		}
		// Checks east
		if(y < BOARD_SIZE-1 && b[x][y+1] == this->player && isDead(x,y+1)) {
			captures += removeGroup(x,y+1);
		}
		// Checks south
		if(x < BOARD_SIZE-1 && b[x+1][y] == this->player && isDead(x+1,y)) {
			captures += removeGroup(x+1,y);
		}
		// Checks west
		if(y > 0 && b[x][y-1] == this->player && isDead(x,y-1)) {
			captures += removeGroup(x,y-1);
		}
		if(captures == 1) {
			this->ko = true;
			this->koX = x;
			this->koY = y;
		}
		if(this->player == 1) moku2 += captures;
		else moku1 += captures;
		this->movements++;
	
		Node* move = this->root->getChild();
		if(move->getX() == x && move->getY() == y) {
			this->root->setChild(NULL);
			Node* todelete = this->root;
			this->root = move;
			delete(todelete);
			delete(this->root->getSibling());
			this->root->setSibling(NULL);
			
		}
		else {
			Node* sibling = move->getSibling();
			while(sibling->getX() != x || sibling->getY() != y) {
				move = move->getSibling();
				sibling = sibling->getSibling();
			}
			move->setSibling(NULL);
			Node* todelete = this->root;
			this->root = sibling;
			delete(todelete);
			delete(this->root->getSibling());
			this->root->setSibling(NULL);
		}
	
		return true;
	}
	return false;
}

/* Plays random moves until endgame and returns the winner */
int Board::playRandomGame() {
	int cur_player = this->player;
	while(!isFinished()) {
		this->makeRandomMove();
	}

	return influence() == cur_player ? 1 : 0;
}

/* Checks the board state and returns if the game is finished */
bool Board::isFinished() {
	if(this->movements > 65) return true;
	return false;
}

/* Checks the endgame board state and returns the winner of the game */
int Board::getWinner() {
	Board clone;
	int p1pts = 0, p2pts = 0;
	int value;

	clone.copyStateFrom(this);

	/* Extends players territories with their stones */
	for(int k = 0; k < TERR_ITER_NUMBER; k++) {
		for(int i = 0; i < BOARD_SIZE; i++) {
			for(int j = 0; j < BOARD_SIZE; j++) {
				if(clone.b[i][j] == 0) {
					clone.b[i][j] = clone.own(i, j);
				}
			}
		}
	}

	/* Counts each player's stones */
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			value = clone.own(i, j);
			switch(value) {
				case 1:
					p1pts++;
					break;
				case 2:
					p2pts++;
					break;
			}
		}
	}

	/* Player 1 wins */
	if(p1pts > p2pts) {
		return 1;
	}

	/* Player 2 wins */
	else if(p1pts < p2pts) {
		return 2;
	}

	/* Draw */
	else {
		return 0;
	}
}

/* Verifies and returns which player owns (approximately) a board position */
int Board::own(int x, int y) {
	/* Trivial case: there is a player's stone */
	if(this->b[x][y] == 1) {
		return 1;
	}
	else if(this->b[x][y] == 2) {
		return 2;
	}

	/* Case where there is no stone: verify adjacent positions */
	else {
		int p1stones = 0, p2stones = 0;
		int value;
		if(x > 0) {
			value = this->b[x-1][y];
			this->updateStones(value, p1stones, p2stones);
		}
		if(y < BOARD_SIZE-1) {
			value = this->b[x][y+1];
			this->updateStones(value, p1stones, p2stones);
		}
		if(x < BOARD_SIZE-1) {
			value = this->b[x+1][y];
			this->updateStones(value, p1stones, p2stones);
		}
		if(y > 0) {
			value = this->b[x][y-1];
			this->updateStones(value, p1stones, p2stones);
		}

		if(p1stones > p2stones) {
			return 1;
		}
		else if(p1stones < p2stones) {
			return 2;
		}
		else {
			return 0;
		}
	}
}

/* Updates the number of stones around a board position */
void Board::updateStones(int value, int& p1stones, int& p2stones) {
	switch(value) {
		case 1:
			p1stones++;
			break;
		case 2:
			p2stones++;
			break;
	}
}

/* Checks if a group of pieces is dead */
bool Board::isDead(int x, int y) {
	// first checks if there is any liberty on the sides
	if((x > 0 && b[x-1][y] == 0) || (y < BOARD_SIZE-1 && b[x][y+1] == 0) ||
	 (x < BOARD_SIZE-1 && b[x+1][y] == 0) || (y > 0 && b[x][y-1] == 0)) {
		return false;
	}
	bool north = true;
	bool east = true;
	bool south = true;
	bool west = true;

	int group = b[x][y];
	/* recursively checks other stones of the same group */

	// marks as already visited
	b[x][y] *= -1;

	// checks north
	if(x > 0 && group == b[x-1][y]) {
		north = isDead(x-1, y);
		
	}
	// checks east
	if(y < BOARD_SIZE-1 && group == b[x][y+1]) {
		east = isDead(x, y+1);
	}
	// checks south
	if(x < BOARD_SIZE-1 && group == b[x+1][y]) {
		south = isDead(x+1, y);
	}
	// checks west
	if(y > 0 && group == b[x][y-1]) {
		west = isDead(x, y-1);
	}

	// returns board to original state
	b[x][y] *= -1;

	return (north && west && south && east);
}

/* Removes a group of stones from the board */
int Board::removeGroup(int x, int y) {
	int group = b[x][y];

	int north = 0;
	int east = 0;
	int south = 0;
	int west = 0;

	b[x][y] = 0;
	// removes north if from the same group
	if(x > 0 && b[x-1][y] == group) north = removeGroup(x-1,y);
	// removes east if from the same group
	if(y < BOARD_SIZE-1 && b[x][y+1] == group) east = removeGroup(x,y+1);
	// removes south if from the same group
	if(x < BOARD_SIZE-1 && b[x+1][y] == group) south = removeGroup(x+1,y);
	// removes west if from the same group
	if(y > 0 && b[x][y-1] == group) west = removeGroup(x,y-1);

	return 1+north+east+south+west;
}

/* Calculates board influence using Zorobris algorithm */
int Board::influence() {
	Board clone;
	Board temp;
	int winner = 0;

	clone.copyStateFrom(this);
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			if(clone.b[i][j] != 0) clone.b[i][j] = clone.b[i][j] == 1 ? 50 : -50;
		}
	}
	temp.copyStateFrom(&clone);
	for(int t = 0; t < 4; t++) {
		for(int i = 0; i < BOARD_SIZE; i++) {
			for(int j = 0; j < BOARD_SIZE; j++) {
				int north = i == 0 ? 0 : clone.b[i-1][j];
				int east = j == BOARD_SIZE-1 ? 0 : clone.b[i][j+1];
				int south = i == BOARD_SIZE-1 ? 0 : clone.b[i+1][j];
				int west = j == 0 ? 0 : clone.b[i][j-1];
				temp.b[i][j] += north + east + south + west;
			}
		}
		clone.copyStateFrom(&temp);
	}
	
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			if(clone.b[i][j] > 500) winner++;
			else if(clone.b[i][j] < -500) winner--;
		}
	}
	winner += (int)this->moku1%1;
	winner -= (int)this->moku2%1;

	return winner == 0 ? 0 : (winner > 0 ? 1 : 2);
}

std::ostream & operator<<(std::ostream & os, const Board &board) {
	using namespace std;

	os << "  ";
	for(int i = 0; i < BOARD_SIZE; i++) os << i << " ";
	os << endl;
	for(int i = 0; i < BOARD_SIZE; i++) {
		os << i << " ";
		for(int j = 0; j < BOARD_SIZE; j++) {
			switch(board.b[i][j]) {
			case 0:
				os << ". ";
				break;
			case 1:
				os << "X ";
				break;
			case 2:
				os << "O ";
				break;
			}
		}
		os << endl;
	}
	os << "Captures1: " << board.moku1 << endl;
	os << "Captures2: " << board.moku2 << endl;
	return os;
}
