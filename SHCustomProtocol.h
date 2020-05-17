#ifndef __SHCUSTOMPROTOCOL_H__
#define __SHCUSTOMPROTOCOL_H__

/* SimHub custom hardware to support SwitechX25/27 gauges
 * Arduino via Switec X12.017 and compatible chips
 * good info at https://guy.carpenter.id.au/gaugette/2017/04/29/switecx25-quad-driver-tests/

 * REQUIRES https://github.com/penfold42/SwitecX25/

 * expects 2 parameters split by ;
 * parameter 1: step count to set the gauge to (0-3779)
 * parameter 2: used to recalibrate the gauge if changed from 0 to 1

SimHub dash config:
- My gauge has 9000 RPM at a count of approx 2700 steps
- I use a change in oil pressure at engine start to recal the gauge

format( [Rpms]*27/90,'0') + ';' +
format( [OilPressure]*10,'0' )

 */
 
#include <Arduino.h>
#include <SwitecX12.h>

// standard X25.168 range 315 degrees at 1/12 degree steps
#define STEPS (315*12)


//    SwitecX12(unsigned int steps, unsigned char pinStep, unsigned char pinDir);
SwitecX12 motor1(STEPS,21, 20, 19);


class SHCustomProtocol {
private:
	int lastOil = 0;

	// mode 0 - normal running
	// mode 1 - seeking to full
	// mode 2 - seeking to zero
	int mode = 0;
public:

	/*
	CUSTOM PROTOCOL CLASS
	SEE https://github.com/zegreatclan/SimHub/wiki/Custom-Arduino-hardware-support

	GENERAL RULES :
		- ALWAYS BACKUP THIS FILE, reinstalling/updating SimHub would overwrite it with the default version.
		- Read data AS FAST AS POSSIBLE in the read function
		- NEVER block the arduino (using delay for instance)
		- Make sure the data read in "read()" function READS ALL THE DATA from the serial port matching the custom protocol definition
		- Idle function is called hundreds of times per second, never use it for slow code, arduino performances would fall
		- If you use library suspending interrupts make sure to use it only in the "read" function when ALL data has been read from the serial port.
			It is the only interrupt safe place

	COMMON FUNCTIONS :
		- FlowSerialReadStringUntil('\n')
			Read the incoming data up to the end (\n) won't be included
		- FlowSerialReadStringUntil(';')
			Read the incoming data up to the separator (;) separator won't be included
		- FlowSerialDebugPrintLn(string)
			Send a debug message to simhub which will display in the log panel and log file (only use it when debugging, it would slow down arduino in run conditions)

	*/

	// Called when starting the arduino (setup method in main sketch)
	void setup() {
		// run the motor against the stops
		motor1.zero();
		delay(100);
		// start moving towards the center of the range
		motor1.full();
		delay(100);
		motor1.setPosition(0);
	}

	// Called when new data is coming from computer
	void read() {

		int MicroStep = FlowSerialReadStringUntil(';').toInt();
		int       Oil = FlowSerialReadStringUntil('\n').toInt();

		if ( (Oil > 0)  && (lastOil == 0) ){
			mode = 1;
			motor1.setPosition(motor1.steps-1);

		} else {
			switch (mode) {
				case 0:
					motor1.setPosition(MicroStep);
					break;
				case 1:
					if (motor1.currentStep == motor1.steps-1) {
						mode = 2;
						motor1.setPosition(0);
					}
					break;
				case 2:
					if (motor1.currentStep == 0) {
						mode = 0;
					}
					break;
			}
		}
		lastOil = Oil;

	}

	// Called once per arduino loop, timing can't be predicted, 
	// but it's called between each command sent to the arduino
	void loop() {
		motor1.update();
	}

	// Called once between each byte read on arduino,
	// THIS IS A CRITICAL PATH :
	// AVOID ANY TIME CONSUMING ROUTINES !!!
	// PREFER READ OR LOOP METHOS AS MUCH AS POSSIBLE
	// AVOID ANY INTERRUPTS DISABLE (serial data would be lost!!!)
	void idle() {
	}
};

#endif
