
#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "mmu.h"
#include "proc.h"
#include "semaphores.h"

#define SEM_VERBOSE 0

struct {
    struct spinlock gslock; //global semaphore lock
    struct semaphore sem[NSEM];
} stable;
void seminit(void) {
    initlock(&stable.gslock, "stable");
}

int sem_open(char *name, int init, int maxVal) {

    if (SEM_VERBOSE) cprintf("Sem open %s %d %d\n", name, init, maxVal);
    
    if (name == 0 ||
        strlen(name) > 6 ||
        init < 0 ||
        maxVal < 0 ||
        maxVal < init)
        return -1;

    acquire(&stable.gslock);

    int i = 0;
    int freeIndex = -1;
    struct semaphore *s;
    //look for semaphore with the same name
    for (s = stable.sem; s < stable.sem + NSEM; s++) {

        if (SEM_VERBOSE) cprintf("checking index %d with reference\n", i, s->ref);
        if (s->status == SEM_ALIVE) {
            if (strncmp(s->name, name, 6) == 0) {
                if (SEM_VERBOSE) cprintf("same name \n");
                //found
                s->ref++;
                //create sem in proc
                i = sem_alloc(s);

                if (i < 0) {
                    //free semaphore
                    s->ref--;
                }
                if (SEM_VERBOSE) cprintf("i = %d, sf ref %d, maxref %d\n", i, s->ref, s->maxVal);

                release(&stable.gslock);
                return i;
            }
        } else if (freeIndex == -1) {
            //save the first free index in the table
            //cprintf("%d",i);
            freeIndex = i;
        }
        i++;
    }

    //if not found then create one in the first free index
    if (freeIndex != -1) {
        s = &(stable.sem[freeIndex]);
        if (SEM_VERBOSE) cprintf("Creating new in %d\n", i);
        //cprintf("Creating new in %d\n", freeIndex);
        //found free spot
        safestrcpy(s->name, name, 6);
        s->ref = 1;
        s->val = init;
        s->maxVal = maxVal;
        s->status = SEM_ALIVE;
        initlock(&s->sslock, name);
        //cprintf("Created %s \n",s->name);
        i = sem_alloc(s);

        if (i < 0) {
            //free semaphore
            s->ref--;
        }

        release(&stable.gslock);
        return i;
    }

    release(&stable.gslock);
    return -1;
}

int sem_close(int sd) {
    if (sd < 0 || sd >= NSEM)
        return  -1;

    if (SEM_VERBOSE) cprintf("sem_close\n");
    acquire(&stable.gslock);

    //get semaphore from process
    struct semaphore *s = proc->osem[sd];
    if (s->status == SEM_DEAD) {
        release(&stable.gslock);
        return -1;
    }

    //clear the reference from the osem
    //cprintf("Closing %s with %d refs\n",s->name,s->ref);
    s->ref--;
    proc->osem[sd] = 0;
    if (s->ref == 0) {
        wakeup(s);
    }
    release(&stable.gslock);
    return 0;
}

//--
int sem_wait(int sd) {
    if (sd < 0 || sd >= NSEM)
        return  -1;


    struct semaphore *s = proc->osem[sd];
    acquire(&s->sslock);

    while (s->val <= 0) {
        sleep(s, &s->sslock);
    }

    s->val--;

    release(&s->sslock);
    return 0;
}

int sem_try_wait(int sd) {
    if (SEM_VERBOSE) cprintf("sem_try_wait\n");
    if (sd < 0 || sd >= NSEM)
        return  -1;
    struct semaphore *s = proc->osem[sd];
    acquire(&s->sslock);

    if (s->status == SEM_DEAD) {
        release(&s->sslock);
        return -1;
    }

    if (s->val > 0) {
        s->val--;
        release(&s->sslock);
        return 0;
    }

    release(&s->sslock);
    return -1;
}

//++
int sem_signal(int sd) {
    if (sd < 0 || sd >= NSEM)
        return  -1;
    struct semaphore *s = proc->osem[sd];
    acquire(&s->sslock);
    //if sd isn't an open semaphore return -1
    if (s->status == SEM_DEAD) {
        release(&s->sslock);
        return -1;
    }

    //if val at maxVal then return -2
    if (s->val == s->maxVal) {
        release(&s->sslock);
        return -2;
    }

    s->val++;
    if (s->val > 0) {
        wakeup(s);
    }

    if (SEM_VERBOSE) cprintf("sem_signal\n");
    release(&s->sslock);
    return 0;
}

/**
 * gets sd and returns the value and maxVlue of a sempahore via given pointers
 * return 0 if everything went smooth, and -1 for error
 */
int sem_gat_value(int sd, int *val, int *maxVal) {
    if (sd < 0 || sd >= NSEM)
        return  -1;
    if (SEM_VERBOSE) cprintf("sem_gat_value\n");
    acquire(&stable.gslock);

    struct semaphore *s = proc->osem[sd];
    if (s->status == SEM_DEAD) {
        release(&stable.gslock);
        return -1;
    }

    *val = s->val;
    *maxVal = s->maxVal;
    release(&stable.gslock);
    //cprintf("...%d %d \n",s->val,s->maxVal);
    return 0;
}
/**
 * closes a semaphore for good!
 * also erases all osem references
 */
int sem_unlink(int sd) {
    if (sd < 0 || sd >= NSEM)
        return  -1;

    if (SEM_VERBOSE || 1) cprintf("sem_unlink\n");
    acquire(&stable.gslock);

    struct semaphore *s = proc->osem[sd];
    if (s->status == SEM_DEAD) {
        release(&stable.gslock);
        return -1;
    }

    //run through all processes and remove sd from them
    while (s->ref > 0 ) {
        cprintf("Ref count for %s is %d... sleeping\n",s->name,s->ref);
        sleep(s,&stable.gslock);
    }

    //now that no process have reference to this lock... clear
    s->status = SEM_DEAD;

    release(&stable.gslock);
    return 0;
}


int
sem_alloc(struct semaphore *s) {
    if (SEM_VERBOSE) cprintf("Sem alloc! - ");
    int sd;

    for (sd = 0; sd < NOSEM; sd++) {
        if (proc->osem[sd] == 0) {
            proc->osem[sd] = s;
            if (SEM_VERBOSE) cprintf("Found free osem in %d \n", sd);
            return sd;
        }
    }
    return -1;
}