#include "tun.h"

namespace tun
{

/**
 * \brief Open a TUN device that emulates a network inside user space.
 *
 * \param devname is the name of the device. Should be "tun0", "tun1" ... "tunXX".
 *
 * \returns returns an Optional with the file descriptor of the tun device.
*/
Optional<i32> TUNOpen(constptr char *devname)
{
    Assert(devname != NULL);

    i32 tunFd = open("/dev/net/tun", O_RDWR);
    if (tunFd < 0) {
        return Optional<i32>(tunFd, strerror(errno));
    }

    struct ifreq ifr = {};
    ifr.ifr_flags = IFF_TUN;
    core::CharPtrCopy(devname, IFNAMSIZ, ifr.ifr_name);

    int ioctlRes;
    if ((ioctlRes = ioctl(tunFd, TUNSETIFF, (void *) &ifr)) == -1 ) {
        return Optional<i32>(ioctlRes, strerror(errno));
    }

    return Optional<i32>(tunFd, null);
}

} // namespace tun