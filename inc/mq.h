#ifndef __mq_h__
#define __mq_h__
#include <mqueue.h>
#include <exception>
#include <stdexcept>

using namespace std;

#define MQ_MAX_MSG	50
#define MQ_READ		0
#define MQ_WRITE	1

/* MQException
 *
 * thrown, when the message queue could not be opened.
 */
class MQException : public std::runtime_error
{
	public:
		MQException() : std::runtime_error("MQException") { }
};


/* MessageQueue class */
class MQ
{
	mqd_t 	m_mq;
	int	m_msgsize;

	public:
		MQ(char *mq_name, int msgsize, int mode);
		~MQ(void);
		int	sendMessage(char *message);
		int	receiveMessage(char *message, int timeout);
};

#endif
