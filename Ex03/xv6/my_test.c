//
// Created by David Rubin on 12/01/2017.
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define XNDIRECT 11

// On-disk inode structure
struct xxx {
    short type;           // File type
    short major;          // Major device number (T_DEV only)
    short minor;          // Minor device number (T_DEV only)
    short nlink;          // Number of links to inode in file system
    uint size;            // Size of file (bytes)
    uint off;
    uint addrs[XNDIRECT+1];   // Data block addresses
};

void testAppend();

void testBlockWrite();

int
main(int argc, char *argv[]) {
    printf(1,"YO YO %d\n\n\n", sizeof(struct xxx));
    testAppend();
    //testBlockWrite();
    exit();
}

void testBlockWrite() {

}

void testAppend() {
    int fd = open("Z",O_RDWR | O_CREATE );
    if (fd<0) {
        write(1,"failed opening file...\n", sizeof("failed opening file...\n"));
        exit();
    }
    write(fd,"Hi there\n", 9);
    close(fd);

    fd = open("Z",O_WRONLY | O_CREATE | O_APPEND );
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    write(fd,"\nSecond time... hmmm \n", sizeof("\nSecond time... hmmm \n"));

    close(fd);
//
//    fd = open("Z",O_RDWR | O_CREATE);
//    if (fd<0) {
//        printf(1,"failed opening file...\n");
//        exit();
//    }
//
//    //write(fd,"Another one bites the dust...\n", sizeof("Another one bites the dust...\n"));
//    write(fd,"Another one bites the dust...\n", 513);
//
//    close(fd);
}
