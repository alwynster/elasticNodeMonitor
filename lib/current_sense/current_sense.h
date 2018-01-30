#ifndef CURRENT_SENSE_H
#define CURRENT_SENSE_H

// #include "lib/twi/twi.h"
#include <i2cmaster/i2cmaster.h>
// #include <softuart/softuart.h>
// #include <uart/uart.h>
#include <current_sense/current_measurement.h>
#include <circular_buffer/circular_buffer.h>
//#include "lib/i2cmaster/i2cmaster.h"
//#include "lib/softuart/softuart.h"
//#include "lib/uart/uart.h"
//#include "lib/current_sense/current_measurement.h"
//#include "lib/circular_buffer/circular_buffer.h"
#include <compat/twi.h>

// counter for monitoring task
#define CURRENT_SENSE_F 12
#define CURRENT_SENSE_OCR (F_CPU / 1024 / CURRENT_SENSE_F - 1)

#define MEASUREMENT_BUFFER_SIZE 25
// #if CURRENT_SENSE_OCR > 0xffff
// 	#warning "OCR is too large"
// #endif


// #include "lib/twi_master/twi_master.h"

// enumerate different current sensors (specific channels)
#define CURRENT_UNKNOWN 0
#define CURRENT_USB 1
#define CURRENT_WIRELESS 2
#define CURRENT_MCU 3
#define CURRENT_FPGA_VCCAUX 4
#define CURRENT_FPGA_VCCINT 5
#define CURRENT_MONITOR 6

// dictate where to find every channel
#define CURRENT_USB_ADDRESS 0b01001101
#define CURRENT_USB_CHANNEL 1
#define CURRENT_USB_R 0.15f
#define CURRENT_MONITOR_ADDRESS 0b01001101
#define CURRENT_MONITOR_CHANNEL 2
#define CURRENT_MONITOR_R 0.8f

#define CURRENT_WIRELESS_ADDRESS 0b00101001
#define CURRENT_WIRELESS_CHANNEL 1
#define CURRENT_WIRELESS_R 0.8f
#define CURRENT_MCU_ADDRESS 0b00101001
#define CURRENT_MCU_CHANNEL 2
#define CURRENT_MCU_R 0.8f

#define CURRENT_FPGA_VCCAUX_ADDRESS 0b01001111
#define CURRENT_FPGA_VCCAUX_CHANNEL 1
#define CURRENT_FPGA_VCCAUX_R 0.8f
#define CURRENT_FPGA_VCCINT_ADDRESS 0b01001111
#define CURRENT_FPGA_VCCINT_CHANNEL 2
#define CURRENT_FPGA_VCCINT_R 0.8f

// REGISTERS
// #define CURRENT_ADDRESS_A 0b00101001 // CHANNEL 1 nothing CHANNEL 2 MCU
// #define CURRENT_ADDRESS_B 0b01001101
#define CURRENT_REG_PRODUCT_ID 0xFD
#define CURRENT_REG_CHANNEL_1_CONFIG 0x0B
#define CURRENT_REG_CHANNEL_2_CONFIG 0x0C
#define CURRENT_REG_CHANNEL_1_HIGH 0x0D
#define CURRENT_REG_CHANNEL_1_LOW 0x0E

#define CURRENT_REG_CHANNEL_2_HIGH 0x0F
#define CURRENT_REG_CHANNEL_2_LOW 0x10

#define FSR 0.08f
#define DEN 2047.0f
// nothing, sampling x3, average x2, range x2
// #define CURRENT_CONFIG 0b01101011
#define CURRENT_RATE_CONFIG 0b11 		// Continuous conversion
#define CURRENT_AVERAGING_CONFIG 0b00 	// No averaging
#define CURRENT_SAMPLING_CONFIG 0b101 	// Most accurate measurements 80ms
#define CURRENT_RANGE_CONFIG 0b11 		// Full swing +-80mV
#define CURRENT_CONFIG ((CURRENT_SAMPLING_CONFIG << 5) | (CURRENT_AVERAGING_CONFIG << 2) | (CURRENT_RANGE_CONFIG))
// #define CURRENT_CONFIG 0b01011111
// #define CURRENT_ADDRESS_A 0b00101001 // CHANNEL 1 nothing CHANNEL 2 MCU

// FSC = FSR/Rsense = 0.08/Rsense = 0.1
	// I = FSC * Vsense/Den= 0.1 * sense / 2047


void initCurrentSense(void);
void currentSensePushMeasurements(uint8_t push);
void findCurrentSensors(void);
uint8_t currentSenseCount(void);
void currentSenseBegin(void);
void currentSenseEnd(void);
uint8_t currentSenseTask(void);
uint8_t popCurrentMeasurement(currentMeasurementFloat *measurement);

void currentSenseClearMeasurements(void);

void printCurrentProductId(uint8_t address);
float currentSenseConvert(uint8_t device, uint16_t sense);
void currentMeasurementConvert(currentMeasurement *measurement, currentMeasurementFloat *converted);
void printCurrent(uint8_t device);
// extern union TWI_statusReg_f TWI_statusReg;

// void printCurrentProductId(uint8_t address);
uint16_t readCurrentSense(uint8_t address, uint8_t channel);
uint16_t readCurrentSenseBlocking(uint8_t address, uint8_t channel);
// float readCurrent(uint8_t address, uint8_t channel);
void printCurrentSense(uint8_t address, uint8_t channel);

float fetchCurrent(uint8_t device);
void printCurrentMeasurement(currentMeasurement c);
void printAllCurrentMeasurementsFloat(void);

void currentSenseStartAll(void);

#endif
