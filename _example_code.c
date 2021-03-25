#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int dns_lookup(char *host, char *service, char* out) {
	int status;
	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	// AF_UNSPEC means I don't care about using IPv4 (can be AF_INET) or IPv6 (can be AF_INET6):
	hints.ai_family = AF_UNSPEC;
	// SOCK_STREAM means we use TCP:
	hints.ai_socktype = SOCK_STREAM;
	// AI_PASSIVE  tells getaddrinfo() to assign the address of my local host to the socket structures:
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(host, service, &hints, &res)) != 0) {
		// can check status with gai_strerror(status)
		return status;
	}

	struct addrinfo *p;
	// ai_next points at the next element - there could be several results for you to choose from.
	char ipstr[INET6_ADDRSTRLEN];
	for(p = res; p != NULL; p = p->ai_next) {
		void *addr;
		char *ip_version;
		if (p->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ip_version = (char*)"IPv4";
		} else if (p->ai_family == AF_INET6) {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ip_version = (char*)"IPv6";
		} else {
			return 1;
		}

		// ipstr is the human readable of the address and the network protocol family
		// ntop - means network to presentation.
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		int written = sprintf(out, "%s:%s;", ip_version, ipstr);
		if (written < 0) {
			return 1;
		}

		out += written;
	}

	freeaddrinfo(res); // free the linked-list
	return 0;
}

int main(int argc, char const *argv[]) {
	struct addrinfo hints, *res;

	int status = getaddrinfo(NULL, "1235", NULL, &res);
	if (status != 0) {
		fprintf(stderr, "failed with %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	// PRINT protocol used:
	struct protoent *protocol = getprotobynumber(res->ai_protocol);
	printf("Protocol is %s\n", protocol->p_name);

	// PRINT socket type
	switch (res->ai_socktype) {
		case SOCK_DGRAM:
			printf("Socket type is SOCK_DGRAM(%d)\n", res->ai_socktype);
			break;
		case SOCK_STREAM:
			printf("Socket type is SOCK_STREAM(%d)\n", res->ai_socktype);
			break;
		default:
			printf("Unsupported socket type = %d\n", res->ai_socktype);
			exit(EXIT_FAILURE);
	}

	// PRINT network protocol IPv4 or IPv6
	switch (res->ai_family) {
		case AF_INET:
			printf("Protocol family is AF_INET(%d) IPv4\n", res->ai_family);
			break;
		case AF_INET6:
			printf("Protocol family is AF_INET6(%d) IPv6\n", res->ai_family);
			break;
		case AF_UNSPEC:
			printf("Protocol family is AF_UNSPEC(%d) Either\n", res->ai_family);
			break;
		default:
			printf("Protocol family is invalid = %d\n", res->ai_family);
			exit(EXIT_FAILURE);
	}

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

#define USE_AS_CLIENT 0
#if USE_AS_CLIENT
	int conn = connect(sockfd, res->ai_addr, res->ai_addrlen);
	if (conn < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}
#else
	int b_ret = bind(sockfd, res->ai_addr, res->ai_addrlen);
	if (b_ret < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	int l_ret = listen(sockfd, 20);
	if (l_ret < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof their_addr;

	int resp_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
	if (resp_fd < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	char *msg = "Please send me something!\n";
	// ssize_t !! ss not just s !!
	ssize_t len = strlen(msg);
	int bytes_sent = send(resp_fd, msg, len, 0);
	if (bytes_sent < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	// ssize_t !! ss not just s !!
	char got[1000];
	ssize_t resp = recv(resp_fd, got, 1000, 0);
	if (resp < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}
	printf("Got this from you: %s", got);

#endif

	int x;
	x = shutdown(sockfd, 2);
	if (x < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}
	x = shutdown(resp_fd, 2);
	if (x < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	x = close(sockfd);
	if (x < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	x = close(resp_fd);
	if (x < 0) {
		const char *err = strerror(errno);
		fprintf(stderr, "failed with %s\n", err);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}