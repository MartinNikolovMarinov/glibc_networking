#ifndef TUN_H
#define TUN_H 1

#include "../libs/core/src/core.h"
#include "../libs/core/src/types.h"

#include "net.h"

#include <linux/if.h>
#include <linux/if_tun.h>

#include <sys/ioctl.h>

#include <errno.h>
#include <string.h>

namespace tun
{

/**
 * \brief Open a TUN device that emulates a network inside user space.
 *
 * \param _devname is the name of the device. Should be "tun0", "tun1" ... "tunXX".
 *
 * \returns returns an Optional with the file descriptor of the tun device.
*/
Optional<i32> TUNOpen(constptr char *_devname);

struct TunTapFrame {
    u16 Flags;                  // ioctl tun/tap flags.
    net::EtherType Protocol;    // Frame protocol.
    void *FrameData;            // Raw data IPv4, IPv6, ARP, etc.
};

} // namespace tun

#endif
