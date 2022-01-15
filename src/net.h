#ifndef NET_H
#define NET_H 1

#include "../libs/core/src/types.h"

namespace net
{
    /**
     * EtherType is a two byte field in an Ethernet frame. It is used to indicate which protocol is
     * encapsulated in the payload of the frame and is used at the receiving end by the data link
     * layer to determine how the payload is processed.
    */
    enum struct EtherType : u16 {
        IPv4 = 0x0800, // Internet Protocol version 4 (IPv4)
        IPv6 = 0x86DD, // Internet Protocol version 6 (IPv6)
        ARP = 0x0806,  // Address Resolution Protocol (ARP)
    };

    constptr char* EtherTypeToCharPtr(EtherType e);

} // namespace net

#endif
