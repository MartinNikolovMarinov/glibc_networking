#include "net.h"

namespace net
{

constptr char* EtherTypeToCharPtr(EtherType e)
{
    switch (e)
    {
        case EtherType::IPv4:   return "IPv4";
        case EtherType::IPv6:   return "IPv6";
        case EtherType::ARP:    return "ARP";
        default:                return "Unknown";
    }
}

} // namespace net