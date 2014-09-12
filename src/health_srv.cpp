#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "udp.h"

#define DEST_IP		"172.16.18.111"
#define DEST_PORT	"10001"
#define	SENSOR_PREFIX	"/sys/class/hwmon/hwmon"
#define SENSOR_POSTFIX	"/temp1_input"
#define NUM_SENSORS	6
#define SEC_THRESH	900

#define LOG_PREFIX	"HLT:\t"
#define FILENAME_PREFIX		"data/health-"
#define FILENAME_POSTFIX	".txt"
#define FILENAME		(string(FILENAME_PREFIX) + hwaddr + string("-") + itos(file_index) + string(FILENAME_POSTFIX)).c_str()

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

	// file handling
	int file_index = 0;
	unsigned long time_start;
	string hwaddr;
	ofstream output_file;
	ifstream eth_file("/sys/class/net/eth0/address");

	system("mkdir -p data");

	if (argc != 2) {
		usage(argv);
		return -1;
	}

	stringstream tokenizer;
	tokenizer << argv[1];
	tokenizer >> interval;

	UDP UDPtx(DEST_IP, DEST_PORT);

	// init file handling
	eth_file >> hwaddr;
	time_start = time(NULL);
	output_file.open(FILENAME);
	output_file << time(NULL) << endl;
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
		output_file << string(txbuf) << endl;
		txbuf += string("\n");
		UDPtx.send(txbuf.c_str());

		/* note: when the time gets set back by much, this won't work correctly.
		 * either set time forward or don't!
		 */
		if (time(NULL) - time_start > SEC_THRESH) {
			output_file.close();
			output_file.clear();
			file_index++;
			output_file.open(FILENAME);
			output_file << time(NULL) << endl;
			time_start = time(NULL);
		}

		// blocking wait until next measurement
		sleep(interval);
	}
	return 0;
}
