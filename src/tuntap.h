#ifndef TUN_TAP_H
#define TUN_TAP_H 1

#include "../libs/core/src/core.h"
#include "../libs/core/src/types.h"

#include "net.h"

#include <linux/if.h>
#include <linux/if_tun.h>

#include <sys/ioctl.h>

#include <errno.h>
#include <string.h>

namespace tuntap
{

/**
 * \brief Open a TUN device that emulates a network inside user space.
 *
 * \param _devname is the name of the device. Should be "tun0", "tun1" ... "tunXX".
 *
 * \returns returns an Optional with the file descriptor of the tun device.
*/
Optional<i32> TUNOpen(constptr char *_devname);

struct TunEthFrame {
    u16 Flags;    // ioctl tun/tap flags.
    u16 Protocol; // Frame protocol.
    u8 *Data;     // Raw data IPv4, IPv6, ARP, etc.
};

} // namespace tun

#endif
