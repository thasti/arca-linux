#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "errors.h"
#include "tcpacceptor.h"

#define TCP_PORT	10002
#define LOG_PREFIX	"TLM:\t"

#define CHECK_ARGC(x) 	if (argv.size() != (x)+1) {retval << ECMDMALFORMED; break; }

using namespace std;

template <typename T>
T stringToNumber ( const string &Text )
{
	stringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}

enum Commands {
	resetARM = 1,
	shutdownARM,
	resetFPGA,
	getLoad,
	syncDisk,
	timeUpload,
	ping};

static map<string, Commands> CommandMap;

void initCommandMap(void)
{
	CommandMap["RA"] = resetARM;
	CommandMap["SA"] = shutdownARM;
	CommandMap["RF"] = resetFPGA;
	CommandMap["GL"] = getLoad;
	CommandMap["SY"] = syncDisk;
	CommandMap["TU"] = timeUpload;
	CommandMap["PI"] = ping;
}

string parse(string cmd) {
	unsigned long time;
	int ret;
	double loadavg[3];
	string command;
	ofstream file;

	stringstream retval;
	stringstream ss(cmd);
	istream_iterator<string> begin(ss);
	istream_iterator<string> end;
	vector<string> argv(begin, end);
	// empty command was sent
	if (argv.size() == 0) {
		retval << "NC";
	} else {
		switch(CommandMap[argv[0]])
		{
			case resetARM:
				CHECK_ARGC(0);
				file.open("/dev/watchdog0");
				file << "0";
				retval << ENOERROR;
				break;
			case shutdownARM:
				CHECK_ARGC(0);
				ret = system("halt");
				if (ret == 0) {
					retval << ENOERROR;
				} else {
					retval << ERETURN;
				}
				break;
			case resetFPGA:
				CHECK_ARGC(0);
				system("echo 1 > /sys/class/gpio/gpio96/value");
				system("echo 0 > /sys/class/gpio/gpio96/value");
				retval << ENOERROR;
				break;
			case getLoad:
				CHECK_ARGC(0);
				ret = getloadavg(loadavg, 3);
				if (ret > 0) {
					retval << ENOERROR << "," << loadavg[0] << "," << loadavg[1] << "," << loadavg[2];
				} else {
					retval << ERETURN;
				}
				break;
			case syncDisk:
				CHECK_ARGC(0);
				ret = system("sync");
				if (ret == 0) {
					retval << ENOERROR;
				} else {
					retval << ERETURN;
				}
				break;
			case timeUpload:
				CHECK_ARGC(1);
				command = "date --set='@" + argv[1] + "'";
				ret = system(command.c_str());
				if (ret != 0) {
					retval << ERETURN;
					break;
				}
				retval << ENOERROR;
				break;
			case ping:
				CHECK_ARGC(0);
				retval << ENOERROR;
				break;
			case 0:
			default:
				retval << ECMDUNKNOWN;
		}
	}
	retval << endl;
	return retval.str();
}

void usage(char *argv[]) {
	cout << "ARCA uplink telemetry server" << endl;
	cout << "usage: " << argv[0] << " " << endl;
	cout << "call without any arguments." << endl;
}

int main(int argc, char *argv[]) {
	// TCP server
	TCPStream *stream = NULL;
	TCPAcceptor *acceptor = NULL;

	if (argc != 1) {
		usage(argv);
		return -1;
	}

	initCommandMap();

	acceptor = new TCPAcceptor(TCP_PORT);
	if (acceptor->start() != 0) {
		cerr << LOG_PREFIX "Could not start TCPAcceptor" << endl;
		return -1;
	}

	cout << LOG_PREFIX "Initialized TLM Server" << endl;
	while(1) {
		cout << LOG_PREFIX "Waiting for connection." << endl;
		stream = acceptor->accept();
		cout << LOG_PREFIX "Connection Accepted, sending telemetry. " << endl;
			if (stream != NULL) {
				ssize_t len;
				char line[256];
				string retval;
				const char *txstr;
				while ((len = stream->receive(line, sizeof(line))) > 0) {
					line[len] = 0;
					retval = parse(line);
					txstr = retval.c_str();
					stream->send(txstr, retval.length());
				}
			delete stream;
			cout << LOG_PREFIX "Connection closed." << endl;
		}
	}
	delete acceptor;

	return 0;
}
