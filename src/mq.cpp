#include <errno.h>
#include <mqueue.h>
#include <iostream>

#include "mq.h"

MQ::MQ(char *mq_name, int msgsize, int mode) : m_msgsize(msgsize) {
	struct mq_attr attr;
	char data[10];
	int mq_mode;

	attr.mq_flags = 0;
	attr.mq_maxmsg = MQ_MAX_MSG;
	attr.mq_msgsize = msgsize;
	attr.mq_curmsgs = 0;
	mq_mode = O_CREAT;
	/* writeable MQ should drop messages if message box is full,
	 * therefore O_NONBLOCK is used.
	 * reception shall use a timeout
	 */
	if (mode == MQ_READ) {
		mq_mode |= O_RDONLY;
	} else {
		mq_mode |= O_WRONLY | O_NONBLOCK;;
	}

	// open the message queue
	m_mq = mq_open(mq_name, mq_mode, 0664, &attr);

	// test for successful completion
	if (m_mq == (mqd_t)-1) {
		cerr << "Message queue open error " << errno << endl;
		throw(MQException());
	}
}

MQ::~MQ(void) {
	mq_close(m_mq);
}

int MQ::sendMessage(char *message) {
	int ret;
	ret = mq_send(m_mq, message, m_msgsize, 1);
	return ret;
}

int MQ::receiveMessage(char *message, int timeout) {
	int ret;
	struct timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);
	tm.tv_sec += timeout;
	ret = mq_timedreceive(m_mq, message, m_msgsize, NULL, &tm);
	return ret;
}

