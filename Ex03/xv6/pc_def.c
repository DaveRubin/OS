//
// Created by David Rubin on 12/01/2017.
//
#define BUF_SIZE 20
#define MAX_MSG 3
#define DELAY 10000
#include "user.h"

void burn_time(int num);

void write_prod_msg()
{
    int i, pid = getpid(), err;
    char buf[BUF_SIZE];

    for (i=0; i< BUF_SIZE-1; i++)
        buf[i]= i < 5 ? '_' : 'A' + (pid % 26);
    buf[i]='\n';

    printf(1, "PRODUCER PID %d ", pid);
    for(i=0; i<BUF_SIZE; i++)
    {
        err = write(1, &buf[i], 1);
        burn_time(pid);
        if ( err < 0)
            printf(2, "**ERROR** write_make_msg failed!\n");
    }

}

void write_cons_msg()
{
    int i, pid = getpid(), err;
    char buf[BUF_SIZE];

    for (i=0; i< BUF_SIZE-1; i++)
        buf[i]= i < 5 ? '_' : '0' + (pid % 10);
    buf[i]='\n';

    printf(1, "CONSUMER PID %d ", pid);
    for(i=0; i<BUF_SIZE; i++)
    {
        err = write(1, &buf[i], 1);
        burn_time(pid);
        if ( err < 0)
            printf(2, "**ERROR** write_make_msg failed!\n");
    }

}

void write_parent_msg()
{
    int i, pid = getpid(), err;
    char buf[BUF_SIZE];

    for (i=0; i< BUF_SIZE-1; i++)
        buf[i]= '*';
    buf[i]='\n';

    printf(1, "PARENT PID %d Created all children ", pid);
    for(i=0; i<BUF_SIZE; i++)
    {
        err = write(1, &buf[i], 1);
        burn_time((pid+2)*2);
        if ( err < 0)
            printf(2, "**ERROR** write_make_msg failed!\n");
    }

}

void burn_time(int num)
{
    int i;
    for (i=0; i < DELAY * num; i++);
}