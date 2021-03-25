#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// devname should be "tun0" or "tun1", "tun%d"
int tunOpen(int fd, const char *devname) {
    assert(devname != NULL);
    assert(fd > 0);

    struct ifreq ifr = {};
    ifr.ifr_flags = IFF_TUN;
    strncpy(ifr.ifr_name, (char*)devname, IFNAMSIZ);

    int ioctlRes;
    if ((ioctlRes = ioctl(fd, TUNSETIFF, (void *) &ifr)) == -1 ) {
        return ioctlRes;
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    // tun device emulates a network inside user space.
    // you need tun

    int tunFd = open("/dev/net/tun", O_RDWR);
    if (tunFd < 0) {
        perror("/dev/net/tun failed to open");
        return -1;
    }

    int tunOpenRes = tunOpen(tunFd, "tun0");
    if (tunOpenRes < 0) {
        perror("Failed to open tun");
        return -1;
    }

    const int bufSize = 1504;
    char buf[bufSize];
    ssize_t rRes = read(tunFd, buf, bufSize);
    if (rRes < 0) {
        perror("Failed to open tun");
        return -1;
    }

    // char buf[1504];
    close(tunFd);
    return 0;
}
