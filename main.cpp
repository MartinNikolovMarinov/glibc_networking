#include "libs/core/src/core.h"
#include "src/tun.h"

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

void SignalHandler(i32 signal)
{
    if (nic > 0) {
        core::PrintF("Closing tun interface.");
        close(nic);
    }
    core::OsExit(signal);
};

void PrintBytes(constptr uchar *bytes, signed_ptr_size len)
{
    for (signed_ptr_size i = 0; i < len; i++) {
        core::PrintF("0x%x ", bytes[i] & 0xff);
    }
}

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

        // uint16_t flags = ntohs(*(uint16_t *)(buf + 0));
        // uint16_t proto = ntohs(*(uint16_t *)(buf + 2));

        // if (proto != (uint16_t)0x0800) {
        //     // This means it's not IPv4
        //     continue;
        // }

        core::PrintF("read %ld bytes\n", rRes);
        core::PrintF("[ ");
        PrintBytes(buf, rRes);
        core::PrintF("]\n");
    }

    close(nic);
    return 0;
}
