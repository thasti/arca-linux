#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include "tcpacceptor.h"
#include "mq.h"

#define MQ_NAME_LENGTH 30

using namespace std;

void usage(char *argv[]) {
	cout << "ARCA telemetry server" << endl;
	cout << "usage: " << argv[0] << " port mq msgs" << endl;
	cout << "  mq:   name of the message queue, starting with a /" << endl;
	cout << "  port: TCP port where the socket is created " << endl;
	cout << "  size: size of one message, in bytes" << endl;
	cout << "  msgs: number of messages to trigger packet transmission" << endl;
}

int main(int argc, char *argv[]) {
	// TCP server
	TCPStream *stream = NULL;
	TCPAcceptor *acceptor = NULL;

	// argument parsing tokenizer
	stringstream tokenizer;
	int port, msg_trigger, msg_size, msg_counter=0;
	char mq_name[MQ_NAME_LENGTH] = { 0 };
	char *mqbuf;
	vector<string> rxbuf;
	string txbuf;

	if (argc != 5) {
		usage(argv);
		return -1;
	}

	// parse command line options
	tokenizer << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4];
	tokenizer >> mq_name >> port >> msg_size >> msg_trigger;
	cout << "Name: " << mq_name << ", Port: " << port << ", Size: " << msg_size << ",  Messages: " << msg_trigger << endl;

	MQ rxmq(mq_name, msg_size, MQ_READ);
	mqbuf = (char *)malloc(msg_size);
	if (mqbuf == NULL) {
		cerr << "Could not allocate buffer" << endl;
		return -1;
	}

	acceptor = new TCPAcceptor(port);
	if (acceptor->start() == 0) {
		cout << "Initialized TLM Server, Message queue opened." << endl;
		while(1) {
			cout << "Waiting for connection." << endl;
			stream = acceptor->accept();
			cout << "Connection Accepted, sending telemetry. " << endl;
			while (stream != NULL) {
				if (rxmq.receiveMessage(mqbuf, 1) > 0) {
					cout << "Message received." << endl;
					msg_counter++;
					rxbuf.push_back(string(mqbuf));
				}
				if (msg_counter > msg_trigger) {
					txbuf = "";
					cout << "Packet transmission triggered." << endl;
					while(!rxbuf.empty()) {
						txbuf = txbuf + rxbuf.back() + string("\n");
						rxbuf.pop_back();
					}
					stream->send(txbuf.c_str(), txbuf.size());
					msg_counter = 0;
				}
			}

		}
	}
	free(mqbuf);
	return 0;
}
