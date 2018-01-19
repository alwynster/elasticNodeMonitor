#ifndef CURRENT_MEASUREMENT_H
#define CURRENT_MEASUREMENT_H

#include <inttypes.h>
#include <circular_buffer/circular_buffer.h>

typedef union currentMeasurementTmp {
	uint16_t measurements[6];
	struct {
		uint16_t usb;
		uint16_t monitor;
		uint16_t wireless;
		uint16_t mcu;
		uint16_t vccaux;
		uint16_t vccint;
	} breakdown;
} currentMeasurement;

typedef union currentMeasurementFloatTmp {
	float measurements[6];
	struct {
		float usb;
		float monitor;
		float wireless;
		float mcu;
		float vccaux;
		float vccint;
	} breakdown;
} currentMeasurementFloat;


#endif