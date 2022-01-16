#include "libs/core/src/core.h"
#include "src/tuntap.h"
#include "src/net.h"

#include <signal.h>

global_variable i32 nic;

void SignalHandler(i32 _signal)
{
    if (nic > 0) {
        core::OsClose(nic);
    }
    core::OsExit(_signal);
};

void PrintBytes(constptr u8 *_bytes, ptr_size _len)
{
    core::PrintF("[ ");
    for (ptr_size i = 0; i < _len; i++) {
        core::PrintF("0x%x ", _bytes[i]);
    }
    core::PrintF("]");
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

    nic = TryOrFail(tuntap::TUNOpen("tun0"));
    const int bufSize = 1504;
    uchar buf[bufSize] = {};

    while (true) {
        auto readRes = core::OsRead(nic, buf, bufSize);
        if (readRes.err != null) {
            core::PrintF("err code: %d, err msg: %s", readRes.val, readRes.err);
            return -1;
        }
        signed_ptr_size recvBytes = readRes.val;

        tuntap::TunEthFrame ethFrame;
        ethFrame.Flags = core::SwapByteOrderU16(*(u16 *)(buf));
        ethFrame.Protocol = core::SwapByteOrderU16(*(u16 *)(buf + sizeof(u16)));
        core::PrintF("bytes received: %llu, flags: %d, protocol: %s\n",
                        recvBytes, ethFrame.Flags,
                        net::EtherTypeToCharPtr((net::EtherType)ethFrame.Protocol));

        if (ethFrame.Protocol == (u16)net::EtherType::IPv4) {
            // Set frame data past the 2 bytes of protocol data:
            ethFrame.Data = buf + 2*sizeof(u16);

            // Print received data:
            PrintBytes(ethFrame.Data, (ptr_size)recvBytes);
            core::PrintF("\n");

            // TODO: Implement IP protocol parsing...

        } else {
            // Can't handle protocol.
            core::PrintF("Unknown protocol.\n");
            continue;
        }
    }

    core::OsClose(nic);
    return 0;
}
