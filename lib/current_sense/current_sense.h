#ifndef CURRENT_SENSE_H
#define CURRENT_SENSE_H


#include "lib/current_sense/current_measurement.h"

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

// counter for monitoring task
#define CURRENT_SENSE_PERIOD 160
#define CURRENT_SENSE_F (1000.0f / CURRENT_SENSE_PERIOD)
#define CURRENT_SENSE_OCR (uint16_t) ((float) (((F_CPU / 1024)) / CURRENT_SENSE_F) - 1)

#define FSR 0.08f
#define SENSE_BITS 12
#define DEN ((float)((1 << (SENSE_BITS - 1)) - 1)) //  2047.0f
#define SENSE_SHIFT_DOWN (16 - SENSE_BITS)
#define SENSE_SHIFT_UP (SENSE_BITS - 8)
// nothing, sampling x3, average x2, range x2
// #define CURRENT_CONFIG 0b01101011
#define CURRENT_RATE_CONFIG 0b11 		// Continuous conversion
#define CURRENT_AVERAGING_CONFIG 0b00 	// No averaging
#define CURRENT_SAMPLING_CONFIG 0b101 	// Measurement accuracy :
// 80 	101 2047	12
// 40	100 1023	11
// 20	011 511		10
// 10 	010 255		9
// 5	001 127		8
// 2.5 	000 63		7
#define CURRENT_RANGE_CONFIG 0b11 		// Full swing +-80mV
#define CURRENT_CONFIG ((CURRENT_SAMPLING_CONFIG << 4) | (CURRENT_AVERAGING_CONFIG << 2) | (CURRENT_RANGE_CONFIG))
// #define CURRENT_CONFIG 0b01011111
// #define CURRENT_ADDRESS_A 0b00101001 // CHANNEL 1 nothing CHANNEL 2 MCU

// FSC = FSR/Rsense = 0.08/Rsense = 0.1
	// I = FSC * Vsense/Den= 0.1 * sense / 2047


void initCurrentSense(void);
void currentSenseConfig(void);
void currentSensePushMeasurements(uint8_t push);
void findCurrentSensors(void);
uint8_t currentSenseCount(void);
void currentSenseBegin(void);
void currentSenseEnd(void);
uint8_t currentSenseTask(void);

uint8_t getAddress(uint8_t device);
void writeRegister(uint8_t address, uint8_t register_address, uint8_t value);

void currentSenseClearMeasurements(void);

void printAccID(void);
void printCurrentProductId(uint8_t address);
float currentSenseConvert(uint8_t device, uint16_t sense);
void printCurrent(uint8_t device);
void printAccelerometerValues(void);
// extern union TWI_statusReg_f TWI_statusReg;

// void printCurrentProductId(uint8_t address);
uint16_t readCurrentSense(uint8_t address, uint8_t channel);
uint16_t readCurrentSenseBlocking(uint8_t address, uint8_t channel);
// float readCurrent(uint8_t address, uint8_t channel);
void printCurrentSense(uint8_t address, uint8_t channel);
void printCurrentMeasurement(currentMeasurement c);
void printCurrentMeasurementFloat(currentMeasurementFloat c);
void printAllCurrentMeasurementsFloat(void);

float fetchCurrent(uint8_t device);
currentMeasurementFloat fetchCurrentMeasurementFloat(void);

void currentSenseStartAll(void);

#endif
