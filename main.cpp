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
}

void PrintBytes(constptr u8 *_bytes, ptr_size _len)
{
    core::PrintF("[ ");
    for (ptr_size i = 0; i < _len; i++) {
        core::PrintF("0x%x ", _bytes[i]);
    }
    core::PrintF("]");
}

// TODO: Move IP Header parsing to net and test it.
// TODO: Implement TCP Header parsing in net and test it.

Optional<i32> ParseIPv4Header(u8 *_rawData, net::IPv4Header *_header)
{
    Assert(_rawData != null);
    Assert(_header != null);

    _header->Version = _rawData[0] >> 4;
    if (_header->Version != 4) {
        // expected version to equal 4
        return Optional<i32>((u32)core::ErrCodes::ERROR, "invalid IP version");
    }
    _header->InternetHeaderLength = _rawData[0] & 0xf;
    if (_header->InternetHeaderLength < 5) {
        // expect length to at minimum 5.
        return Optional<i32>((u32)core::ErrCodes::ERROR, "invalid internet header length");
    }
    _rawData += sizeof(u8);

    _header->TypeOfService = _rawData[0];
    _rawData += sizeof(u8);

    _header->TotalLength = core::SwapByteOrderU16(*(u16*)_rawData);
    _rawData += sizeof(u16);

    _header->Id = core::SwapByteOrderU16(*(u16*)_rawData);
    _rawData += sizeof(u16);

    // TODO: skiping flags.
    _rawData += sizeof(u16);

    _header->Ttl = _rawData[0];
    _rawData += sizeof(u8);

    _header->HeaderChecksum = core::SwapByteOrderU16(*(u16*)_rawData);
    _rawData += sizeof(u16);

    _header->SourceAddres = core::SwapByteOrderU32(*(u32*)_rawData);
    _rawData += sizeof(u32);

    _header->DestAddres = core::SwapByteOrderU32(*(u32*)_rawData);
    _rawData += sizeof(u32);

    return Optional<i32>((u32)core::ErrCodes::OK, null);
}

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

            // TODO: Remove debug print. Print received data:
            PrintBytes(ethFrame.Data, (ptr_size)recvBytes);
            core::PrintF("\n");

            net::IPv4Header header;
            Optional<i32> parseResOpt = ParseIPv4Header(ethFrame.Data, &header);
            if (parseResOpt.err != null) {
                core::PrintF("failed to parse ipv4 header: ");
                core::PrintF(parseResOpt.err);
                core::PrintF("\n");
                continue;
            }

            core::PrintF("source address: %u, dest address: %u\n",
                        header.SourceAddres, header.DestAddres);
        } else {
            // Can't handle protocol.
            core::PrintF("Unknown protocol.\n");
            continue;
        }
    }

    core::OsClose(nic);
    return 0;
}
