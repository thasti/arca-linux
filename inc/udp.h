#ifndef __udp_h__
#define __udp_h__
#include <exception>
#include <stdexcept>

using namespace std;

#define MQ_MAX_MSG	50
#define MQ_READ		0
#define MQ_WRITE	1

/* UDPException
 *
 * thrown, when the message queue could not be opened.
 */
class UDPException : public std::runtime_error
{
	public:
		UDPException() : std::runtime_error("UDPException") { }
};


/* UDP Socket class */
class UDP
{
	int fd;
	struct addrinfo *res;

	public:
		UDP(const char *host, const char *port);
		~UDP(void);
		int	send(const char *message);
};

#endif
