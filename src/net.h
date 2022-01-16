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

    /**
     * The IPv4 header as defiend by RFC791.
     */
    struct IPv4Header {
        /**
         * Indicates the version of the IP protocol. In the case of IPv4 it always equals 4.
        */
        u8 Version;
        /**
         * Indicates the number of 4-byte blocks in the IPv4 header. The size of this field is 4 bits.
         * Because an IPv4 header is a minimum of 20 bytes in size, the smallest value of the
         * Internet Header Length (IHL) field is 5.
         */
        u8 InternetHeaderLength;
        /**
         * The Type of Service provides an indication of the abstract parameters of the quality of service desired.
         * These parameters are to be used to guide the selection of the actual service parameters when transmitting
         * a datagram through a particular network. NOTE: This field is largley ignored by the code base.
         * For more information see RFC791 page 12.
        */
        u8 TypeOfService;
        /**
         * Total Length is the length of the datagram, measured in bytes, including internet header and data.
        */
        u16 TotalLength;
        /**
         * An identifying value assigned by the sender to aid in assembling the fragments of a datagram.
        */
        u16 Id;
        /**
         * Various Control Flags.
         * For more information see RFC791 page 13.
        */
        u8 Flags;
        /**
         * This field indicates where in the datagram this fragment belongs. The first fragment has offset zero.
         * For more information see RFC791 page 13-14.
        */
        u16 FragOffset;
        /**
         * This field indicates the maximum time the datagram is allowed to remain in the internet system.
         * If this field contains the value zero, then the datagram must be destroyed. This field is modified
         * in internet header processing. The time is measured in units of seconds, but since every module that
         * processes a datagram must decrease the TTL by at least one even if it process the datagram in less than a
         * second, the TTL must be thought of only as an upper bound on the time a datagram may exist. The intention
         * is to cause undeliverable datagrams to be discarded, and to bound the maximum datagram lifetime.
        */
        u8 Ttl;
        /**
         * This field indicates the next level protocol used in the data portion of the internet datagram.
        */
        u8 Protocol;
        /**
         * A checksum on the header only. Since some header fields change (e.g., time to live), this is recomputed
         * and verified at each point that the internet header is processed.
         * For more information see RFC791 page 13-14.
        */
        u16 HeaderChecksum;
        /**
         * SourceAddress as defined by RFC791 section-3.2
        */
        u32 SourceAddres;
        /**
         * DestAddress as defined by RFC791 section-3.2
        */
        u32 DestAddres;
    };

} // namespace net

#endif
