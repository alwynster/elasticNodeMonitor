#ifndef _DEBUG_H
#define _DEBUG_H

// #include <main.h>
#include <inttypes.h>
#include <stdlib.h>
#include <usbInterface.h>

void debugWriteString(char *str);
void debugNewLine(void);
void debugWriteLine(char *str);

void debugWriteStringLength(char *s, uint8_t length);
void debugWriteChar(uint8_t c);
void debugWriteHex8(uint8_t num);
void debugWriteHex16(uint16_t num);
void debugWriteDec8(uint8_t num);
void debugWriteDec16(uint16_t num);
void debugWriteBin16(uint16_t num);
void debugWriteBin8(uint8_t num);
void debugWriteHex32(uint32_t num);
void debugWriteBin32(uint32_t num);
void debugWriteFloat(float num);
void debugWriteBin4(uint8_t num);
void debugWriteDec32(uint32_t num);

#endif