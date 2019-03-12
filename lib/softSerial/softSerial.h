#ifndef SOFT_SERIAL_H

#define SOFT_SERIAL_H

/* See LICENSE file for copyright and license details. */
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// sputc 
#define DEBUG_SS_DDR DDRB
#define DEBUG_SS_PORT PORTB
#define DEBUG_SS_P PB6
#define DEBUG_SS_BAUD 57600


//Set tx to PB2 / rx to PB1
#define UART_Port _SFR_IO_ADDR(DEBUG_SS_PORT)
#define UART_Tx 3
#define UART_Rx 1

//Define baudrate
#define BAUD_RATE 57600

//Calculate delays
#ifdef F_CPU
#define TXDELAY (((F_CPU/BAUD_RATE)-7 +1.5)/3)
#define RXDELAY (((F_CPU/BAUD_RATE)-5 +1.5)/3)
#else
  #error CPU frequency F_CPU undefined
#endif

#define SOFT_BUFFER_SIZE 20
void initSoftSerial(void);
char *readSoftString(void);
char readSoftChar(void);

#endif