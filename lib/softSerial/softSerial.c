#include "softSerial.h"

char readc();

char *buffer;
void initSoftSerial(void)
{
	buffer = (char *) malloc(SOFT_BUFFER_SIZE); // must be cleared by user

	DEBUG_SS_DDR |= _BV(DEBUG_SS_P);
	DEBUG_SS_DDR &= ~_BV(UART_Rx);
}

char *readSoftString(void)
{
	char *ptr = buffer;
	char value;
	for(int i = 0; i < SOFT_BUFFER_SIZE; i++)
	{
		value = readSoftChar();
		*ptr++ = value;
		if (!value)
			break;
	}
	return buffer;
}

char readSoftChar(void)
{
	char read;
	cli();
	read = readc();
	sei();
	return read;
}



// void printc(char ch) {
// 	uint8_t txdelay = TXDELAY;
// 	uint8_t delayCount = 0;

// 	__asm__ __volatile__(
// 		"0: cli\n\t"
// 		"   sbi %[uart_port]-1,%[uart_tx]\n\t"
// 		"   cbi %[uart_port],%[uart_tx]\n\t"
// 		"   in __tmp_reg__,%[uart_port]\n\t"
// 		"   ldi r25,3\n\t"
// 		"1: mov %[delayCount],%[txdelay]\n\t"
// 		"2: dec %[delayCount]\n\t"
// 		"   brne 2b\n\t"
// 		"   bst %[ch],0\n\t"
// 		"   bld __tmp_reg__,%[uart_tx]\n\t"
// 		"   lsr r25\n\t"
// 		"   ror %[ch]\n\t"
// 		"   out %[uart_port],__tmp_reg__\n\t"
// 		"   brne 1b\n\t"
// 		:
// 		  [ch] "+r" (ch),
// 		  [delayCount] "+r" (delayCount)
// 		:
// 		  [uart_port] "M" (UART_Port),
// 		  [uart_tx] "M" (UART_Tx),
// 		  [txdelay] "r" (txdelay)
// 		:
// 		  "r25"
// 	);
// }

// void println(const char *string) {
// 	while(*string) {
// 		printc(*string++);
// 	}
// 	printc('\n');
// 	printc('\r');
// }

void sputchar(uint8_t c)
{
	// if (!debugEnabled) return;

	cli(); 

	c = ~c;
	DEBUG_SS_PORT &= ~(1 << DEBUG_SS_P);            // start bit
	for (uint8_t i = 10; i; i--)
	{        // 10 bits
		_delay_us(1000000UL / DEBUG_SS_BAUD);            // bit duration
		if (c & 1)
			DEBUG_SS_PORT &= ~(1 << DEBUG_SS_P);        // data bit 0
		else
			DEBUG_SS_PORT |= 1 << DEBUG_SS_P;          // data bit 1 or stop bit
		c >>= 1;
	}

	sei();
}

char readc() {
	uint8_t rxdelay = RXDELAY;
	uint8_t rxdelay15 = (RXDELAY*1.5)-2.5;
	uint8_t delayCount = 0;

	__asm__ __volatile__(
		"0: cbi %[uart_port]-1,%[uart_rx]\n\t"
		"   sbi %[uart_port],%[uart_rx]\n\t"
		"   mov %[delayCount],%[rxdelay15]\n\t"
		"   ldi %[rxdelay15],0x80\n\t"
		"1: sbic %[uart_port]-2,%[uart_rx]\n\t"
		"   rjmp 1b\n\t"
		"   cli\n\t"
		"2: subi %[delayCount], 1\n\t"
		"   brne 2b\n\t"
		"   mov %[delayCount],%[rxdelay]\n\t"
		"   sbic %[uart_port]-2,%[uart_rx]\n\t"
		"   sec\n\t"
		"   ror %[rxdelay15]\n\t"
		"   brcc 2b\n\t"
		"3: dec %[delayCount]\n\t"
		"   brne 3b\n\t"
	:
	  [rxdelay15] "+r" (rxdelay15),
	  [delayCount] "+r" (delayCount)
	:
	  [uart_port] "M" (UART_Port),
	  [uart_rx] "M" (UART_Rx),
	  [rxdelay] "r" (rxdelay)
	);
	return (char) rxdelay15;
}