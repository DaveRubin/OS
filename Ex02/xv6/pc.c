//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pc_def.c"
#include "fcntl.h"

void doProdThings();

void doConsThings();

void logAllSems();
void cleanup();

char *SEM_MAIN_NAME = "main";
char *SEM_PRODUCER = "prod";
char *SEM_CONSUMER = "cons";
char *SEM_CONSOLE = "cnsl";
char *SEM_MESSAGES = "msgs";
int messagesPerProd;
int consoleSD;
int producerSD;
int consumerSD;
int mainSem;
int totalMessagesSD;

int
main(int argc, char *argv[]) {
    int prodCount, consCount, i, childProcId;
    int newFD = -1;
    //char *fileName;
    //first arg is always mandatory... so 3-4 => 4-5
    if (argc != 4 && argc != 5) {
        printf(1, "%d Not enought arguments!!\n", argc);
        exit();
    }
    int fd = open("XXXX",O_RDWR);
    if (fd>= 0 ) {
        printf(fd,"AAAAA");
    }

    prodCount = atoi(argv[1]);
    consCount = atoi(argv[2]);
    messagesPerProd = atoi(argv[3]);
    if(argc == 5){
        
        if((fd = open(argv[4], O_CREATE | O_RDWR)) < 0){
            printf(1,"filed to create the supplied file\n");
            return -1;
        }
        
        close(1);
        newFD = dup(fd);
        close(fd);
    }

    //create base semaphores
    int totalMessages = prodCount*messagesPerProd;
    mainSem = sem_open(SEM_MAIN_NAME, 0, 2);
    producerSD = sem_open(SEM_PRODUCER, prodCount, prodCount);
    consumerSD = sem_open(SEM_CONSUMER, 0, consCount);
    consoleSD = sem_open(SEM_CONSOLE, 1, 1);
    totalMessagesSD = sem_open(SEM_MESSAGES, totalMessages, totalMessages);
    //logAllSems();

    //PRODUCERS
    childProcId = fork();
    if (childProcId == 0) {
        //PRODUCER CREATOR PROCESS
        sem_wait(mainSem);
        sem_signal(mainSem);
        //create producer processes
        for (i = 0; i < prodCount ; ++i) {
            childProcId = fork();
            if (childProcId == 0) {
                doProdThings();
                exit();
            }
        }

        while (wait() > 0);

        exit();
    }

    //CONSUMER
    childProcId = fork();
    if (childProcId == 0) {
        //CONSUMER CREATOR PROCESS
        sem_wait(mainSem);
        sem_signal(mainSem);

        for (i = 0; i < consCount; ++i) {
            childProcId = fork();
            //new
            if (childProcId == 0) {
                doConsThings();
                exit();
            }
        }

        while (wait() > 0);

        exit();
    }


    write_parent_msg();
    //release restriction for prod & consumer
    sem_signal(mainSem);
    sem_signal(mainSem);

    while (wait() > 0);
    if (newFD != -1) {
        close(newFD);
    }
    //cleanup();
    exit();
}

void logAllSems() {
    int a, b;
    sem_gat_value(mainSem, &a, &b);
    printf(1, "mainSem val %d , max %d\n", a, b);
    sem_gat_value(producerSD, &a, &b);
    printf(1, "producerSD val %d , max %d\n", a, b);
    sem_gat_value(consumerSD, &a, &b);
    printf(1, "consumerSD val %d , max %d\n", a, b);
    sem_gat_value(consoleSD, &a, &b);
    printf(1, "consoleSD val %d , max %d\n", a, b);
    sem_gat_value(totalMessagesSD, &a, &b);
    printf(1, "totalMessagesSD val %d , max %d\n", a, b);
}

void doConsThings() {

    int loop = sem_try_wait(totalMessagesSD) >= 0;

    while (loop) {
        //while not done, return
        //consume messages while messages are waiting, when messages are done done
        sem_wait(consumerSD);
        //safe write
        sem_wait(consoleSD);
        write_cons_msg();
        sem_signal(consoleSD);
        loop = sem_try_wait(totalMessagesSD) >= 0; //when no more total messages leave do consThings
    }

}

void doProdThings() {
    int i;
    for (i = 0; i < messagesPerProd; ++i) {

        sem_wait(producerSD);
        //safe write
        sem_wait(consoleSD);
        write_prod_msg();
        sem_signal(consoleSD);

        sem_signal(consumerSD); //free one consumer lock
        sem_signal(producerSD); //free the taken producer lock
    }
}

void cleanup() {
    printf(1,"A\n");
    sem_close(mainSem);
    sem_unlink(mainSem);

    printf(1,"A\n");
    sem_close(producerSD);
    sem_unlink(producerSD);

    printf(1,"A\n");
    sem_close(consumerSD);
    sem_unlink(consumerSD);

    printf(1,"A\n");
    sem_close(consoleSD);
    sem_unlink(consoleSD);

    printf(1,"A\n");
    sem_close(totalMessagesSD);
    sem_unlink(totalMessagesSD);
}