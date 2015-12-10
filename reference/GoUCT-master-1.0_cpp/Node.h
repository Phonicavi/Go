/* Node.h */

#ifndef __NODE_H__
#define __NODE_H__

class Node {
	private:
		int wins;
		int visits;

		int x, y; /* Position of move */

		Node *child;
		Node *sibling;

	public:
		Node(int x, int y);
		virtual ~Node();
		void update(int val);
		double getWinRate();
		int getWins() const;
		int getVisits() const;
		int getX() const;
		int getY() const;
		Node *getChild() const;
		void setChild(Node *);
		Node *getSibling() const;
		void setSibling(Node *);
};

#endif /* __NODE_H__ */
