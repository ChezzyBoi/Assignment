/*
 * Project.cpp
 * Modified for EEE3095S/EEE3096S
 * September 2019
 * 
 * WLLCHE013 & PHHPET001
 * 25 September 2019
*/

#include "Project.h"

//Global variables       
long lastInterruptTime = 0;     //Used for button debounce
bool monitoring = true;         //Boolean to State if Sensors are Monitoring
bool alarmed = false;           //Boolean to State if the Alarm is Sounding
int frequency = 1;              //Frequency of the monitoring in seconds

void initGPIO(void)
{
	//Set Up Wiring Pi
	wiringPiSetup(); 
	
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
        //systemTime 
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
    delay(1000);

    // Repeat this until we shut down
	for (;;)
    {
        if (monitoring)
        {
            //Print The System Time and The RTC Time
		
            //Blynk.virtualWrite(V0, RTCTime);       //Display RTC Time on Blynk
		
            //Blynk.virtualWrite(V1, systemTime);    //Display System Time on Blynk
            delay(frequency*1000);
        }
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

//This interrupt will fetch current time
void toggleTime(void)
{
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200)
    {

	}
	lastInterruptTime = interruptTime;
}