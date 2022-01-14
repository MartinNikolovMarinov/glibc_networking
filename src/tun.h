#ifndef TUN_H
#define TUN_H 1

#include "../libs/core/src/core.h"
#include "../libs/core/src/types.h"

#include <linux/if.h>
#include <linux/if_tun.h>

#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

namespace tun
{

/**
 * \brief Open a TUN device that emulates a network inside user space.
 *
 * \param devname is the name of the device. Should be "tun0", "tun1" ... "tunXX".
 *
 * \returns returns an Optional with the file descriptor of the tun device.
*/
Optional<i32> TUNOpen(constptr char *devname);

} // namespace tun

#endif
