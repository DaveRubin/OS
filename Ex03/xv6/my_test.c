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

    int fd = open(deleteRangeFileName,O_WRONLY | O_CREATE | O_BLOCK_WRITE);
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    char *stringNdirect = "1234567890abcd"; // the c&d should be in indirect reference
    printf(fd,stringNdirect);

    int testArgumentA = delete_range(-1,0,0); //(arguments)
    int testArgumentB = delete_range(fd,90000,0); //(from > size)
    int testArgumentC = delete_range(fd,0,90000); //(till > size)
    int testArgumentD = delete_range(fd,-1,0); //(from < 0)
    int testArgumentE = delete_range(fd,0,-1); //(till < 0)
    int testArgumentF = delete_range(fd,100,99); //(till < 0)

    //check invalid input
    printf(1,"Testing invalid input (arguments), should be -1 : got %d\n",testArgumentA);
    printf(1,"Testing invalid input (from > size), should be -2 : got %d\n",testArgumentB);
    printf(1,"Testing invalid input (till > size), should be -2 : got %d\n",testArgumentC);
    printf(1,"Testing invalid input (from < 0), should be -2 : got %d\n",testArgumentD);
    printf(1,"Testing invalid input (till < 0), should be -2 : got %d\n",testArgumentE);
    printf(1,"Testing invalid input (from>till), should be -3 : got %d\n",testArgumentF);
    delete_range(fd,2500,3400);

    close(fd);
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
