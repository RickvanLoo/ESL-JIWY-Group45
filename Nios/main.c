/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */


/*
 * INA => PWM_BUS(2),INB => PWM_BUS(1), PWM => PWM_BUS(0));
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


#include <io.h>
#include "system.h"
#include "sys/alt_irq.h"

#include <math.h>


//ADDRESSSES
#define ADDR_PWM_DIV 0x02
#define ADDR_PWM_DIV1 0x03
#define ADDR_PWM_DIR 0x04
#define ADDR_PWM_DIR1 0x05
#define ADDR_RST 0x01
#define ADDR_COUNT 0x20
#define ADDR_COUNT1 0x21

#define MAX_PWM 2500

//Functions
#define MinCntP 'q'
#define MaxCntP 'a'
#define GetCntP 'z'
#define SetCntP 'x'

#define MinCntT 'Q'
#define MaxCntT 'A'
#define GetCntT 'Z'
#define SetCntT 'X'

#define FuncRst 'R'
#define HomP 'P'
#define HomT 'T'

#define ACTIVERESET 1

float SAMPLETIME = 1/1000; //HAVE TO EDIT


//HOMING
int MaxCount[2] = {0,0};
int MinCount[2] = {0,0};
int DefaultCount[2] = {0,0};
int HomingFinished[2] = {0,0};

int Val_Pan[3] = {0, 0, 0};
float Gain_Pan[4] = {2.6, 9, 0.05, 0.17};

int Val_Tilt[3] = {0, 0, 0};
float Gain_Tilt[4] = {1.6, 10.5, 0.05, 0.001};

int Target[2] = {0,0};

//SERIAL
enum SerialState {
	FUNC,
	CHAR0,
	CHAR1,
	CHAR2,
	CHAR3
};

char State = FUNC;
char CurrentFunc = 0;
unsigned char RXBuff[4] = {0,0,0,0};
unsigned char TXBuff[4] = {0,0,0,0};
char OK_SYM = 100;
char NOT_OK_SYM = 99;



void SoftReset(){
	printf("Soft Reset System!\n");
	IOWR(QUADRATUREPWM_ESL_0_BASE, ADDR_RST, ACTIVERESET);
	usleep(5000);
	IOWR(QUADRATUREPWM_ESL_0_BASE, ADDR_RST, !ACTIVERESET);

	IOWR(TIMER_0_BASE, 1, 0);
	IOWR(TIMER_1_BASE, 1, 0);

	memset(MaxCount, 0, sizeof MaxCount);
	memset(MinCount, 0, sizeof MinCount);
	memset(DefaultCount, 0, sizeof DefaultCount);
	memset(HomingFinished, 0, sizeof HomingFinished);
	memset(Target, 0, sizeof Target);

	 SetPWM(1,0,0);
	 SetPWM(0,0,0);
}


//PAN IS 1
void SetPWM(bool pan, int div, bool dir){
	int addr_div = 01;
	int addr_dir = 0;

	if(pan){
		addr_div = ADDR_PWM_DIV;
		addr_dir = ADDR_PWM_DIR;
	}else{
		addr_div = ADDR_PWM_DIV1;
		addr_dir = ADDR_PWM_DIR1;
	}

	IOWR(QUADRATUREPWM_ESL_0_BASE, addr_div, div); //Set DIV
	usleep(5000);

	if(dir){
		IOWR(QUADRATUREPWM_ESL_0_BASE, addr_dir, 1); //Set DIR
	}else{
		IOWR(QUADRATUREPWM_ESL_0_BASE, addr_dir, 0);
	}
}

void Homing(int pan){
	SetPWM(pan, 0, 1);

	printf("Start Homing %d\n",pan);
	int FirstCount = GetCounter(pan);
	int SecondCount = GetCounter(pan)+10;

	int Speed = 0;
	int Sleep = 300;
	if(pan){
		Speed = 1000;
	}else{
		Speed = 1200;
	}


	//Direction 1
	while(FirstCount != SecondCount){
		SetPWM(pan, Speed, 1);
		FirstCount = SecondCount;
		SecondCount = GetCounter(pan);
		MaxCount[pan] = SecondCount;
		usleep(Sleep);
	}

	SetPWM(pan,0,1);

	FirstCount = GetCounter(pan);
	SecondCount = GetCounter(pan)+1;

	//Direction 0
	while(FirstCount != SecondCount){
		SetPWM(pan, Speed, 0);
		FirstCount = SecondCount;
		SecondCount = GetCounter(pan);
		MinCount[pan] = SecondCount;
		usleep(Sleep);
	}

	SetPWM(pan,0,0);

	DefaultCount[pan] = (MaxCount[pan] + MinCount[pan]) / 2;
	HomingFinished[pan] = 1;
	Target[pan] = DefaultCount[pan];

	printf("Max(%d):%d\n", pan, MaxCount[pan]);
	printf("Min(%d):%d\n", pan, MinCount[pan]);
	printf("Default(%d):%d\n",pan, DefaultCount[pan]);
}


//PAN IS 1
int GetCounter(int pan){
	if(pan){
		return IORD(QUADRATUREPWM_ESL_0_BASE, ADDR_COUNT);
	}else{
		return IORD(QUADRATUREPWM_ESL_0_BASE, ADDR_COUNT1);
	}
}


//Interrupt PID PAN Function
void PID_Pan(void* context){
	//Val[0] -- LastIntegral
	//Val[1] -- Last_Error
	//Val[2] -- LastDerivative
	//Gain[0] -- KP
	//Gain[1] -- tauL
	//Gain[2] -- tauD
	//Gain[3] -- Beta

	int CurrPos = GetCounter(1);

	int Error = Target[1] - CurrPos;

	float factor = 1 / ( SAMPLETIME + Gain_Pan[2] * Gain_Pan[3] );
	float uD = factor * ( Gain_Pan[2]  * Val_Pan[2] * Gain_Pan[3] + Gain_Pan[2]  * Gain_Pan[0] * ( Error - Val_Pan[1]) + SAMPLETIME * Gain_Pan[0] * Error );
	float uI = Val_Pan[0] +  SAMPLETIME * uD / Gain_Pan[1];

	int Output = uI + uD;

	//Bound PWM
	if (Output > MAX_PWM) Output = MAX_PWM;
	if (Output < -MAX_PWM) Output = -MAX_PWM;

	//Setup Direction
	if (Output > 0) SetPWM(1, Output, 1); //CORRECT
	if (Output < 0) SetPWM(1, -Output, 0);

	Val_Pan[0] = uI;
	Val_Pan[1] = Error;
	Val_Pan[2] = uD;
}


//Interrupt PID PAN Function
void PID_Tilt(void* context){
	//Val[0] -- LastIntegral
	//Val[1] -- Last_Error
	//Val[2] -- LastDerivative
	//Gain[0] -- KP
	//Gain[1] -- tauL
	//Gain[2] -- tauD
	//Gain[3] -- Beta

	int CurrPos = GetCounter(0);

	int Error = Target[0] - CurrPos;

	float factor = 1 / ( SAMPLETIME + Gain_Tilt[2] * Gain_Tilt[3] );
	float uD = factor * ( Gain_Tilt[2]  * Val_Tilt[2] * Gain_Tilt[3] + Gain_Tilt[2]  * Gain_Tilt[0] * ( Error - Val_Tilt[1]) + SAMPLETIME * Gain_Tilt[0] * Error );
	float uI = Val_Tilt[0] +  SAMPLETIME * uD / Gain_Tilt[1];

	int Output = uI + uD;

	//Bound PWM
	if (Output > MAX_PWM) Output = MAX_PWM;
	if (Output < -MAX_PWM) Output = -MAX_PWM;

	//Setup Direction
	if (Output > 0) SetPWM(0, Output, 1); //CORRECT
	if (Output < 0) SetPWM(0, -Output, 0);

	Val_Tilt[0] = uI;
	Val_Tilt[1] = Error;
	Val_Tilt[2] = uD;
}


//Takes unsigned 32-bit int and transforms it into 4 bytes
void setTXArray(unsigned int Data32){
	TXBuff[3] = (Data32 >> 0) & 0xff;
	TXBuff[2] = (Data32 >> 8) & 0xff;
	TXBuff[1] = (Data32 >> 16) & 0xff;
	TXBuff[0] = (Data32 >> 24) & 0xff;
}

//Retrieves 4-byte length array and returns unsigned 32-bit int
unsigned int getRXArray(){
	unsigned int OUT = 0;
	OUT |= (unsigned int)RXBuff[0] << 24;
	OUT |= (unsigned int)RXBuff[1] << 16;
	OUT |= (unsigned int)RXBuff[2] << 8;
	OUT |= (unsigned int)RXBuff[3];
	return OUT;
}


//Function takes CHAR stage, handles FUNC and outputs TX
unsigned char HandleFunc(int c){
	int Data32 = 0;

	switch(CurrentFunc){
		//Simple Functions
	case FuncRst:
		if(c == 0){
			SoftReset();
		}
		return OK_SYM;
		break;
	case HomP:
		if(c == 0){
			Homing(1);
		}
		return OK_SYM;
		break;
	case HomT:
		if(c == 0){
			Homing(0);
		}
		return OK_SYM;
		break;

		//Set Functions
	case SetCntP:
		if(c == 3){
			Target[1] = (int)getRXArray;
		}
		return OK_SYM;
		break;
	case SetCntT:
		if(c == 3){
			Target[0] = (int)getRXArray;
		}
		return OK_SYM;
		break;

		//Get Functions
	case GetCntP:
		if(c == 0){
			Data32 = GetCounter(1);
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;
	case GetCntT:
		if(c == 0){
			Data32 = GetCounter(0);
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;

		//MinMax
	case MinCntP:
		if(c == 0){
			Data32 = MinCount[1];
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;
	case MaxCntP:
		if(c == 0){
			Data32 = MaxCount[1];
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;
	case MinCntT:
		if(c == 0){
			Data32 = MinCount[0];
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;
	case MaxCntT:
		if(c == 0){
			Data32 = MaxCount[0];
			setTXArray(Data32);
		}
		return TXBuff[c];
		break;
	default:
		break;
	}

	return NOT_OK_SYM;
}

void SerialReceive(void* context){
	unsigned char RX = IORD(UART_0_BASE, 0);
	unsigned char TX = 0;

	switch(State){
	case FUNC:
		CurrentFunc = RX;
		TX = OK_SYM;
		State = CHAR0;
		break;
	case CHAR0:
		RXBuff[0] = RX;
		TX = HandleFunc(0);
		State = CHAR1;
		break;
	case CHAR1:
		RXBuff[1] = RX;
		TX = HandleFunc(1);
		State = CHAR2;
		break;
	case CHAR2:
		RXBuff[2] = RX;
		TX = HandleFunc(2);
		State = CHAR3;
		break;
	case CHAR3:
		RXBuff[3] = RX;
		TX = HandleFunc(3);
		State = FUNC;
		break;
	default:
		break;
	}

	IOWR(UART_0_BASE, 1, TX);


	return;
}






int main()
{

  printf("Hello from Nios II!\n");
  SoftReset();
  alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, PID_Pan, NULL, NULL);
  //alt_ic_isr_register(TIMER_1_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_1_IRQ, PID_Tilt, NULL, NULL);
  alt_ic_isr_register(UART_0_IRQ_INTERRUPT_CONTROLLER_ID, UART_0_IRQ, SerialReceive, NULL, NULL);


  usleep(5000);

  Homing(1);

  usleep(5000);

  //Homing(0);

  //usleep(5000);

  printf("START PID\n");

  IOWR(TIMER_0_BASE, 1, 7);
  //IOWR(TIMER_1_BASE, 1, 7);



  return 0;
}
