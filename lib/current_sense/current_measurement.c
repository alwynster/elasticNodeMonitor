
#include "lib/circular_buffer/circular_buffer.h"
#include "lib/current_sense/current_measurement.h"

uint8_t popCurrentMeasurement(circularBuffer *buf, currentMeasurementFloat *measurement)
{
	uint8_t *destPtr = (uint8_t *) measurement->measurements;

	// uint8_t gi = interruptStatus();
	
	// if (gi)
		cli();

	// pop enough bytes out of the circular buffer
	for(int s = 0; s < sizeof(currentMeasurementFloat); s++)
	{
		// pop out one byte and increase pointer 
		if (!circBufPop(buf, destPtr++))
		{
			// debugWriteLine("cannot pop measurement");
			// if (gi)
				sei();

			return 0;
		}
	}

	// debugWriteLine("popped measurement");
	// if (gi) 
		sei();

	// if end is reached then it's fine
	return 1;
}


currentMeasurementFloat measurementFloat;
uint8_t pushCurrentMeasurement(circularBuffer *buf, currentMeasurement *measurement)
{
	// convert to floats 
	currentMeasurementConvert(measurement, &measurementFloat);
	uint8_t *destPtr = (uint8_t *) measurementFloat.measurements;

	// pop enough bytes out of the circular buffer
	for(int s = 0; s < sizeof(currentMeasurementFloat); s++)
	{
		// push one byte at a time

		circBufPush(buf, *destPtr++);
	}

	// if end is reached then it's fine
	return 1;
}