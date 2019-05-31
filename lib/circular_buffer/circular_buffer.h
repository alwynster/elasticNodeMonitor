#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

typedef struct {
    uint8_t *buffer;
    uint8_t *head;
    uint8_t *tail;
    uint8_t *last;
    uint16_t currentLen;
    uint16_t maxLen;
} circularBuffer;

void circBufInit(circularBuffer *c, uint16_t size);
void circBufFree(circularBuffer *c);

uint16_t circBufSpace(circularBuffer *c);
uint16_t circBufCount(circularBuffer *c);
uint16_t circBufCountObjects(circularBuffer *c, uint16_t size);

uint8_t circBufPush(circularBuffer *c, uint8_t data);
uint8_t circBufPop(circularBuffer *c, uint8_t *data);

#endif
