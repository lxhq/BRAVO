#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "BRAVOLock.h"

const int SIZE = 1024;
BRAVO_rwlock_t *VisibleReaders[1024];
const int N = 9;

int calhash(BRAVO_rwlock_t *l) {
    unsigned int a = (unsigned int)pthread_self();
    unsigned int c = a + (unsigned int)l;
    c = (c >> 20) ^ (c >> 12) ^ (c >> 7) ^ (c >> 4);
    return c % SIZE;
}

void BRAVO_rwlock_init(BRAVO_rwlock_t *l, pfq_rwlock_t* pl) {
    l->Underlying = pl;
    l->InhibitUntil = 0;
    pfq_rwlock_init(l->Underlying);
}

int BRAVO_rwlock_read_lock(BRAVO_rwlock_t * l) {
    if (l->RBias == 1) {
        int pos = calhash(l);
        if (__sync_bool_compare_and_swap(VisibleReaders + pos, NULL, l)) {
            if (l->RBias == 1) {
                //fast path
                return pos;
             } else {
                VisibleReaders[pos] = NULL;
             }
        }
    }
    //slow path
    pfq_rwlock_read_lock(l->Underlying);
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    if (l->RBias == 0 && ts.tv_nsec >= l->InhibitUntil) {
        l->RBias = 1;
    }
    return -1;
}

void BRAVO_rwlock_read_unlock(BRAVO_rwlock_t *l, int pos) {
    if (pos != -1) {
        assert(VisibleReaders[pos] == l);
        VisibleReaders[pos] = NULL;
    } else {
        pfq_rwlock_read_unlock(l->Underlying);
    }
}

void BRAVO_rwlock_write_lock(BRAVO_rwlock_t * l, pfq_rwlock_node_t* node) {
    pfq_rwlock_write_lock(l->Underlying, node);
    if (l->RBias == 1) {
        l->RBias = 0;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC,&ts);
        long start = ts.tv_nsec;
        int i = 0;
        for (i = 0; i < SIZE; i++) {
                while (VisibleReaders[i] == l) {
                usleep(1);
                }
        }
        clock_gettime(CLOCK_MONOTONIC,&ts);
        long now = ts.tv_nsec;
        l->InhibitUntil = now + ((now - start) * N);
    }
}

void BRAVO_rwlock_write_unlock(BRAVO_rwlock_t *l, pfq_rwlock_node_t* node) {
    pfq_rwlock_write_unlock(l->Underlying, node);
}

void BRAVO_rwlock_destroy(BRAVO_rwlock_t *l) {
   // pthread_rwlock_destroy(l->Underlying);
}
