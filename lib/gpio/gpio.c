#include "gpio.h"

volatile uint8_t monitorStatus;

void initMonitorPin(void)
{
    MONITOR_DDR &= ~_BV(MONITOR_P);
    MONITOR_PORT |= _BV(MONITOR_P);

    monitorStatus = 0;

    EICRB |= _BV(ISC60);
    EIMSK |= _BV(INT6);
    sei();
}

uint8_t getMonitorPin(void)
{
    return 1;
    // uint8_t result = MONITOR_PIN | _BV(MONITOR_P);
    // return result != 0;
}

uint8_t getMonitorStatus(void)
{
    return monitorStatus;
}

ISR(INT6_vect)
{
    monitorStatus = getMonitorPin();
    // debugWriteString("monitor pin change");
}