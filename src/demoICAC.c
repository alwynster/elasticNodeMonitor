#include <avr/power.h>
#include <avr/wdt.h>
#include "lib/debug/debug.h"
#include "lib/current_sense/current_sense.h"
#include "lib/current_sense/current_measurement.h"

int main(void)
{
    int finished;
    debugInit(NULL);
    initCurrentSense();
    currentSenseConfig();
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    clock_prescale_set(clock_div_1);


    finished = 0;
    while(!finished)
    {
        debugReadChar();
        if (debugReadCharAvailable())
        {
            uint8_t data = debugGetChar();

            switch(data)
            {
                case 'C':
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
                    break;

                case 'F':
                    debugWriteChar(data);
                    findCurrentSensors();
                    break;

                case 'M':
                    debugWriteChar(data);
                    currentSenseClearMeasurements();
                    currentSenseBegin();
                    break;

                case 'm':
                    debugWriteChar(data);
                    currentSenseEnd();
                    break;

                case 'f':
                    debugWriteChar(data);

                    currentMeasurementFloat current = fetchCurrentMeasurementFloat();
                    // for(uint8_t *ptr = (uint8_t *)current.measurements; ptr < (uint8_t *)current.measurements + (sizeof(uint8_t) * sizeof(currentMeasurementFloat)); ptr++)
                    debugWriteStringLength((char *) current.measurements, sizeof(currentMeasurementFloat));
                    break;
                case 'q':
                    finished = 1;
                    break;

                default:
                    break;
            }
        }
    }
    return 0;
}