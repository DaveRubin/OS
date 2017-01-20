//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *appendTestFileName = "a";
char *blockTestFileName = "b";
char *deleteRangeFileName = "c";
void testAppend();

void testBlockWrite();

void testDeleteRange();

int
main(int argc, char *argv[]) {
    testAppend();
    testBlockWrite();
    testDeleteRange();
    exit();
}

void testDeleteRange() {
    delete_range(2,4,6);
}

void testBlockWrite() {
    char *string = "Testing block write...";
    int fd = open(blockTestFileName,O_WRONLY | O_CREATE | O_BLOCK_WRITE);
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    write(fd,string, strlen(string));

    close(fd);
}

void testAppend() {
    char *s1 = "Append Test - Hi there\n";
    char *s2 = "Append Test - Second time... hmmm \n";
    char *s3 = "Append Test - Another one bites the dust...\n";
    char *s4 = "Append Test - fourth time a charm... \n";
    int fd = open("Z",O_RDWR | O_CREATE );
    if (fd<0) {
        write(1,"failed opening file...\n", sizeof("failed opening file...\n"));
        exit();
    }
    write(fd,s1, strlen(s1));
    close(fd);

    fd = open(appendTestFileName,O_WRONLY | O_CREATE | O_APPEND );
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    write(fd,s2, strlen(s2));

    close(fd);

    fd = open(appendTestFileName,O_RDWR | O_CREATE);
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    write(fd,s3, strlen(s3));

    close(fd);
    fd = open(appendTestFileName,O_WRONLY | O_CREATE | O_APPEND );
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    write(fd,s4, strlen(s4));

    close(fd);
}
