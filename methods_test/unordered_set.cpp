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


int main()
{
	// std::unordered_set<int> ust;
	Te tee;
	for (int i = 0; i < 10; ++i) {
		// int x = rand()%17;
		std::cout<<i<<'\t';
		tee.add(i);
	}
	Te::Re ree(&tee);
	std::cout<<"\n--------\n";
	// for (int i = 0; i < 10; ++i) {
	// 	std::cout<<*(ree)<<'\t';
	// 	++ree;
	// }
	for (; ree; ++ree) {
		std::cout<<*(ree)<<'\t';
	}
	std::cout<<"\n--------\n";
	return 0;
}