#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "BRAVOLock.h"
#include "pfq-rwlock.h"

BRAVO_rwlock_t rwlock;             //declare our rw lock
int data = 0;                      //the shared variable
const int READER_TIMES = 8000000;  //each reader thread will read data this times
const int WRITER_TIMES = 5000000;  //each writer thread will add 1 to data WRITER_TIMES times
const int READSIZE = 100;          //how many reader threads we have
const int WRITESIZE = 3;           //how many writer threads we have

void* reader(void* arg)
{
    int i = 0;
    for (i = 0; i < READER_TIMES; i++)
    {
        int pos = BRAVO_rwlock_read_lock(&rwlock);
        //pthread_rwlock_rdlock(&rwlock);    //read lock
        
        int a = data;
        a++;
        //printf("%lu read : %d , %d times \n",pthread_self(), data, i); //read the shared value
        
        //pthread_rwlock_unlock(&rwlock);    //read unlock
        BRAVO_rwlock_read_unlock(&rwlock, pos);
    }
    return NULL;
}


void* writer(void* arg)
{
    int i = 0;
    for (i = 0; i < WRITER_TIMES; i++) {
        pfq_rwlock_node_t node;
        BRAVO_rwlock_write_lock(&rwlock, &node);
        //pthread_rwlock_wrlock(&rwlock);      //write lock
        data++;                              //write to the shared value
        //printf("    %lu write : %d\n",pthread_self(), data);
        //pthread_rwlock_unlock(&rwlock);      //write unlock
        BRAVO_rwlock_write_unlock(&rwlock, &node);
    }
    return NULL;
}


int main(int argc,char** argv)
{
    pfq_rwlock_t pl;    //create the underlying lock
    BRAVO_rwlock_init(&rwlock, &pl);  //create our BRAVO lock
    pthread_t read[READSIZE];   //create our reader threads
    pthread_t write[WRITESIZE]; //create our writer threads
    
    printf("we have total %d reader threads and %d writer threads\n", READSIZE, WRITESIZE);
    printf("Each reader threads will read %d times\n", READER_TIMES);
    printf("Each writer threads will write %d times\n", WRITER_TIMES);
    
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    long start = ts.tv_sec;
    
    int i = 0;
    for (i = 0; i < READSIZE; i++) {
        pthread_create(&read[i],NULL,reader,NULL); //start reader threads
    }
    for (i = 0; i < WRITESIZE; i++) {
        pthread_create(&write[i],NULL,writer,NULL); //start writer threads
    }
    for (i = 0; i < READSIZE; i++) {
        pthread_join(read[i], NULL); //wait for reader threads
    }
    for (i = 0; i < WRITESIZE; i++) {
        pthread_join(write[i], NULL); //wait for writer threads
    }
    
    clock_gettime(CLOCK_MONOTONIC,&ts);
    long per = ts.tv_sec - start;
    
    //BRAVO_rwlock_destroy(&rwlock); //destroy our BRAVO lock
    printf("End of Program with %lus, data: %d\n", per, data);
    return 0;
}
