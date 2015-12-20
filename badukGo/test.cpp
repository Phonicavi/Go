#include <cstdio>
#include <unordered_set>
#include <ctime>
using namespace std;

class Test{
	unordered_set<int> uset;
	

public:
	Test(){uset.clear();}
	~Test(){};
	void add(int a){uset.insert(a);}
	void remove(int a){uset.erase(a);}
	bool find(int a){return uset.find(a)!=uset.end();}
	//unordered_set<int>::iterator getIter(){return it;}
	

	class TestItr{
	protected:
		Test* p_t;
		unordered_set<int>::iterator it;
	public:
		TestItr(){};
		TestItr(Test *t){it = (t->uset).begin();p_t = t;}
		~TestItr(){p_t = NULL;delete p_t;}

		operator bool()const{return it != (p_t->uset).end();}
		int operator*() const{return *it;}
		void operator++(){++it;}
	};

};
int main(int argc, char const *argv[])
{
	// Test test1;
	// for (int i=0; i<100; ++i)
	// 	test1.add(i);

	// test1.remove(100);
	// test1.remove(50);
	// test1.add(1000);
	// for (Test::TestItr it(&test1);it;++it){
	// 	printf("%d\n", *it);
	// }
	// Test test2;
	// test2.add(214);
	// for (Test::TestItr it(&test2);it;++it){
	// 	printf("%d\n", *it);
	// }

	// for (Test::TestItr it(&test1);it;++it){
	// 	printf("%d\n", *it);
	// }

	// printf("%d\n", test1.find(7));
	printf("%f\n", 0.9*CLOCKS_PER_SEC);

	
}