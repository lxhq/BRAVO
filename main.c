#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "BRAVOLock.h"
#include "pfq-rwlock.h"

bool BRAVO = true;          //TRUE: BRAVO will be used. FALSE: pfp will be used
pfq_rwlock_t pfq_rwlock;           //declare our pfq rw lock
BRAVO_rwlock_t BRAVO_rwlock;       //declare our BRAVO rw lock
int data = 0;                      //the shared variable
const int READER_TIMES = 8000000;  //each reader thread will read data READER_TIMES times
const int WRITER_TIMES = 5000000;  //each writer thread will add 1 to data WRITER_TIMES times

void* reader(void* arg)
{
    int i = 0;
    for (i = 0; i < READER_TIMES; i++)
    {
        if (BRAVO) {
            int pos = BRAVO_rwlock_read_lock(&BRAVO_rwlock);
            int a = data;
            //printf("%lu read : %d , %d times \n",pthread_self(), data, i); //read the shared value
            BRAVO_rwlock_read_unlock(&BRAVO_rwlock, pos);
        } else {
            pfq_rwlock_read_lock(&pfq_rwlock);
            int a = data;
            //printf("%lu read : %d , %d times \n",pthread_self(), data, i); //read the shared value
            pfq_rwlock_read_unlock(&pfq_rwlock);
        }
    }
    return NULL;
}

void* writer(void* arg)
{
    int i = 0;
    for (i = 0; i < WRITER_TIMES; i++) {
        if (BRAVO) {
            pfq_rwlock_node_t node;
            BRAVO_rwlock_write_lock(&BRAVO_rwlock, &node);
            data++;                              //write to the shared value
            //printf("    %lu write : %d\n",pthread_self(), data);
            BRAVO_rwlock_write_unlock(&BRAVO_rwlock, &node);
        } else {
            pfq_rwlock_node_t node;
            pfq_rwlock_write_lock(&pfq_rwlock, &node);
            data++;                              //write to the shared value
            //printf("    %lu write : %d\n",pthread_self(), data);
            pfq_rwlock_write_unlock(&pfq_rwlock, &node);
        }
    }
    return NULL;
}

int main(int argc,char** argv)
{

    if (argc != 4) {
        printf("Must have Four input arguments, only have %d now\n", argc);
        exit(0);
    }
    if (strcmp(argv[1], "BRAVO") == 0) {
        BRAVO = true;
    } else {
        BRAVO = false;
    }
    
    int READSIZE = atoi(argv[2]);   //we have READSIZE reader threads
    int WRITESIZE = atoi(argv[3]);  //we have WRITESIZE writer thread

    if (BRAVO) {
        printf("This is the test for BRAVO rwlock\n");
    } else {
        printf("This is the test for pfq rwlock\n");
    }
    pfq_rwlock_t underlying;                        //create the underlying lock for BRAVO
    BRAVO_rwlock_init(&BRAVO_rwlock, &underlying);  //init the BRAVO lock
    pfq_rwlock_init(&pfq_rwlock);                   //init the pfq lock
    
    pthread_t read[READSIZE];                       //create our reader threads
    pthread_t write[WRITESIZE];                     //create our writer threads
    
    printf("we have total %d reader threads and %d writer threads\n", READSIZE, WRITESIZE);
    printf("Each reader threads will read %d times\n", READER_TIMES);
    printf("Each writer threads will write %d times\n", WRITER_TIMES);
    
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    long start = ts.tv_sec;
    
    int i = 0;
    for (i = 0; i < READSIZE; i++) {
        pthread_create(&read[i],NULL,reader,NULL);  //start reader threads
    }
    for (i = 0; i < WRITESIZE; i++) {
        pthread_create(&write[i],NULL,writer,NULL); //start writer threads
    }
    for (i = 0; i < READSIZE; i++) {
        pthread_join(read[i], NULL);                //wait for reader threads
    }
    for (i = 0; i < WRITESIZE; i++) {
        pthread_join(write[i], NULL);               //wait for writer threads
    }
    
    clock_gettime(CLOCK_MONOTONIC,&ts);
    long time_cost = ts.tv_sec - start;
    
    printf("End of Program with %lus, data: %d\n", time_cost, data);
    return 0;
}
