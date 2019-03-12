#ifndef GPIO_H
#define GPIO_H

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <debug.h>

#define MONITOR_DDR DDRE
#define MONITOR_PORT PORTE
#define MONITOR_PIN PINE
#define MONITOR_P PE6

void initMonitorPin(void);
uint8_t getMonitorPin(void);
uint8_t getMonitorStatus(void);

#endif