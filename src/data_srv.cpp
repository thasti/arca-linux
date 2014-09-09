#include <fstream>
#include <cstdio>
#include <iostream>
#include <string>
#include "mq.h"

#define	UART_NAME	"/dev/ttyACM0"
#define UART_BUF_SIZE	80
#define MODE_S_LENGTH	28

int main(void) {
	FILE *uart_fd;
	char uart_buf[UART_BUF_SIZE];
	char *read_buf = uart_buf;
	size_t len;
	int read;

	char mq_name[] = "/arca-payload-data";

	// open message queue
	MQ txmq(mq_name, MODE_S_LENGTH, MQ_WRITE);

	// open serial port
	uart_fd = fopen(UART_NAME, "r");
	if (uart_fd == NULL) {
		cerr << "Could not open UART, exiting." << endl;
		return -1;
	}

	cout << "Data Server initalized." << endl;

	while(1) {
		// get one line from UART
		if((read = getline(&read_buf, &len, uart_fd)) != -1) {
			// hand that message to the queue
			txmq.sendMessage(uart_buf);
			cout << "Message sent." << endl;
		}
	}
	return 0;
}
