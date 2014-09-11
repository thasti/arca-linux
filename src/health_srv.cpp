#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "udp.h"

#define DEST_IP		"127.0.0.1"
#define DEST_PORT	"10001"
#define	SENSOR_PREFIX	"/sys/class/hwmon/hwmon"
#define SENSOR_POSTFIX	"/temp1_input"
#define NUM_SENSORS	6

#define LOG_PREFIX	"HLT:\t"

using namespace std;

string itos(int i) {
	stringstream s;
	s << i;
	return s.str();
}

string ltos(signed long l) {
	stringstream s;
	s << l;
	return s.str();
}

void usage(char *argv[]) {
	cout << "ARCA health telemetry server" << endl;
	cout << "usage: " << argv[0] << " interval" << endl;
	cout << "  interval:   time in seconds between measurements" << endl;
}

int main(int argc, char *argv[]) {
	ifstream file;
	string txbuf;
	int i, interval;
	signed long temp;

	if (argc != 2) {
		usage(argv);
		return -1;
	}

	stringstream tokenizer;
	tokenizer << argv[1];
	tokenizer >> interval;

	UDP UDPtx(DEST_IP, DEST_PORT);

	cout << LOG_PREFIX "Health server initialized." << endl;
	while(1) {
		txbuf = "";
		txbuf += itos(time(NULL));
		for (i = 0; i < NUM_SENSORS; i++) {
			file.open((string(SENSOR_PREFIX) + itos(i) + SENSOR_POSTFIX).c_str());
			if (file.good()) {
				file >> temp;
			} else {
				temp = 0;
			}
			file.close();
			txbuf += string(",") + ltos(temp);
		}
		cout << LOG_PREFIX "Packet sent." << endl;
		txbuf += string("\n");
		UDPtx.send(txbuf.c_str());
		sleep(interval);
	}
	return 0;
}
