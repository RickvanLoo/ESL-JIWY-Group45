/*
 * SerialConnection.cpp
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */

#include "SerialConnection.h"

SerialConnection::SerialConnection(const char *Handle) {
	// TODO Auto-generated constructor stub
	this->SerialHandle = Handle;

}

int SerialConnection::Connect(){
	int serial_port = open(this->SerialHandle, O_RDWR);
	this->Serial_Port = serial_port;

	if (serial_port < 0) {
	    printf("Error %i from open: %s\n", errno, strerror(errno));
	    return 1;
	}

	return 0;
}

int SerialConnection::Config(){

	// Create new termios struc, we call it 'tty' for convention
	struct termios tty;
	memset(&tty, 0, sizeof tty);

	// Read in existing settings, and handle any error
	if(tcgetattr(this->Serial_Port, &tty) != 0) {
	    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	    return 1;
	}

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD | CLOCAL; //Pls check
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ISIG;

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); //Check
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR;

	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	// Save tty settings, also checking for error
	if (tcsetattr(this->Serial_Port, TCSANOW, &tty) != 0) {
	    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	    return 1;
	}

	this->tty = tty;
	return 0;
}

void SerialConnection::Close(){
	close(this->Serial_Port);
}

//Write Read Function, Returns amount of 8-bit Chars received
int SerialConnection::WriteRead(unsigned char msg){
	write(this->Serial_Port, &msg, 1);

	memset(&this->read_buf, '\0', sizeof(this->read_buf));
	int n = read(this->Serial_Port, &this->read_buf, sizeof(this->read_buf));

	return n;
}

int SerialConnection::CheckOK(int n){
	//Error Testing
	if(n != 1){
		return 1;
	}

	if(this->read_buf[0] != 100){
		return 1;
	}

	return 0;
}

int SerialConnection::Reset(){
	int n = this->WriteRead('R');
	return this->CheckOK(n);
}

int SerialConnection::HomingPan(){
	int n = this->WriteRead('P');
	return this->CheckOK(n);
}

int SerialConnection::GetPan(){
	this->WriteRead('z');
	return atoi(this->read_buf);
}

int SerialConnection::SetPan(int count){
	unsigned char msg[sizeof(int)];
	memcpy(&msg, &count, sizeof count);

	write(this->Serial_Port, &msg, sizeof(msg));

	memset(&this->read_buf, '\0', sizeof(this->read_buf));
	int n = read(this->Serial_Port, &this->read_buf, sizeof(this->read_buf));

	return this->CheckOK(n);
}

int SerialConnection::MinCountPan(){
	this->WriteRead('q');
	return atoi(this->read_buf);
}

int SerialConnection::MaxCountPan(){
	this->WriteRead('a');
	return atoi(this->read_buf);
}







