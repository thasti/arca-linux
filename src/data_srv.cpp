#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "udp.h"

#define DEST_IP		"127.0.0.1"
#define DEST_PORT	"10000"
#define	UART_NAME	"/dev/ttyUSB0"
#define UART_BUF_SIZE	80
#define MODE_S_LENGTH	28
#define DATA_PER_FILE	10000

#define LOG_PREFIX	"DATA:\t"

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
	int file_counter = 0;

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

	system("mkdir -p data");

	cout << LOG_PREFIX "Data Server initalized." << endl;

	while(1) {
		if ((getline(&read_buf, &len, uart_fd)) != -1) {
			read_buf[MODE_S_LENGTH] = '\0';
			fifo.push_back(string(read_buf));
			msg_counter++;
			// write to open file
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
	}
	return 0;
}
