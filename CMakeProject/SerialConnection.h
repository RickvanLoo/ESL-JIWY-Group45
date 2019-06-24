/*
 * SerialConnection.h
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */

#ifndef SERIALCONNECTION_H_
#define SERIALCONNECTION_H_

// C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

class SerialConnection {
public:
	SerialConnection(const char *Handle); // No error detection
	int Connect(); // 1 = error
	int Config(); // 1 = error
	void Close(); // No error detection
	// SerialFuncions
	int Reset(); // 1 = error
	int HomingPan(); // 1 = error
	//int HomingTilt();
	int GetPan(); // No error detection
	//int GetTilt();
	int SetPan(int count); // 1 = error
	//int SetTilt(int count);
	int MinCountPan(); // No error detection
	int MaxCountPan(); // No error detection
	//int MinCountTilt();
	//int MaxCountTilt();
private:
	int WriteRead(unsigned char msg);
	int CheckOK(int n);
	unsigned int getBuffInt();
	const char *SerialHandle;
	char read_buf[5];
	int Serial_Port;
	struct termios tty;
};

#endif /* SERIALCONNECTION_H_ */
