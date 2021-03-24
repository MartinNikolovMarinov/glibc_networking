# (WIP) Unix Network Socket control flow

![Unix Socket Diagram](./imgs/unix_socket.png)

# How does `getaddrinfo` work:

* Uses `/etc/gai.conf` (file name comes from RFC 3484). `/etc/gai.conf` contains three types of information: A label table, a precedence table, and an IPv4 scopes table. The label information is needed by source address selection; all three types of information are needed by destination address selection.

* Uses `/etc/services` to resolve port numbers and protocols for known services. For example "http" is resolved to `80/tcp`.

* Uses `/etc/resolv.conf` this file, most noteably, can define which dns server to use, for example `nameserver 8.8.8.8` to set google's DNS server. The resolver is a set of routines in the C library that provide access to the Internet Domain Name System (DNS). The resolver configuration file contains information that is read by the resolver routines the first time they are invoked by a process.

* Uses `/etc/nsswitch.conf` "Name Service Switch" to determine the sources from which to obtain name-service information in a range of categories, and in what order. For example `files dns` means first look in `/etc/hosts` then consult a dns server like `8.8.8.8:53`.

* Uses `/etc/host.conf` which  tells the resolver which services to use, and in what order. Is ignored in newer implementation of libc.

* Uses `/etc/hosts` is a Host Name to IP resolution table which usually is the first place programs use to resolve a name to an IP address.

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node, // e.g. "www.example.com" or IP
    const char *service, // e.g. "http" or port number
    const struct addrinfo *hints,
    struct addrinfo **res);
```

# The socket system call:

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain,  // IPv4 or IPv6
    int type,           // SOCK_DGRAM or SOCK_STREAM
    int protocol);      // TCP or UDP
```

# The bind system call:

Bind the socket to a port on the local machine. Can be skipped in which case the kernel decides on a random free port.

```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, // sockfd is the socket file descriptor returned by socket()
    struct sockaddr *my_addr, // struct with port and IP address
    int addrlen); // length of the struct in bytes
```

# The connect system call:

Bind the socket to a port on a server someware.

```c
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, // sockfd is the socket file descriptor returned by socket()
    struct sockaddr *serv_addr, // struct with port and IP address
    int addrlen); // length of the struct in bytes
```

# The listen system call:

Listen for client requests on a given socket. Incoming connections are
going to wait in this queue until you call `accept()`.

```c
#include <sys/types.h>
#include <sys/socket.h>

// sockfd is the socket file descriptor returned by socket()
// backlog is the number of connections allowed on the incoming queue.
int listen(int sockfd, int backlog);
```

# The accept system call:

```c
#include <sys/types.h>
#include <sys/socket.h>

// accept returns a new file descriptor that is used for sending a respose to the client.

int accept(int sockfd, // sockfd is the listen() ing socket descriptor.
    struct sockaddr *addr,  // struct holding information for the incoming traffic.
    socklen_t *addrlen); // length of the struct in bytes
```
