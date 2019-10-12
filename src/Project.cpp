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
int RTCTimeHour;
int RTCTimeMinute;
int RTCTimeSecond;
int startTimeHour;
int startTimeMinute;
int startTimeSecond;
int systemTimeHour;
int systemTimeMinute;
int systemTimeSecond;
String alarm = "";

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
			alarmed = false;
            alarm = "";
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
        setStarTime(); 
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
    setStarTime();
    delay(1000);
    
    printf("|RTC Time|Sys Timer|Humidity|Temp|Light|DAC out|Alarm|"); //print the headings of the output table

    // Repeat this until we shut down
	for (;;)
    {
        if (monitoring)
        {
            toggleTime();
            //get the other readings, DAC out = (Light/1023) * Humidity
            //trigger alarm if needed and if alarm wasnt triggered 3 mins ago
            
            //Print The info to the Table
            printf("The current time is: %x:%x:%x\n", hours, mins, secs);
            //Blynk.virtualWrite(V0, RTCTime);       //Display RTC Time on Blynk
            printf("The current time is: %x:%x:%x\n", hours, mins, secs);
            //Blynk.virtualWrite(V1, systemTime);    //Display System Time on Blynk
            delay(frequency*1000);                   //Monitoring frequency, wait 1, 2 or 5 seconds 
        }
	}
	return 0;
}

void soundAlarm()
{
	alarmed = true;
    alarm = "*"
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
        time_t rawtime;
        struct tm * timeinfo;
        time (  &rawtime);
        timeinfo = localtime (  &rawtime);
        
        RTCTimeHour = timeinfo->tm_hour; 
        RTCTimeMinute = timeinfo->tm_min;
        RTCTimeSecond = timeinfo->tm_sec;
        
        systemTimeHour = RTCTimeHour - startTimeHour;
        systemTimeMinute = RTCTimeMinute - startTimeMinute;
        systemTimeSecond = RTCTimeSecond - startTimeSecond;
	}
	lastInterruptTime = interruptTime;
}

void setStarTime(void)
{
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200)
    {   
        time_t rawtime;
        struct tm * timeinfo;
        time (  &rawtime);
        timeinfo = localtime (  &rawtime);
        
        startTimeHour = timeinfo->tm_hour; 
        startTimeMinute = timeinfo->tm_min;
        startTimeSecond = timeinfo->tm_sec;
	}
	lastInterruptTime = interruptTime;    
}