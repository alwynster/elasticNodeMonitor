#ifndef LIB_DEBUG_H
#define LIB_DEBUG_H

#include <stdint.h>

#define DEBUG 0

void debugEnable(void);
void debugDisable(void);
void debugInit(void (*receiveHandler)(uint8_t));
void setDebugReceiveHandler(void (*receiveHandler)(uint8_t));

void debugNewLine(void);
void debugWriteBool(uint8_t input);
void debugWriteLine(char *s);
void debugWriteString(char *s);
void debugWriteStringLength(char *s, uint8_t length);
void debugWriteChar(uint8_t c);
void debugWriteCharBlock(uint8_t c);
void debugWriteCharHelper(uint8_t c, uint8_t last);

void debugReadChar(void);
uint8_t debugReadCharAvailable(void);
void debugReadCharProcessed(void);
uint8_t debugGetChar(void);

void debugWriteHex8(uint8_t num);
void debugWriteHex8Helper(uint8_t num, uint8_t last);
void debugWriteHex16(uint16_t num);
void debugWriteDec8(uint8_t num);
void debugWriteDec16(uint16_t num);
void debugWriteBin8(uint8_t num);
void debugWriteHex32(uint32_t num);
void debugWriteBin32(uint32_t num);
void debugWriteFloat(float num);
void debugWriteFloatFull(float num);
void debugWriteBin4(uint8_t num);
void debugWriteDec32(uint32_t num);
void debugWriteDec32S(int32_t num);

void debugDone(void);

void debugReady(void);

void debugWaitUntilDone(void);
uint8_t debugSending(void);

void debugAck(uint8_t c);

void printUartHandler(void);

#endif
