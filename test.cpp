#include <iostream>
#include <thread>
#include <math.h>
#include <atomic>
using namespace std;

class Lock {
public:
	int id;
};

int calhash(Lock &L) {
	std::thread::id id = std::this_thread::get_id();
	int a = std::hash<std::thread::id>()(id);
	int* b = (int*)&L;
	int c = abs(a + *b) % 1024;
	cout << c << endl;
	return 0;
}

Lock* table[1024];
int main() {
	cout << "Hello World" << endl;
	Lock* lock = new Lock();
	lock->id = 3;
	int pos = calhash(*lock);
	table[pos] = lock;
	Lock* n = NULL;
	//if (visualtable[0] == *n) { //we need to overload the == operator here
	//	cout << "Yes" << endl;
	//}
	bool d = std::atomic<Lock*> (table[pos]).compare_exchange_strong(lock, n);
	cout << d << endl;
	cout << table[pos]->id << " " << lock->id <<endl;
	getchar();
	return 0;
}
