//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pc_def.c"

void doProdThings(int prod);

void doConsThings(int sd);

int
main(int argc, char *argv[])
{
    int prodCount,consCount,messagesPerProd,i,prodId;
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
    int mainSem = sem_open("Main",0,1);
    //create producer processes
//    for (i = 0; i < prodCount; ++i) {
//        prodId = fork();
//        //new
//        if (prodId == 0 ) {
//            doProdThings(messagesPerProd);
//            exit();
//        }
//    }

    for (i = 0; i < consCount; ++i) {
        prodId = fork();
        //new
        if (prodId == 0 ) {
            doConsThings(mainSem);
            exit();
        }
    }

    write_parent_msg();
    sem_signal(mainSem);

    exit();
}

void doConsThings(int sd) {
    sem_wait(sd);
    write_cons_msg();
}

void doProdThings(int prod) {
    printf(2,"proc #%d doing things",getpid());
}