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

    /**
     * \brief Converts the ether type to a char pointer.
     *
     * \returns a char pointer that is a literal.
    */
    constptr char* EtherTypeToCharPtr(EtherType _e);

    /**
     * Specifies the operation that the sender is performing: 1 for request, 2 for reply.
     * See RFC6747 for more information on the IPv4 based ARP protocol.
    */
    enum struct ArpOpCode : u16 {
        ARP_REQUEST = 1,
        ARP_REPLAY = 2,
        RARP_REQUEST = 3,
        RARP_REPLAY = 4
    };

    /**
     * The generic ARP header, which is agnostic to used Network Layer protocol, as defined by RFC826.
    */
    struct ArpHeader {
        u16 HWType;         // Link Layer Hardware Type used, like Ethernet, or IEEE 802.11 for wireless LAN
        u16 Protocol;       // The type of protocol. Could be Ether Type.
        u8 HWSize;          // The size of the hardware field.
        u8 ProtocolSize;    // The size of the protocol field.
        ArpOpCode OPCode;   // The operation field.
        u8 *Data;           // The actual payload of the ARP message.
    };

    /**
     * The ARP based on IPv4 payload, as defined by RFC6747.
    */
    struct ArpIPv4Payload {
        u8 SenderMAC[6]; // Sender MAC address.
        u32 SenderIP;    // Sender IP address.
        u8 DestMAC[6];   // Destination MAC address.
        u32 DestIP;      // Destination IP addreess.
    };

} // namespace net

#endif
