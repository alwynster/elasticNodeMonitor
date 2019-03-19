#include "current_sense.h"
// #include "../main.h"


void findCurrentSensors(void);
uint8_t getChannel(uint8_t device);
float getR(uint8_t device);
char *getName(uint8_t device);
uint16_t readCurrentSense(uint8_t address, uint8_t channel);
void printCurrentSense(uint8_t address, uint8_t channel);
void currentSenseConfigDevice(uint8_t device);

// CIRCBUF_DEF(monitorBuf, sizeof(currentMeasurement) * MEASUREMENT_BUFFER_SIZE + 1);
circularBuffer monitorBuf;
volatile uint8_t currentSenseFlag;
currentMeasurement measurement;
currentMeasurementFloat measurementFloat;
uint8_t measurementCounter;

// interrupt-based reading variables
volatile uint8_t currentDevice;
typedef enum {I2C_IDLE, I2C_ERROR, I2C_START, I2C_RESTART, I2C_ADDRESS, I2C_READDRESS, I2C_REGISTER, I2C_READING} i2c_state;
volatile i2c_state currentState;
volatile uint8_t currentByte = 0;

uint8_t *dummy;

// should measurements be pushed to uart immediately?
uint8_t pushMeasurements; 

void initCurrentSense(void)
{
    // unsigned char ret;
    circBufFree(&monitorBuf);

    circBufInit(&monitorBuf, sizeof(currentMeasurementFloat) * MEASUREMENT_BUFFER_SIZE);

    // debugWriteString("init twi\n");
	i2c_init();
	i2c_enable();
	currentSenseConfig();

	currentSenseFlag = 0x0;
	measurementCounter = 0;

	// setup timer 3 to monitor (but do not start)
	TCCR3B = _BV(WGM32);

	OCR3A = CURRENT_SENSE_OCR;

	TIMSK3 |= _BV(OCIE3A);

	currentState = I2C_IDLE;

	pushMeasurements = 0x00;

}

void currentSensePushMeasurements(uint8_t push) { pushMeasurements = push; }

uint8_t currentSenseCount()
{
	// return circBufCountCurrentMeasurement(&monitorBuf);
	return 0;
}

void currentSenseClearMeasurements()
{
	while (popCurrentMeasurement(&monitorBuf, &measurementFloat)) { }
}


// begin task to monitor current consumption
void currentSenseBegin()
{
	TCCR3B |= _BV(CS32) | _BV(CS30);
	// TCCR3B |= _BV(CS32);
	TCNT3 = 0;
	currentSenseFlag = 0x0;
}

// begin task to monitor current consumption
void currentSenseEnd()
{
	TCCR3B &= ~(_BV(CS32) | _BV(CS30));

	debugWriteLine("Finished with current monitoring");

	// // float printFloat;
	// // send collected values
	// while(circBufPopCurrentMeasurement(&monitorBuf, &measurement)) // circBufCountCurrentMeasurement(&monitorBuf) > 0)
	// {
	// 	printCurrentMeasurement(measurement);
	// }
	// debugNewLine();
	currentSenseFlag = 0x0;
}


// grab new measurement if it is time
uint8_t currentSenseTask(void)
{
	// check if timer has occurred
	if(currentSenseFlag)
	{
		// debugWriteString("space:");
		// debugWriteDec8(circBufSpace(&monitorBuf));
		// debugNewLine();
		// start measurement of all 
		if(circBufSpace(&monitorBuf) > 1)
		{
			currentSenseStartAll();
		}
		else
		{
			// setLed(2,1);

			// debugWriteLine("No space for measurement");
		}

		currentSenseFlag = 0x00;

		// measurement was due
		return 1;
	}

	// measurement not due
	return 0;
}

ISR(TIMER3_COMPA_vect)
{
	// debugWriteLine("TIMER3_COMPA_vect");

	currentSenseFlag = 0x1;
	currentSenseTask();
	// setLed(3, 1);

	// debugWriteLine("/TIMER3_COMPA_vect");
}

void currentSenseConfig(void)
{
	currentSenseConfigDevice(CURRENT_WIRELESS);
	currentSenseConfigDevice(CURRENT_USB);
	currentSenseConfigDevice(CURRENT_MONITOR);
	currentSenseConfigDevice(CURRENT_MCU);
	currentSenseConfigDevice(CURRENT_FPGA_VCCAUX);
	currentSenseConfigDevice(CURRENT_FPGA_VCCINT);
}

void currentSenseConfigDevice(uint8_t device)
{
	cli();

	uint8_t address = getAddress(device);
	uint8_t channel = getChannel(device);

	uint8_t address8 = address << 1;
	uint8_t result;
	uint8_t reg;

	result = i2c_start(address8);
	if (result)
	{
		debugWriteString("couldn't start i2c ");
		// debugWriteBin8(address8);
		debugNewLine();
	}
	else
	{
		switch(channel)
		{
			case 1:
				reg = CURRENT_REG_CHANNEL_1_CONFIG;
				break;
			case 2:
				reg = CURRENT_REG_CHANNEL_2_CONFIG;
				break;
			default:
				debugWriteLine("Unknown current channel...");
				reg = 0;
				break;
		}
		result = i2c_write(reg);
		if (result)
		{
			debugWriteLine("couldn't send register address");
		}
		else
		{
			result = i2c_write(CURRENT_CONFIG);
			if (result)
			{
				debugWriteLine("couldn't send register data");
			}	
		}
	}
	i2c_stop();

	sei();
}

uint8_t readRegister(uint8_t address)
{
	cli();

	// product id: 0xFD
	uint8_t address8 = 0b0101000 << 1;
	uint8_t result;

	result = i2c_start(address8);
	if (result)
	{
		debugWriteString("couldn't start i2c");
	}
	else
	{
		result = i2c_write(address);
		if (result)
		{
			debugWriteString("couldn't send register address");
		}
		else
		{
			result = i2c_rep_start(address8 | 1);
			if (result)
			{
				debugWriteString("couldn't repeat start");
			}
			else
			{
				result = i2c_readNak();
			}
		}
	}
	i2c_stop();

	sei();	

	return result;
}

void writeRegister(uint8_t address, uint8_t register_address, uint8_t value)
{
	cli();

	uint8_t address8 = address << 1;
	uint8_t result;
	// uint8_t reg;

	result = i2c_start(address8);
	if (result)
	{
		debugWriteString("couldn't start i2c ");
		// debugWriteBin8(address8);
		debugNewLine();
	}
	else
	{
		result = i2c_write(register_address);
		if (result)
		{
			debugWriteLine("couldn't send register address");
		}
		else
		{
			result = i2c_write(value);
			if (result)
			{
				debugWriteLine("couldn't send register data");
			}	
		}
	}
	i2c_stop();

	sei();
}

// print the product id for testing
void printAccID(void)
{
	debugWriteString("Writing reg");
	writeRegister(0b0101000, 0x20, 7 + (2 << 4));
	debugWriteString("Acc ID: ");

	debugWriteString("Reading char ");
	debugWriteHex8(readRegister(0x0f));
	debugNewLine();
}



void printAccelerometerValues(void)
{
	int8_t x = readRegister(0x29), y = readRegister(0x2b), z = readRegister(0x2d);
	debugWriteString("Acc Data: (");
	debugWriteHex8(x);
	debugWriteChar(' ');
	debugWriteHex8(y);
	debugWriteChar(' ');
	debugWriteHex8(z);
	debugWriteString(") ");
	debugWriteFloat((float) x / 128.0f * 2.0f);
	debugWriteChar(' ');
	debugWriteFloat((float) y / 128.0f * 2.0f);
	debugWriteChar(' ');
	debugWriteFloat((float) z / 128.0f * 2.0f);
	debugWriteChar(' ');
	debugNewLine();
}

// print the product id for testing
void printCurrentProductId(uint8_t device)
{
	cli();

	// product id: 0xFD
	uint8_t address8 = getAddress(device) << 1;
	uint8_t result;

	debugWriteString("Product ID: ");

	result = i2c_start(address8);
	if (result)
	{
		debugWriteString("couldn't start i2c");
	}
	else
	{
		result = i2c_write(CURRENT_REG_PRODUCT_ID);
		if (result)
		{
			debugWriteString("couldn't send register address");
		}
		else
		{
			result = i2c_rep_start(address8 | 1);
			if (result)
			{
				debugWriteString("couldn't repeat start");
			}
			else
			{
				result = i2c_readNak();
			}
		}
	}
	i2c_stop();

	debugWriteHex8(result);
	debugNewLine();

	sei();
}

// this function is non-blocking
// triggers reading of sensor values
void currentSenseStartAll(void)
{
	currentDevice = CURRENT_USB;

	// start with first device
	currentState = I2C_START;

	// todo: really do measurement
	// circBufPushCurrentMeasurement(&monitorBuf, measurement);
	i2c_enable();

	i2c_start_nonblocking();
}


ISR(TWI_vect)
{
	cli();
	// debugWriteLine("TWI_vect");

	uint16_t data;
	uint8_t status = i2c_status();

	switch(currentState)
	{
		case I2C_START:
			if ( (status != TW_START) && (status != TW_REP_START)) 
				currentState = I2C_ERROR;
			else
			{
				currentState = I2C_ADDRESS;
				uint8_t address = getAddress(currentDevice) << 1;
				i2c_write_nonblocking(address);
			}
			break;
		
		case I2C_ADDRESS:
			if ( (status != TW_MT_SLA_ACK) && (status != TW_MR_SLA_ACK) ) 
				currentState = I2C_ERROR;
			else
			{
				currentState = I2C_REGISTER;
				// start with first register of channel 1
				i2c_write_nonblocking(CURRENT_REG_CHANNEL_1_HIGH);
			}
			break;

		case I2C_REGISTER:
			if( status != TW_MT_DATA_ACK) 
				currentState = I2C_ERROR;
			else
			{
				currentState = I2C_RESTART;
				i2c_start_nonblocking();
			}
			break;

		case I2C_RESTART:
			if ( (status != TW_START) && (status != TW_REP_START)) 
				currentState = I2C_ERROR;
			else
			{
				currentState = I2C_READDRESS;
				uint8_t address = (getAddress(currentDevice) << 1) | 1;
				i2c_write_nonblocking(address);
			}
			break;

		case I2C_READDRESS:
			if ( (status != TW_MT_SLA_ACK) && (status != TW_MR_SLA_ACK) ) 
				currentState = I2C_ERROR;
			else
			{
				currentByte = 0;
				currentState = I2C_READING;

				i2c_readAck_nonblocking();
			}
			break;

		case I2C_READING:
			data = (uint16_t) i2c_getData();
			
			// store new data
			switch(currentDevice)
			{
				case CURRENT_USB: // CURRENT_WIRELESS
				case CURRENT_MONITOR:
					switch(currentByte)
					{
						// USB H
						case 0:
							measurement.breakdown.usb = data << SENSE_SHIFT_UP;
							i2c_readAck_nonblocking();
							break;
						// USB L
						case 1:
							measurement.breakdown.usb |= data >> SENSE_SHIFT_DOWN;
							i2c_readAck_nonblocking();
							currentDevice = CURRENT_MONITOR;
							break;
						// MONITOR H
						case 2:
							measurement.breakdown.monitor = data << SENSE_SHIFT_UP;
							i2c_readNak_nonblocking();
							break;
						// MONITOR L
						case 3:
							measurement.breakdown.monitor |= data >> SENSE_SHIFT_DOWN;
							i2c_stop();

							// start next immediately
							currentDevice = CURRENT_WIRELESS;

							

							// start new measurement
							currentState = I2C_START;
							i2c_start_nonblocking();

							break;
						default:
							currentState = I2C_ERROR;
							break;
					}
					break;

				case CURRENT_MCU: 
				case CURRENT_WIRELESS:
					switch(currentByte)
					{
						// WIRELESS H
						case 0:
							measurement.breakdown.wireless = data << SENSE_SHIFT_UP;
							i2c_readAck_nonblocking();
							break;
						// WIRELESS L
						case 1:
							measurement.breakdown.wireless |= data >> SENSE_SHIFT_DOWN;
							i2c_readAck_nonblocking();
							currentDevice = CURRENT_MCU;
							break;
						// MCU H
						case 2:
							measurement.breakdown.mcu = data << SENSE_SHIFT_UP;
							i2c_readNak_nonblocking();
							break;
						// MCU L
						case 3:
							measurement.breakdown.mcu |= data >> SENSE_SHIFT_DOWN;
							i2c_stop();

#ifdef v1board
							// done!
							pushCurrentMeasurement(&measurement);
							// setLed(2, 0); // done with measurement

							if (pushMeasurements)
								printAllCurrentMeasurementsFloat();

							currentState = I2C_IDLE;

							i2c_disable();

							// debugWriteLine("done v1");
#else
							// start next immediately
							currentDevice = CURRENT_FPGA_VCCAUX;
							// start new measurement
							currentState = I2C_START;
							i2c_start_nonblocking();
#endif

							break;
						default:
							currentState = I2C_ERROR;
							break;
					}
					break;

				case CURRENT_FPGA_VCCINT:
				case CURRENT_FPGA_VCCAUX:
					switch(currentByte)
					{
						// VCCAUX H
						case 0:
							measurement.breakdown.vccaux = data << SENSE_SHIFT_UP;
							i2c_readAck_nonblocking();
							break;
						// VCCAUX L
						case 1:
							measurement.breakdown.vccaux |= data >> SENSE_SHIFT_DOWN;
							i2c_readAck_nonblocking();
							currentDevice = CURRENT_FPGA_VCCINT;
							break;
						// VCCINT H
						case 2:
							measurement.breakdown.vccint = data << SENSE_SHIFT_UP;
							i2c_readNak_nonblocking();
							break;
						// VCCINT L
						case 3:
							measurement.breakdown.vccint |= data >> SENSE_SHIFT_DOWN;
							i2c_stop();
							
							// debugWriteString("vccint ");
							// debugWriteFloat(measurement.breakdown.vccint);
							// debugNewLine();

							// done!

							// setLed(2, 0); // done with measurement

							i2c_disable();
							pushCurrentMeasurement(&monitorBuf, &measurement);

							if (pushMeasurements)
								printAllCurrentMeasurementsFloat();
							// debugWriteLine("done v2");

							currentState = I2C_IDLE;
							break;
						default:
							currentState = I2C_ERROR;
							break;
					}
					break;

				default:
					currentState = I2C_ERROR;
					break;
			}
			currentByte++;

			break;

		case I2C_IDLE:
		default:
			currentState = I2C_IDLE;
			break;
	}

	if (currentState == I2C_ERROR)
	{
		debugWriteLine("Error in I2C");
		
		i2c_stop();
		currentState = I2C_IDLE;
		// uartWriteLine("Error in I2C ");
	}
	
	// debugWriteLine("/TWI_vect");
	sei();
}

uint16_t readCurrentSense(uint8_t address, uint8_t channel)
{
	cli();

	uint8_t address8 = address << 1;
	uint8_t result;
	uint8_t reg;
	uint16_t sense = 0;

	result = i2c_start(address8);
	if (result)
	{
		debugWriteString("couldn't start i2c ");
		debugWriteBin8(address8);
		debugNewLine();
		sense = 0xFF8;
	}
	else
	{
		switch(channel)
		{
			case 1:
				reg = CURRENT_REG_CHANNEL_1_HIGH;
				break;
			case 2:
				reg = CURRENT_REG_CHANNEL_2_HIGH;
				break;
			default:
				debugWriteLine("Unknown current channel...");
				reg = 0;
				break;
		}
		result = i2c_write(reg);
		if (result)
		{
			debugWriteString("couldn't send register address");
			sense = 0xFF8;
		}
		else
		{
			result = i2c_rep_start(address8 | 1);
			if (result)
			{
				debugWriteString("couldn't repeat start");
				sense = 0xFF8;
			}
			else
			{
				// result is in 15-4 of these two registers 
				result = i2c_readAck();
				sense = result;
				sense <<= SENSE_SHIFT_UP;
				result = i2c_readNak();
				sense |= result >> SENSE_SHIFT_DOWN;
			}
		}
	}
	i2c_stop();

	sei();

	// check for disconnected
	if (sense == 0xFF8)
		return -1;
	else
		return sense;
}

currentMeasurementFloat fetchCurrentMeasurementFloat(void)
{
	currentMeasurementFloat output;
	output.breakdown.wireless = fetchCurrent(CURRENT_WIRELESS);
	output.breakdown.usb = fetchCurrent(CURRENT_USB);
	output.breakdown.monitor = fetchCurrent(CURRENT_MONITOR);
	output.breakdown.mcu = fetchCurrent(CURRENT_MCU);
	output.breakdown.vccaux = fetchCurrent(CURRENT_FPGA_VCCAUX);
	output.breakdown.vccint = fetchCurrent(CURRENT_FPGA_VCCINT);

	return output;
}

void printCurrent(uint8_t device)
{
	debugWriteString("Current ");
	debugWriteString(getName(device));
	debugWriteString(":\t");
	if(device == CURRENT_USB || device == CURRENT_MCU)
		debugWriteChar('\t');
	

	uint16_t currentSense = readCurrentSense(getAddress(device), getChannel(device));

	debugWriteString("(");
	debugWriteHex16(currentSense);
	debugWriteString(") ");

	float current = currentSenseConvert(device, currentSense);
	if (current != -1)
	// 	debugWriteLine("disconnected...");
	// else
	{
		debugWriteFloat(current * 1000);
		debugWriteLine(" mA");
	}
}

void printCurrentSense(uint8_t address, uint8_t channel)
{
	uint16_t sense = readCurrentSense(address, channel);

	// #warning "Function deprecated..."
	if (sense == -1 || sense == 0xFF8)
		debugWriteLine(" disconnected...");
	else
	{
		// debugWriteString(": ");
		debugWriteDec16(sense);
		debugNewLine();
	}
}

float fetchCurrent(uint8_t device)
{
	uint16_t currentSense = readCurrentSense(getAddress(device), getChannel(device));

	float current = currentSenseConvert(device, currentSense);
	return current * 1000.0f;
}


float currentSenseConvert(uint8_t device, uint16_t sense)
{
	if (sense == 0xFFF)
		return 0;
	else if (sense == -1 || sense == 0xFF8) 
		return -1;
	else
	{
		// negative
		if (sense >> (SENSE_BITS - 1))
		{	
			// pad value to 16 bit
			sense = sense | (0xffff << SENSE_BITS);
			sense = -((int16_t) sense);
		}

		
		// FSC = FSR/Rsense = 0.08/Rsense = 0.1
		// I = FSC * Vsense/Den= 0.1 * sense / 2047
		float R = getR(device);

		// debugWriteFloat(R);
		// debugWriteChar(' ');

		if (R == 0)
			return -1;

		// float R = (channel == 1) ? CURRENT_CHANNEL_1_R : CURRENT_CHANNEL_2_R;
		float current = FSR / R * (float)(sense) / DEN;
		return current;
	}
}

// found so far: 	0b00101001 	0b01001101	0b01001111
// 					2k			100			300
// 					WI &MCU		MON & USB 	FPGA
void findCurrentSensors()
{
	cli();

	debugWriteString("Finding current sensors...\n");
	// uint8_t address_a = 0b0101000, address_b = 0b10010000; // 0b01010000
	uint8_t address = 0b01010001;
	// uint8_t read = 0x1;
	// uint8_t i = 0;
	// uint8_t reg = 0xFD;
	// char msg[10];
	uint8_t response_a; //, response_b;
	// uint8_t state;

	debugNewLine();
	// for (i = 0; i <= 0xEF; i++)
	// {
		// address = address_a | (i << 1);
		// address = i << 1;

		// request data
		// debugWriteLine("starting");
		response_a = i2c_start(address);
		// debugWriteLine("stopping");
		i2c_stop();
		// debugWriteLine("done");
		if(!response_a)
		{
			debugWriteString("Found: ");
		// else
		// 	debugWriteString("Not found: ");
			debugWriteBin8(address >> 1);
			debugNewLine();
		}

		// address = address_b | (i << 1);
		// response_b = i2c_start(address);
		// i2c_stop();
		// if(!response_b)
		// 	debugWriteString("Found: ");
		// else
		// 	debugWriteString("Not found: ");
		// debugWriteBin8(address >> 1);
		// debugNewLine();
		// TWI_Start_Transceiver_With_Data(msg, 1);
		// state = TWI_Get_State_Info();

		// debugWriteBin8(address);
		// debugWriteString(" : ");
		// debugWriteHex8(response_a);
		// debugNewLine();

		// // read response
		// msg[1] = 0;
		// msg[0] |= read;

		// // debugWriteString("receiving...\n");
		// // response = TWI_Get_Data_From_Transceiver(msg, 2);


		// debugWriteHex8(response);
		// debugWriteString(" : ");
		// debugWriteHex8(msg[1]);

	// 	_delay_ms(1);
	// }

	debugNewLine();
	debugNewLine();

	sei();
}

char printBuf[50];
// print a single current measurement to the debug output
void printCurrentMeasurement(currentMeasurement c)
{
	// print a single measurement	
	for(int i = CURRENT_USB; i <= CURRENT_MONITOR; i++)
	{
		sprintf(printBuf, "%s: %.6f mA ", getName(i), 1000.0f * currentSenseConvert(i, measurement.measurements[i - 1]));
		debugWriteString(printBuf);
	}
	debugNewLine();
}

void printAllCurrentMeasurementsFloat(void)
{
	currentMeasurementFloat measurementFloat;
	uint8_t responses;

	cli();

	// first print size of buffer (in number of measurements)

	responses = circBufCountObjects(&monitorBuf, sizeof(currentMeasurementFloat));
	debugWriteChar(responses);
	if(responses > 0)
	{
		for(int i = 0; i < responses; i ++)
		{
			if (popCurrentMeasurement(&monitorBuf, &measurementFloat))
			{
				// debugWriteString("**");
				debugWriteStringLength((char *) measurementFloat.measurements, sizeof(measurementFloat));
			}
			else
			{
				debugWriteLine("not enough responses in queue");
				// uartWriteStringLength((char *) measurementFloat.measurements, sizeof(measurementFloat));
			}
		}
	}
	sei();	
}

void currentMeasurementConvert(currentMeasurement *measurement, currentMeasurementFloat *converted)
{
	// currentMeasurementFloat converted;

	// convert values one at a time
	for(int i = CURRENT_USB; i <= CURRENT_MONITOR; i++)
	{
		converted->measurements[i - 1] = 1000.0f * currentSenseConvert(i, measurement->measurements[i - 1]);
	}
	// return converted;
}

uint8_t getAddress(uint8_t device)
{
	switch(device)
	{
		case CURRENT_USB:
			return CURRENT_USB_ADDRESS;
		case CURRENT_MONITOR:
			return CURRENT_MONITOR_ADDRESS;
		case CURRENT_WIRELESS:
			return CURRENT_WIRELESS_ADDRESS;
		case CURRENT_MCU:
			return CURRENT_MCU_ADDRESS;
		case CURRENT_FPGA_VCCINT:
			return CURRENT_FPGA_VCCINT_ADDRESS;
		case CURRENT_FPGA_VCCAUX:
			return CURRENT_FPGA_VCCAUX_ADDRESS;
		case CURRENT_UNKNOWN:
		default:
			return 0;
	}
}

uint8_t getChannel(uint8_t device)
{
	switch(device)
	{
		case CURRENT_USB:
			return CURRENT_USB_CHANNEL;
		case CURRENT_MONITOR:
			return CURRENT_MONITOR_CHANNEL;
		case CURRENT_WIRELESS:
			return CURRENT_WIRELESS_CHANNEL;
		case CURRENT_MCU:
			return CURRENT_MCU_CHANNEL;
		case CURRENT_FPGA_VCCINT:
			return CURRENT_FPGA_VCCINT_CHANNEL;
		case CURRENT_FPGA_VCCAUX:
			return CURRENT_FPGA_VCCAUX_CHANNEL;
		case CURRENT_UNKNOWN:
		default:
			return 0;
	}
}

float getR(uint8_t device)
{
	switch(device)
	{
		case CURRENT_USB:
			return CURRENT_USB_R;
		case CURRENT_MONITOR:
			return CURRENT_MONITOR_R;
		case CURRENT_WIRELESS:
			return CURRENT_WIRELESS_R;
		case CURRENT_MCU:
			return CURRENT_MCU_R;
		case CURRENT_FPGA_VCCINT:
			return CURRENT_FPGA_VCCINT_R;
		case CURRENT_FPGA_VCCAUX:
			return CURRENT_FPGA_VCCAUX_R;
		case CURRENT_UNKNOWN:
		default:
			return 0;
	}
}

char *getName(uint8_t device)
{
	switch(device)
	{
		case CURRENT_USB:
			return "USB";
		case CURRENT_MONITOR:
			return "MONITOR";
		case CURRENT_WIRELESS:
			return "WIRELESS";
		case CURRENT_MCU:
			return "MCU";
		case CURRENT_FPGA_VCCINT:
			return "FPGA VCCINT";
		case CURRENT_FPGA_VCCAUX:
			return "FPGA VCCAUX";
		case CURRENT_UNKNOWN:
		default:
			return "Unknown";
	}
}
