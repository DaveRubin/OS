//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pc_def.c"

void doProdThings();
void doConsThings();

void logAllSems();

char *SEM_MAIN_NAME = "main";
char *SEM_PRODUCER = "prod";
char *SEM_CONSUMER= "cons";
char *SEM_CONSOLE = "cnsl";
int messagesPerProd;
int consoleSD;
int prodSD;
int consSD;
int mainSem;

int
main(int argc, char *argv[])
{
    int prodCount,consCount,i,childProcId;
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
    mainSem = sem_open(SEM_MAIN_NAME,0,2);
    prodSD = sem_open(SEM_PRODUCER,0,prodCount);
    consSD = sem_open(SEM_CONSUMER,prodCount,prodCount);
    consoleSD = sem_open(SEM_CONSOLE ,0,1);
    logAllSems();

    //create producer processes
    for (i = 0; i < prodCount; ++i) {
        childProcId = fork();
        //new
        if (childProcId == 0 ) {
            sem_wait(mainSem);
            doProdThings();
            sem_signal(mainSem);
            exit();
        }
    }

    for (i = 0; i < consCount; ++i) {
        childProcId = fork();
        //new
        if (childProcId == 0 ) {
            sem_wait(mainSem);
            doConsThings();
            sem_signal(mainSem);
            exit();
        }
    }

    write_parent_msg();
    //release restriction for prod & consumer
    sem_signal(mainSem);
    sem_signal(mainSem);

    while (wait() > 0);
    exit();
}

void logAllSems() {
    int a,b;
    sem_gat_value(mainSem,&a,&b);
    printf(2,"mainSem val %d , max %d\n",a,b);
    sem_gat_value(prodSD,&a,&b);
    printf(2,"prodSD val %d , max %d\n",a,b);
    sem_gat_value(consSD,&a,&b);
    printf(2,"consSD val %d , max %d\n",a,b);
    sem_gat_value(consoleSD,&a,&b);
    printf(2,"consoleSD val %d , max %d\n",a,b);
}

void doConsThings() {
    //wait till production signals...
    sem_wait(prodSD);
    //safe write
    sem_wait(consoleSD);
    write_cons_msg();
    sem_signal(consoleSD);
    //then tell whomever wants, that a consumer have opened
    //sem_signal(consSD);
}

void doProdThings() {
    int i;
    for (i = 0; i < messagesPerProd; ++i) {

        //first reduce the consumer counter, if no consumers waiting, wait...
        //sem_wait(consSD);
        //then signal that producer message has printed
        sem_signal(prodSD);
        //safe write
        sem_wait(consoleSD);
        write_prod_msg();
        sem_signal(consoleSD);
    }

    sem_close(prodSD);
}