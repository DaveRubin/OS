#include "spinlock.h"

struct semaphore {
    struct spinlock sslock; // single semaphore lock
    char name[6];
    int ref;
    int maxVal;
};

void seminit(void);
int sem_alloc(struct semaphore *s);
