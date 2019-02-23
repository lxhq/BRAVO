template<typename T> 
class BRAVOLock {
public:
	T Underlying;
	time_t InhibitUntil;
	int RBias;
};

class Lock {
public:
	void requireRead() {};
	void releaseRead() {};
	void requireWrite() {};
	void releaseWrite() {};
};

char* Hash(BRAVOLock<Lock>* L, int);

template<typename T>
void Reader(BRAVOLock<T>*);

template<typename T>
void Writer(BRAVOLock<T>*);


