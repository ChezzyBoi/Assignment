/*
 * Project.cpp
 * Modified for EEE3095S/EEE3096S
 * September 2019
 * 
 * WLLCHE013 & PHHPET001
 * 25 September 2019
*/

#include "Project.h"
#include "CurrentTime.h"

//Global variables       
long lastInterruptTime = 0;     //Used for button debounce
int RTC;                        //Holds the RTC instance
bool monitoring = true;         //Boolean to State if Sensors are Monitoring
bool alarmed = false;           //Boolean to State if the Alarm is Sounding
int frequency = 1;              //Frequency of the monitoring in seconds

int HH,MM,SS;                   //Used for RTC Time
int h,m,s;                      //Used as Srart Time for System
int hh,mm,ss;                   //Used for the System Timer
String RTCTime;
String systemTime;

void initGPIO(void)
{
	//Set Up Wiring Pi
	wiringPiSetup(); 
	
	//Setting Up The RTC
	RTC = wiringPiI2CSetup(RTCAddr); 
	//Setting Up The SPI Interface
	wiringPiSetup(SPI_CHAN, SPI_SPEED);

	//Setting Up The Buttons
	//Stop/Start Monitoring Button
	pinMode(MONITOR_BUTTON, INPUT);                         //Set button to input mode
	pullUpDnControl(MONITOR_BUTTON, PUP_UP);                //Set up pull-up resistor 
	wiringPiISR(MONITOR_BUTTON, INT_EDGE_FALLING, monitor); //Attach Interrupt to button
	//Dismiss Alarm Button                                      
	pinMode(ALARM_BUTTON, INPUT);                           //Set button to input mode
	pullUpDnControl(ALARM_BUTTON, PUP_UP);                  //Set up pull-up resistor 
	wiringPiISR(ALARM_BUTTON, INT_EDGE_FALLING, stopAlarm); //Attach Interrupt to button
	//Reset System Time and Clear Concole Button 
	pinMode(RESET_BUTTON, INPUT);                           //Set button to input mode
	pullUpDnControl(RESET_BUTTON, PUP_UP);                  //Set up pull-up resistor 
	wiringPiISR(RESET_BUTTON, INT_EDGE_FALLING, resetTime); //Attach Interrupt to button
	//Change Reading Interval Button  
	pinMode(INTERVAL_BUTTON, INPUT);                                    //Set button to input mode
	pullUpDnControl(INTERVAL_BUTTON, PUP_UP);                           //Set up pull-up resistor 
	wiringPiISR(INTERVAL_BUTTON, INT_EDGE_FALLING, changeReadInterval); //Attach Interrupt to button 
}

void monitor(void)
{
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200)
	{
		if (monitoring == true)
		{
			monitoring = false;
		}
		else
		{
			monitoring = true;
		}
    }
	lastInterruptTime = interruptTime;
}

void stopAlarm(void)
{
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200)
	{
		if (alarmed == true)
		{
			alarmed == false;
			ALARM_SIGNAL = 0;
		}
    }
	lastInterruptTime = interruptTime;	
}

void resetTime(void)
{
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200)
	{
		system("@cls||clear");
		//terminal.clear();   //Clear Blynk Terminal
		h = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		m = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
		s = hexCompensation(wiringPiI2CReadReg8(RTC, SEC) - 0b10000000);
    }
	lastInterruptTime = interruptTime;
}

void changeReadInterval(void)
{
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200)
	{
		if(frequency == 1)
		{
			frequency = 2;
		}
		if (frequency == 2)
		{
			frequency = 5;
		}
		if (frequency == 5) 
		{
			frequency = 1;
		}
    }
	lastInterruptTime = interruptTime;
}

/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void)
{
	initGPIO();
    toggleTime();
	h = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
	m = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
	s = hexCompensation(wiringPiI2CReadReg8(RTC, SEC) - 0b10000000);
    delay(1000);

    // Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC and write system time
		HH = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		MM = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
		SS = hexCompensation(wiringPiI2CReadReg8(RTC, SEC) - 0b10000000);
		hh = abs(HH - h);
		mm = abs(MM - m);
		ss = abs(SS - s);
		//Print The System Time and The RTC Time
		RTCTime = HH + ":" + MM +":"+ SS;
		//Blynk.virtualWrite(V0, RTCTime);       //Display RTC Time on Blynk
		systemTime = hh + ":" + mm +":"+ ss;
        //Blynk.virtualWrite(V1, systemTime);    //Display System Time on Blynk
	}
	return 0;
}

void soundAlarm()
{
	alarmed = true;
	softPwmCreate(ALARM_SIGNAL, 0, 2);  
}

/*
Blynk Stuff Start Here
static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

void setup()
{
}

void loop()
{
	Blynk.run();
}

int main(int argc, char* argv[])
{
  const char *auth, *serv;
  uint16_t port;
  parse_options(argc, argv, auth, serv, port);

  Blynk.begin(auth, serv, port);

  setup();
  while(true) {
    loop();
  }

  return 0;
}
Blynk Stuff End Here
*/

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}

/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}