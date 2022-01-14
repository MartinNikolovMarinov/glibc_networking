#include "libs/core/src/core.h"
#include "src/tun.h"

#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

global_variable i32 nic;

void SignalHandler(i32 _signal)
{
    if (nic > 0) {
        close(nic);
    }
    core::OsExit(_signal);
};

void PrintBytes(constptr uchar *_bytes, ptr_size _len)
{
    for (ptr_size i = 0; i < _len; i++) {
        core::PrintF("0x%x ", _bytes[i]);
    }
}

// struct iphdr {
//     uint8_t version : 4; // 4 this specifies the size in bits (NOT bytes!)
//     uint8_t ihl : 4;
//     uint8_t tos;
//     uint16_t len;
//     uint16_t id;
//     uint16_t flags : 3;
//     uint16_t frag_offset : 13;
//     uint8_t ttl;
//     uint8_t proto;
//     uint16_t csum;
//     uint32_t saddr;
//     uint32_t daddr;
// } __attribute__((packed));

i32 main(i32 argc, constptr char *argv[], constptr char *envp[])
{
    signal(SIGINT, SignalHandler);

    nic = TryOrFail(tun::TUNOpen("tun0"));
    const int bufSize = 1504;
    uchar buf[bufSize] = {};

    while (true) {
        signed_ptr_size rRes = read(nic, buf, bufSize);
        if (rRes < 0) {
            core::PrintF("Failed to read tun");
            return -1;
        }

        u16 flags = ntohs(*(u16 *)(buf + 0));
        u16 protocol = ntohs(*(u16 *)(buf + 2));

        if (protocol != (uint16_t)0x0800) {
            // This means it's not IPv4
            continue;
        }

        core::PrintF("read %ld bytes\n", rRes);
        core::PrintF("[ ");
        PrintBytes(buf, (ptr_size)rRes);
        core::PrintF("]\n");
    }

    close(nic);
    return 0;
}
