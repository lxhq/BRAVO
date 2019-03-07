main : main.o BRAVOLock.o mcs-lock.o pfq-rwlock.o
	gcc -lrt -lpthread -o main main.o BRAVOLock.o mcs-lock.o pfq-rwlock.o
main.o : main.c
	gcc -lrt -c main.c
BRAVOLock.o : BRAVOLock.h BRAVOLock.c
	gcc -lrt -c BRAVOLock.c
mcs-lock.o : mcs-lock.h mcs-lock.c
	gcc -c mcs-lock.c
pfq-rwlock.o : pfq-rwlock.h pfq-rwlock.c
	gcc -c pfq-rwlock.c
clean:
	rm main main.o BRAVOLock.o mcs-lock.o pfq-rwlock.o
