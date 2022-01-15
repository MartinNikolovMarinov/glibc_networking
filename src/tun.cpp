#include "tun.h"

namespace tun
{

Optional<i32> TUNOpen(constptr char *_devname)
{
    Assert(_devname != NULL);

    i32 tunFd = TryOrReturn(core::OsOpen("/dev/net/tun", core::OpenFlag::READ_WRITE));
    if (tunFd < 0) {
        return Optional<i32>(tunFd, strerror(errno));
    }

    struct ifreq ifr = {};
    ifr.ifr_flags = IFF_TUN;
    core::CharPtrCopy(_devname, IFNAMSIZ, ifr.ifr_name);

    int ioctlRes;
    if ((ioctlRes = ioctl(tunFd, TUNSETIFF, (void *) &ifr)) == -1 ) {
        return Optional<i32>(ioctlRes, strerror(errno));
    }

    return Optional<i32>(tunFd, null);
}

} // namespace tun