#include "pfq-rwlock.h"

typedef struct  {
	pfq_rwlock_t* Underlying;
	long InhibitUntil;
	int RBias;
} BRAVO_rwlock_t;

void BRAVO_rwlock_init(BRAVO_rwlock_t *l, pfq_rwlock_t* pl);

int BRAVO_rwlock_read_lock(BRAVO_rwlock_t *l);

void BRAVO_rwlock_read_unlock(BRAVO_rwlock_t *l, int pos);

void BRAVO_rwlock_write_lock(BRAVO_rwlock_t *l, pfq_rwlock_node_t* node);

void BRAVO_rwlock_write_unlock(BRAVO_rwlock_t *l, pfq_rwlock_node_t* node);

void BRAVO_rwlock_destroy(BRAVO_rwlock_t *l);
