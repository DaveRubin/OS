

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"
#include "proc.h"
#include "proc.c"
#include "semaphores.h"

struct {
    struct spinlock gslock; //global semaphore lock
    struct semaphore sem[NSEM];
} stable;

void seminit(void) {
    initlock(&stable.gslock, "stable");
}

int sem_open(char *name, int init, int maxVal) {

    cprintf("Sem open %s %d %d\n", name, init, maxVal);
    if (strlen(name) > 6)
        return -1;

    acquire(&stable.gslock);

    int i = 0;
    int freeIndex = -1;
    struct semaphore *s;
    //look for semaphore with the same name
    for (s = stable.sem; s < stable.sem + NSEM; s++) {

        cprintf("checking index %d with reference\n", i, s->ref);
        if (s->ref != 0 && strncmp(s->name, name, 6) == 0) {
            cprintf("same name \n");
            //found
            s->ref++;
            //create sem in proc
            i = sem_alloc(s);

            if (i < 0) {
                //free semaphore
                s->ref--;
            }
            cprintf("i = %d, sf ref %d, maxref %d\n", i, s->ref, s->maxVal);
            if (s->ref > s->maxVal) {
                release(&stable.gslock);
                while(s->ref > s->maxVal ) {
                    cprintf("Sleep \n");
                    acquire(&s->sslock);
                    sleep(s, &s->sslock);
                }
                return i;
            }
            else {
                release(&stable.gslock);
                return i;
            }
        } else if (freeIndex == -1) {
            //save the first free index in the table
            freeIndex = i;
        }
        i++;
    }

    i = 0;
    //if not found then create one in the first free index
    if (freeIndex != -1) {
        s = &(stable.sem[freeIndex]);
        cprintf("Creating new in %d\n", i);
        //found free spot
        safestrcpy(name, s->name, sizeof(name));
        s->ref = 1;
        s->maxVal = maxVal;
        initlock(&s->sslock, name);
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
    cprintf("sem_close\n");
    if (sd<0 || sd > NSEM)
        return -1;

    //get semaphore from process
    struct semaphore *s = proc->osem[sd];
    if (s == 0)
        return -1;

    //clear the reference from the osem
    proc->osem[sd] = 0;

    //reduce ref count, and close if ref reached 0
    s->ref--;
    if (s->ref <= s->maxVal ){
        cprintf("Wakeup!!!");
        wakeup(s);
    }
    return 0;
}

int sem_wait(int sd) {
    cprintf("sem_wait\n");
    return 0;
}

int sem_try_wait(int sd) {
    cprintf("sem_try_wait\n");
    return 0;
}

int sem_signal(int sd) {
    cprintf("sem_signal\n");
    return 0;
}

int sem_gat_value(int sd, int *val, int *maxVal) {
    cprintf("sem_gat_value\n");
    return 0;
}

int sem_unlink(int fd) {
    cprintf("sem_unlink\n");
    return 0;
}


int
sem_alloc(struct semaphore *s) {
    cprintf("Sem alloc! - ");
    int sd;

    for (sd = 0; sd < NOFILE; sd++) {
        if (proc->osem[sd] == 0) {
            proc->osem[sd] = s;
            cprintf("Found free osem in %d \n", sd);
            return sd;
        }
    }
    return -1;
}