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

int
main(int argc, char *argv[]) {
    printf(1,"YO YO %d\n\n\n", sizeof(struct xxx));
    int fd = open("Z",O_RDWR | O_CREATE);
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }

    printf(fd,"YO YO YO !! ! ! ! \n");
    close(fd);

    fd = open("Z",O_RDWR | O_CREATE |O_APPEND);
    if (fd<0) {
        printf(1,"failed opening file...\n");
        exit();
    }
    printf(fd,"\nSecond time... hmmm \n");

//    close(fd);
//
//    fd = open("Z",O_RDWR | O_CREATE);
//    if (fd<0) {
//        printf(1,"failed opening file...\n");
//        exit();
//    }
//
//    printf(fd,"Another one bites the dust...\n");

    close(fd);
    exit();
}
