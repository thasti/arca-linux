#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "udp.h"

#define DEST_IP		"127.0.0.1"
#define DEST_PORT	"10000"
#define	UART_NAME	"/dev/ttyS1"
#define UART_BUF_SIZE	80
#define MODE_S_LENGTH	28
#define SEC_THRESH	1000

#define LOG_PREFIX		"DATA:\t"
#define FILENAME_PREFIX		"data/data-"
#define FILENAME_POSTFIX	".txt"
#define FILENAME		(string(FILENAME_PREFIX) + hwaddr + string("-") + itos(file_index) + string(FILENAME_POSTFIX)).c_str()

using namespace std;

string itos(int i) {
	stringstream s;
	s << i;
	return s.str();
}

void usage(char *argv[]) {
	cout << "ARCA data telemetry server" << endl;
	cout << "usage: " << argv[0] << " msgcount" << endl;
	cout << "  msgcount:   number of messages to get before transmitting" << endl;
}

int main(int argc, char *argv[]) {
	FILE *uart_fd;
	char uart_buf[UART_BUF_SIZE];
	char *read_buf = uart_buf;
	size_t len;

	vector<string> fifo;
	string txbuffer;
	int msg_counter = 0, msg_trigger = 1;
	int file_index = 0;
	unsigned long time_start;
	string hwaddr;

	system("mkdir -p data");
	ofstream output_file;
	ifstream eth_file("/sys/class/net/eth0/address");

	if (argc != 2) {
		usage(argv);
		return -1;
	}

	stringstream tokenizer;
	tokenizer << argv[1];
	tokenizer >> msg_trigger;

	UDP UDPtx(DEST_IP, DEST_PORT);

	// open serial port
	uart_fd = fopen(UART_NAME, "r");
	if (uart_fd == NULL) {
		cerr << LOG_PREFIX "Could not open UART, exiting." << endl;
		return -1;
	}

	eth_file >> hwaddr;
	time_start = time(NULL);
	output_file.open(FILENAME);
	output_file << time(NULL) << endl;
	cout << LOG_PREFIX "Data Server initalized." << endl;

	while(1) {
		if ((getline(&read_buf, &len, uart_fd)) != -1) {
			read_buf[MODE_S_LENGTH] = '\0';
			fifo.push_back(string(read_buf));
			msg_counter++;
			output_file << string(read_buf) << endl;
		}
		if (msg_counter >= msg_trigger) {
			txbuffer = "";
			txbuffer += itos(time(NULL)) + string(",") + itos(msg_trigger) + string("\n");
			while (!fifo.empty()) {
				txbuffer += fifo.back() + string("\n");
				fifo.pop_back();
			}
			UDPtx.send(txbuffer.c_str());
			cout << LOG_PREFIX "UDP Packet sent." << endl;
			msg_counter = 0;
		}
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
	}
	return 0;
}
