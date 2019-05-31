/*****
 * Software Half-duplex UART Driver
 * Henry Chan (hc352 @cornell.edu)
 * Based on AVR304
 *
 * Sample Usage:
 * RECEIVING
 * sw_uart_receive_byte();
 * // Other instructions while receiving
 * if (sw_uart_state == IDLE) {
 *    x = sw_uart_rxdata;
 * }
 *
 * TRANSMITTING
 * sw_uart_send_byte(c);
 * // Other instructions while transmitting
 * if (sw_uart_state == IDLE) {
 *    //Stuff when transmission done
 * }
 ****/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib/LUFA-Setup/Helpers.h"
#include "lib/debug/debug.h"

void debugWriteBin(uint32_t num, uint8_t length);

uint8_t debugEnabled = 0x01;
volatile uint8_t uartData;

void debugDisable(void) 
{
	// debugWriteLine("Debug disabled");
	debugEnabled = 0x0; 
}
void debugEnable(void) 
{
	debugEnabled = 0x1; 
	// debugWriteLine("Debug enabled");
}

void debugInit(void (*receiveHandler)(uint8_t))
{
	setUpUsbSerial();
	debugWriteString("\r\n\n\n\nStarting debug...");
	debugNewLine();
}

void setDebugReceiveHandler(void (*receiveHandler)(uint8_t))
{
	// usb not using interrupts
	// setUpUsbSerial();
}


// ignore extra parameter used for flash readback
void debugWriteCharHelper(uint8_t c, uint8_t last)
{
	debugWriteChar(c);
}


void debugNewLine(void)
{
	debugWriteChar('\r');
	debugWriteChar('\n');
}

void debugWriteLine(char *s)
{
	debugWriteString(s);
	debugWriteChar('\n');
}

void debugWriteString(char *s)
{
	debug(s);
}

void debugWriteStringLength(char *s, uint8_t length)
{
	debugSized(s, length);
}

void debugWriteChar(uint8_t c)
{
	writeByte(c);
}


void debugWriteCharBlock(uint8_t c)
{
	writeByte(c);
}

void debugReadChar(void)
{
	uartData = readByte();
}

uint8_t debugReadCharAvailable(void)
{
	return uartData != 0xff;
}

void debugReadCharProcessed(void)
{
	// not needed for usb
}

uint8_t debugGetChar(void)
{
	return uartData;
}
		

void debugWriteBool(uint8_t input)
{
	if(input)
		debugWriteString("true");
	else
		debugWriteString("false");
}

void debugWaitUntilDone(void)
{
	#warning "lufa not waiting for finished"
}

uint8_t debugSending(void)
{
	#warning "lufa not waiting for finished"
}


void debugAck(uint8_t c)
{
	debugWriteChar(c);
	// uartWriteCharBlock(c);
}

void debugWriteHex32(uint32_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%08lX", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteBin32(uint32_t num)
{
	debugWriteBin(num, 32);
}

void debugWriteBin8(uint8_t num)
{
	debugWriteBin((uint32_t) num, 8);
}

void debugWriteBin4(uint8_t num)
{
	debugWriteBin((uint32_t) num, 4);
}

void debugWriteBin(uint32_t num, uint8_t length)
{
	debugWriteString("0b");
	uint32_t i = (uint32_t) 1 << (length - 1);
	uint32_t number = num;
	for(; i; i >>= 1)
	{
		if (number & i)
			debugWriteCharBlock('1');
		else
			debugWriteCharBlock('0');
		// number >>= 1;
	}
}

void debugWriteHex8(uint8_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%02X", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteHex8Helper(uint8_t num, uint8_t last)
{
	debugWriteHex8(num);
}

void debugWriteHex16(uint16_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%04X", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteDec8(uint8_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%2d", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteFloat(float num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%.2f", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteFloatFull(float num)
{
	char *buf = (char *) malloc(100);
	sprintf(buf, "%f", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteDec16(uint16_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%u", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteDec32(uint32_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%lu", num);
	debugWriteString(buf);
	free(buf);
}

void debugWriteDec32S(int32_t num)
{
	char *buf = (char *) malloc(10);
	sprintf(buf, "%ld", num);
	debugWriteString(buf);
	free(buf);
}

void debugDone(void)
{
	debugWriteLine("\n$$");
}

void debugReady(void)
{
	debugWriteLine("\n%%");
}

// void printUartHandler(void)
// {
// 	debugWriteString("Uart Handler: ");
// // 	debugWriteHex16((uint16_t) (sendingBuf.head));
// // 	debugWriteChar(' ');
// // 	debugWriteHex16((uint16_t) (sendingBuf.last));
// // 	debugWriteChar(' ');

// 	debugWriteHex32((uint16_t) (getUartReceiveHandler()));
// 	debugWriteChar(' ');
// 	debugWriteHex8(UCSR1B);
// 	debugWriteChar(' ');
// 	debugWriteDec8((SREG & _BV(7)) != 0);
// 	debugNewLine();
	

// // 	debugWriteHex32((uint32_t) (&uartReceiveHandler));
// // 	debugWriteChar(' ');

// // 	return 0;
// // 	// return (uint16_t) uartReceiveHandler;
// }