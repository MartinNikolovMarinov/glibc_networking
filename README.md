<!-- TOC -->

- [Project Description](#project-description)
- [TUN/TAP device driver description.](#tuntap-device-driver-description)
- [Unix Network Socket control flow diagram](#unix-network-socket-control-flow-diagram)
- [System calls](#system-calls)
    - [1. How does `getaddrinfo` work:](#1-how-does-getaddrinfo-work)
    - [2. The socket system call:](#2-the-socket-system-call)
    - [3. The bind system call:](#3-the-bind-system-call)
    - [4. The connect system call:](#4-the-connect-system-call)
    - [5. The listen system call:](#5-the-listen-system-call)
    - [6. The accept system call:](#6-the-accept-system-call)

<!-- /TOC -->

# Project Description

This project was created for educational purpose. The main goal is to explore low level C/C++ networking Linux apis and try to partially implement some RFCs of core networking protocols, like TCP.

# Requirements

1. Any modern Linux OS.

# TUN/TAP device driver description.

> TUN/TAP provides packet reception and transmission for user space programs. It can be seen as a simple Point-to-Point or Ethernet device, which, instead of receiving packets from physical media, receives them from user space program and instead of sending packets via physical media writes them to the user space program.

> In order to use the driver a program has to open **/dev/net/tun** and issue a corresponding **ioctl()** to register a network device with the kernel. A network device will appear as **tunXX** or **tapXX**, depending on the options chosen. When the program closes the file descriptor, the network device and all corresponding routes will disappear.

> Depending on the type of device chosen the userspace program has to read/write IP packets (with tun) or ethernet frames (with tap). Which one is being used depends on the flags given with the **ioctl()**.

The description comes from www.kernel.org documentation.

The project uses a tun device to receive raw packets directly in user space. The kernel treats this tun device as if it's a network card. In other words, when the kernel calls **send** with some raw bits, that will turn into a **recv** call in the user space process and vice versa.

# Unix Network Socket control flow diagram

![Unix Socket Diagram](./docs/unix_socket.png)

# System calls

##  1. How does `getaddrinfo` work:

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

int getaddrinfo(
    const char *node, // e.g. "www.example.com" or IP
    const char *service, // e.g. "http" or port number
    const struct addrinfo *hints,
    struct addrinfo **res
);
```

##  2. The socket system call:

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(
    int domain, // IPv4 or IPv6
    int type, // SOCK_DGRAM or SOCK_STREAM
    int protocol // TCP or UDP
);
```

##  3. The bind system call:

Bind the socket to a port on the local machine. Can be skipped in which case the kernel decides on a random free port.

```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(
    int sockfd, // sockfd is the socket file descriptor returned by socket()
    struct sockaddr *my_addr, // struct with port and IP address
    int addrlen // length of the struct in bytes
);
```

##  4. The connect system call:

Bind the socket to a port on a server someware.

```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(
    int sockfd, // sockfd is the socket file descriptor returned by socket()
    struct sockaddr *serv_addr, // struct with port and IP address
    int addrlen // length of the struct in bytes
);
```

##  5. The listen system call:

Listen for client requests on a given socket. Incoming connections are
going to wait in this queue until you call `accept()`.

```c
#include <sys/types.h>
#include <sys/socket.h>

// sockfd is the socket file descriptor returned by socket()
// backlog is the number of connections allowed on the incoming queue.
int listen(int sockfd, int backlog);
```

##  6. The accept system call:

```c
#include <sys/types.h>
#include <sys/socket.h>

// accept returns a new file descriptor that is used for sending a respose to the client.
int accept(
    int sockfd, // sockfd is the listen() ing socket descriptor.
    struct sockaddr *addr,  // struct holding information for the incoming traffic.
    socklen_t *addrlen // length of the struct in bytes
);
```
