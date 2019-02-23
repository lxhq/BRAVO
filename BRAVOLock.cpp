#include <iostream>
#include <thread>
#include <atomic>
#include "BRAVOLock.h"

BRAVOLock<Lock>* VisibleReaders[1024];
const int N = 9;

void Reader(BRAVOLock<Lock>* L) {
	BRAVOLock<Lock>* slot = 0;
	if (L->RBias == 1) {
		
		//slot = VisinbleReaders + Hash(L);
		//if (std::atomic_compare_exchange_strong(slot, 0, L) == NULL) {
			if (L->RBias == 1) {
				//goto EnterCS
			}
			//*slot = 0;
		//}
		slot = 0;
	}
	//slow path
	L->Underlying.requireRead();
	if (L->RBias == 0 && time(0) >= L->InhibitUntil) {
		L->RBias = 1;
	}
	//Enter CS
	//enter critical section

	//end of critical section
	if (slot != NULL) {
		//*slot = NULL;
	}
	else {
		L->Underlying.releaseRead();
	}
}

void Writer(BRAVOLock<Lock>* L) {
	L->Underlying.requireWrite();
	if (L->RBias) {
		L->RBias = 0;
		auto start = time(0);
		for (int i = 0; i < 1024; i++) {
			while (VisibleReaders[i] == L) {
				//pause();
				
			}
		}
		auto now = time(0);
		L->InhibitUntil = now + ((now - start) * N);
	}
	//enter critical section

	//end of critical section
	L->Underlying.releaseWrite();
}

char* Hash(BRAVOLock<Lock>* L, int self) {
	int hash = (L + self) % 1024;
	return 0;
}