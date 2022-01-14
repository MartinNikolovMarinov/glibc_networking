#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/if.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// tun device emulates a network inside user space.
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

void printBytes(char *bytes, ssize_t len) {
    for (ssize_t i = 0; i < len; i++) {
        printf("0x%x ", bytes[i] & 0xff);
    }
}

int tunFd = -1;

void on_sigint(int signo) {
    printf("Received %d\n", signo);
    if (tunFd > 0) {
        close(tunFd);
    }
    exit(signo);
}

struct iphdr {
    uint8_t version : 4; // 4 this specifies the size in bits (NOT bytes!)
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_offset : 13;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
} __attribute__((packed));

int main(int argc, char const *argv[]) {
    signal(SIGINT, on_sigint);

    tunFd = open("/dev/net/tun", O_RDWR);
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
    unsigned char buf[bufSize];

    while (1) {
        ssize_t rRes = read(tunFd, buf, bufSize);
        if (rRes < 0) {
            perror("Failed to open tun");
            return -1;
        }

        uint16_t flags = ntohs(*(uint16_t *)(buf + 0));
        uint16_t proto = ntohs(*(uint16_t *)(buf + 2));

        if (proto != (uint16_t)0x0800) {
            // This means it's not IPv4
            continue;
        }

        printf("read %ld bytes\n", rRes);
        printf("[ ");
        printBytes(buf, bufSize);
        printf("]\n");
    }

    close(tunFd);
    return 0;
}
