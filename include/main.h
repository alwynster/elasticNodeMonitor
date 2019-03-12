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

		/* Includes: */

		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <util/delay.h>
		#include <stdbool.h>
		#include <string.h>

		#include <gpio.h>		
		#include <Descriptors.h>
		#include <usbInterface.h>

		#include <softuart.h>
		// #include <softSerial.h>
		#include <current_sense.h>
		#include <debug.h>
		// #include <avr/sleep.h>
		#include <avr/io.h>

	/* Function Prototypes: */
		void SetupHardware(void);

		/* LEDs */
		#define MIDI_LED_ON()      (PORTC |= (1<<6))
		#define MIDI_LED_OFF()      (PORTC &= ~(1<<6))
		#define USB_LED_ON()        (PORTC |= (1<<7))
		#define USB_LED_OFF()       (PORTC &= ~(1<<7))

		