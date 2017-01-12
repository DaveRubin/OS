
enum STATUS {SEM_DEAD,SEM_ALIVE} ;
struct semaphore {
    struct spinlock sslock; // single semaphore lock
    char name[6];
    int ref;
    int val;
    int maxVal;
    enum STATUS status;
};

void seminit(void);
int sem_alloc(struct semaphore *s);
