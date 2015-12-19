#include <iostream>
#include <cstdlib>

#include <unordered_set>

class Te
{
private:
	std::unordered_set<int> ust;
public:
	Te() { ust.clear();}
	void add(int a) { ust.insert(a);}
	void remove(int a) { ust.erase(a);}
	class Re
	{
	private:
		Te *p_t;
		std::unordered_set<int>::iterator it;
	public:
		Re(Te *t) {
			it = (t->ust).begin();
			p_t = t;
		}
		operator bool() const {
			return it != (p_t->ust).end();
		}
		int operator*() const {
			return *it;
		}
		void operator++() {
			++it;
		}
	};
};


int main(int argc, char const *argv[])
{
	// std::unordered_set<int> ust;
	Te tee;
	Te::Re ree(&tee);
	return 0;
}