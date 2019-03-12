/**
 * Copyright (c) 2012 Alfredo Prado <droky@radikalbytes.com.com>. All rights reserved.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "main.h"


int16_t received;
uint8_t command, channel, data2;
int status;

uint8_t monitoring;

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = 0,
				.DataINEndpoint                 =
					{
						.Address                = CDC_TX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.DataOUTEndpoint                =
					{
						.Address                = CDC_RX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.NotificationEndpoint           =
					{
						.Address                = CDC_NOTIFICATION_EPADDR,
						.Size                   = CDC_NOTIFICATION_EPSIZE,
						.Banks                  = 1,
					},
			},
	};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */

int main(void)
{

	DDRC |= _BV(6) | _BV(7);
	
	/* Initialization */
	SetupHardware();
	wdt_reset();
	GlobalInterruptEnable();


	// _delay_ms(500);

	debugWriteLine("Starting monitoring...");

	// CDC_Device_SendString(&VirtualSerial_CDC_Interface,"Starting monitoring... @\r\n");

	// CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	// USB_USBTask();	

	// current sensors
	// initCurrentSense();

	monitoring = 0;


	// _delay_ms
	// findCurrentSensors();


	/* Endless loop */
	for(;;)
	{
        int16_t data1 = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        if(!(data1<0))
        {
        	data2 = (uint8_t) data1;
        	if (data2 == 'C')
        	{
       			initCurrentSense();

        		debugWriteChar(data2);
        		monitoring = 1;

        		currentSenseConfig();

        		debugWriteString("Config: ");
        		debugWriteBin8(CURRENT_CONFIG);
        		debugWriteChar(' ');
        		debugWriteDec16(CURRENT_SENSE_OCR);
        		debugNewLine();



				printCurrent(CURRENT_USB);
				printCurrent(CURRENT_MONITOR);
				printCurrent(CURRENT_WIRELESS);
				printCurrent(CURRENT_MCU);
				printCurrent(CURRENT_FPGA_VCCINT);
				printCurrent(CURRENT_FPGA_VCCAUX);

				debugNewLine();
        	}
        	else if(data2=='l')
        	{
        		debugWriteChar(data2);
        		DDRD |= 3;
        		PORTC |= _BV(6) | _BV(7);
        		PORTD |= 3;
				_delay_ms(100);
				PORTC &= ~(_BV(6) | _BV(7));
				PORTD &= ~(3);
				_delay_ms(100);	
        	}
        	else if(data2 == 'd')
			{
				debugWriteLine("disable all sensors");
				for (uint8_t i = 1; i <= 6; i++)
				{
					writeRegister(getAddress(i), 0, 0x1B);
				}

			}
        	else if(data2 == 'e')
			{
				debugWriteLine("enable all sensors");
				for (uint8_t i = 1; i <= 6; i++)
				{
					writeRegister(getAddress(i), 0, 0);
				}
			}
        	else if(data2 == 'F')
        	{
				debugWriteChar(data2);
       			findCurrentSensors();
        	}
        	else if(data2 == 'M')
        	{
				debugWriteChar(data2);
				currentSenseClearMeasurements();
				currentSenseBegin();
        	} 
        	else if(data2 == 'm')
        	{
				debugWriteChar(data2);
				currentSenseEnd();
        	}
        	else if(data2 == 'f')
        	{
        		debugWriteChar(data2);
        		printAllCurrentMeasurementsFloat();
        	}
        	else if(data2 == 'a')
        	{
        		debugWriteChar(data2);
        		printAccelerometerValues();
        	}
        	else if(data2 == 'W')
        	{
        		i2c_init();
				i2c_enable();
        		debugWriteChar(data2);
        		printAccID();	
        	}
   //      	else if(data2 == 's')
   //      	{

   //      		USB_Disable();   // turn off usb to prevent wake from sleep
			//     wdt_disable();  // turn off wdt before sleep
			//     set_sleep_mode(SLEEP_MODE_IDLE);
			//     sleep_enable();
			//     // attachInterrupt(0, pin3_ISR, LOW);
			//     cli();
			// // enable sleeping wake-up
			// 	DDRE &= ~_BV(PE6);
			// 	PORTE |= _BV(PE6);
			// 	EIMSK |= _BV(INT6);
			// 	EICRB |= (_BV(ISC60) | _BV(ISC61));
			// 	sei();
			//     sleep_cpu();    
			//     // CPU will wakeup here - pin3 ISR fires here
			    
			//     // _delay_ms(4);

			//     sleep_disable();

			//     SetupHardware();   //turn usb back on
			//     // wdt_enable(WDTO_8S);  // enable the watchdog - was disabled in usb_init

   //      	}
   //      	CDC_Device_SendByte(&VirtualSerial_CDC_Interface,(uint8_t)data1);
			// CDC_Device_SendString(&VirtualSerial_CDC_Interface," @\r\n");
        }

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();	
	}
	
}

/** Configures the board hardware and chip peripherals */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	// /* LEDs */
	// DDRC |= (1<<6); //PC6 MIDI
	// DDRC |= (1<<7); //PC7 USB

	// /* LEDs OFF */
	// MIDI_LED_OFF();
	// USB_LED_OFF();

	/* Hardware Initialization */
	USB_Init();

	// USB_Disable();
	// USB_Init();
	// MIDI_LED_ON();
	// _delay_ms(150);
	// MIDI_LED_OFF();

	
}

// wake up!
// ISR(INT6_vect)
// {
// 	EIMSK &= ~_BV(INT6);

// 	debugWriteLine("wake up!");
// }

void usbSendChar(char str)
{
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, str);
}

void usbSendString(char *str)
{
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, str);
}




/*********************************************************
 **                                                     **
 **   USB Functions used from LUFA CDC Virtual Serial   **
 **                                                     **
 ********************************************************/

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	// USB_LED_ON();
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	// USB_LED_OFF();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;
	uint8_t ff;
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
	// for (ff=0;ff<6;ff++)
	// {
	// 		MIDI_LED_ON();
	// 		_delay_ms(25);
	// 		MIDI_LED_OFF();
	// 		_delay_ms(25);
	// }

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
	uint8_t ConfigMask = 0;

	switch (CDCInterfaceInfo->State.LineEncoding.ParityType)
	{
		case CDC_PARITY_Odd:
			ConfigMask = ((1 << UPM11) | (1 << UPM10));
			break;
		case CDC_PARITY_Even:
			ConfigMask = (1 << UPM11);
			break;
	}

	if (CDCInterfaceInfo->State.LineEncoding.CharFormat == CDC_LINEENCODING_TwoStopBits)
	  ConfigMask |= (1 << USBS1);

	switch (CDCInterfaceInfo->State.LineEncoding.DataBits)
	{
		case 6:
			ConfigMask |= (1 << UCSZ10);
			break;
		case 7:
			ConfigMask |= (1 << UCSZ11);
			break;
		case 8:
			ConfigMask |= ((1 << UCSZ11) | (1 << UCSZ10));
			break;
	}

	/* Must turn off USART before reconfiguring it, otherwise incorrect operation may occur */
	UCSR1B = 0;
	UCSR1A = 0;
	UCSR1C = 0;

	/* Set the new baud rate before configuring the USART */
	UBRR1  = SERIAL_2X_UBBRVAL(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);

	/* Reconfigure the USART in double speed mode for a wider baud rate range at the expense of accuracy */
	UCSR1C = ConfigMask;
	UCSR1A = (1 << U2X1);
	UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));
}




