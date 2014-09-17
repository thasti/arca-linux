#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

#include "udp.h"

UDP::UDP(const char *host, const char *port)
{
	int err;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family =	AF_UNSPEC;
	hints.ai_socktype =	SOCK_DGRAM;
	hints.ai_protocol =	0;
	hints.ai_flags =	AI_ADDRCONFIG;

	err = getaddrinfo(host, port, &hints, &this->res);
	if (err != 0) {
		cerr << "Failed to resolve remote socket address: " << err << endl;
		throw(UDPException());
	}

	this->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->fd == -1) {
		cerr << "Failed to create socket: " << strerror(errno) << endl;
		throw(UDPException());
	}
}

UDP::~UDP(void) {
	freeaddrinfo(this->res);
}

int UDP::send(const char *message) {
	int ret;

	ret = sendto(this->fd, message, strlen(message), 0, this->res->ai_addr, this->res->ai_addrlen);
	if (ret == -1) {
		cerr << "Could not send UDP packet: " << strerror(errno) << endl;
	}

	return ret;
}
