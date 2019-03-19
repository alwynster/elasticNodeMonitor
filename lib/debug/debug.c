#include "debug.h"

void (*writeCharMock) (char) = usbSendChar;
void debugWriteBin(uint32_t num, uint8_t length);

void debugSwitchUSB(void)
{
	writeCharMock = usbSendChar;
}

void debugSwitchSoftUart(void)
{
	writeCharMock = softuart_putchar_blocking;
}

void debugWriteLine(char *str)
{
	debugWriteString(str);
	debugWriteString(" \r\n");
}

void debugWriteString(char *str)
{
	while (*str)
		debugWriteChar(*str++);
	// usbSendString(str);
}

void debugNewLine(void)
{
	debugWriteString(" \r\n");

	// CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	// USB_USBTask();
}

void debugWriteStringLength(char *s, uint8_t length)
{
	uint8_t i;
	for (i = 0; i < length; i++)
	// while (*s)
		debugWriteChar(*s++);
}

void debugWriteChar(uint8_t c)
{
	writeCharMock(c);
}

void debugWriteHex32(uint32_t num)
{
	char buf[10];
	sprintf(buf, "%08lX", num);
	debugWriteString(buf);
}

void debugWriteBin32(uint32_t num)
{
	debugWriteBin(num, 32);
}

void debugWriteBin8(uint8_t num)
{
	debugWriteBin((uint32_t) num, 8);
}

void debugWriteBin16(uint16_t num)
{
	debugWriteBin((uint32_t) num, 16);
}

void debugWriteBin4(uint8_t num)
{
	debugWriteBin((uint32_t) num, 4);
}

void debugWriteBin(uint32_t num, uint8_t length)
{
	debugWriteString("0b");
	int i;
	uint32_t number = num;
	for(i = length-1; i >= 0; i--)
	{
		if (number & _BV(i))
			debugWriteChar('1');
		else
			debugWriteChar('0');
		// number >>= 1;
	}
}

void debugWriteHex8(uint8_t num)
{
	char buf[10];
	sprintf(buf, "%02X", num);
	debugWriteString(buf);
}

void debugWriteHex16(uint16_t num)
{
	char buf[10];
	sprintf(buf, "%04X", num);
	debugWriteString(buf);
}

void debugWriteDec8(uint8_t num)
{
	char buf[10];
	sprintf(buf, "%2d", num);
	debugWriteString(buf);
}

void debugWriteFloat(float num)
{
	char buf[10];
	sprintf(buf, "%.2f", num);
	debugWriteString(buf);
}

void debugWriteDec16(uint16_t num)
{
	char buf[10];
	sprintf(buf, "%u", num);
	debugWriteString(buf);
}

void debugWriteDec32(uint32_t num)
{
	char buf[10];
	sprintf(buf, "%lu", num);
	debugWriteString(buf);
}