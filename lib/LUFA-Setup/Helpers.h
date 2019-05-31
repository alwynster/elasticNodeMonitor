#ifndef COMMUNICATIONMODULE_HELPERS_H
#define COMMUNICATIONMODULE_HELPERS_H

// #include "Descriptors.h"
// #include "LUFAConfig.h"
// #include "LUFA/Drivers/Peripheral/Serial.h"
// #include <avr/power.h>
// #include <avr/io.h>
// #include <avr/wdt.h>
// #include <avr/interrupt.h>

// also enables global interrupts
void setUpUsbSerial(void);

void writeByte(uint8_t);
uint8_t readByte(void);
void debug(const char *data);
void debugSized(const char *data, uint16_t size);

// needs to be called periodically in short intervals from the main method
void periodicUsbTask(void);

#endif //COMMUNICATIONMODULE_HELPERS_H
