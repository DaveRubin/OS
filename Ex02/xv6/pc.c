//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pc_def.c"

void doProdThings();
void doConsThings();

char *SEM_MAIN_NAME = "main";
char *SEM_PRODUCER = "prod";
char *SEM_CONSOLE = "cons";
int messagesPerProd;
int consoleSD;
int prodSD;

int
main(int argc, char *argv[])
{
    int prodCount,consCount,i,prodId;
    //char *fileName;
    //first arg is always mandatory... so 3-4 => 4-5
    if (argc != 4 &&  argc != 5) {
        printf(2,"%d Not enought arguments!!\n",argc);
        exit();
    }
    prodCount = atoi(argv[1]);
    consCount = atoi(argv[2]);
    messagesPerProd = atoi(argv[3]);
    if (argc == 5) {
        //fileName = argv[4];
    }

    printf(2,"Starting pc with %d producers, %d consumers , and %d  messages per producer\n",
           prodCount,consCount,messagesPerProd);
    //create base semaphores
    int mainSem = sem_open(SEM_MAIN_NAME,0,2);
    prodSD = sem_open(SEM_PRODUCER,0,prodCount);
    consoleSD = sem_open(SEM_CONSOLE ,0,1);
    printf(2,"%d %d %d\n\n",mainSem,prodSD,consoleSD);

    sem_close(prodSD);
    //create producer processes
    for (i = 0; i < prodCount; ++i) {
        prodId = fork();
        //new
        if (prodId == 0 ) {
//            sem_wait(mainSem);
//            doProdThings();
//            sem_signal(mainSem);
            exit();
        }
    }

    for (i = 0; i < consCount; ++i) {
        prodId = fork();
        //new
        if (prodId == 0 ) {
//            sem_wait(mainSem);
//            doConsThings();
//            sem_signal(mainSem);
            exit();
        }
    }

    write_parent_msg();
    //release restriction for prod & consumer
    sem_signal(mainSem);
    int a,b;
    sem_gat_value(mainSem,&a,&b);
    printf(2,"%d=== %d %d\n ",mainSem,a,b);
    sem_signal(mainSem);
    sem_gat_value(mainSem,&a,&b);
    printf(2,"%d=== %d %d \n",mainSem,a,b);
    sem_signal(consoleSD);

    while (wait() > 0);
    printf(2,"\nDone\n");
    exit();
}

void doConsThings() {
    //sem_wait(prodSD);
    sem_wait(consoleSD);
    write_cons_msg();
    sem_signal(consoleSD);
}

void doProdThings() {
    int i;
    for (i = 0; i < messagesPerProd; ++i) {
        sem_signal(prodSD);

        sem_wait(consoleSD);
        write_prod_msg();
        sem_signal(consoleSD);
    }

    sem_close(prodSD);
}