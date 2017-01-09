#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    int val = testKern(19);
    printf(2,"Hello from test %d\n", val);
    sem_open("KOKO", 3, 3);
    printf(2,"\n");
    sem_open("KOKO", 3, 3);
    printf(2,"\n");
    sem_open("KOKO", 3, 3);
    printf(2,"\n");
    sem_open("KOKO", 3, 3);
    printf(2,"\n");
//    sem_close(0);
//    sem_wait(0);
//    sem_try_wait(0);
//    sem_signal(0);
//    sem_gat_value(0, &val, &val);
//    sem_unlink(0);
    exit();
}
