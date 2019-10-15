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
bool monitoring = false;         //Boolean to State if Sensors are Monitoring
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
int timerSecond = 0;
int timerMinute = 0;
int timerHour = 0;
float temperature;
float humidity;
float light;
float DAC_out;
int timer = 0;
int lastAlarm = 369;   //random amount higher than 3 minutes

string alarmStr = "";

const char separator    = ' ';
const int nameWidth     = 12;
const int numWidth      = 6;

void initGPIO(void)
{
	//Set Up Wiring Pi
	wiringPiSetup();

	//Setting Up The SPI Interface
	wiringPiSPISetup(SPI_CHAN, SPI_SPEED);
	
    	mcp3004Setup (BASE, SPI_CHAN);

	//Setting Up The Buttons
	//Stop/Start Monitoring Button
	pinMode(MONITOR_BUTTON, INPUT);                         //Set button to input mode
	pullUpDnControl(MONITOR_BUTTON, PUD_UP);                //Set up pull-up resistor
	wiringPiISR(MONITOR_BUTTON, INT_EDGE_FALLING, monitor); //Attach Interrupt to button
	//Dismiss Alarm Button
	pinMode(ALARM_BUTTON, INPUT);                           //Set button to input mode
	pullUpDnControl(ALARM_BUTTON, PUD_UP);                  //Set up pull-up resistor
	wiringPiISR(ALARM_BUTTON, INT_EDGE_FALLING, stopAlarm); //Attach Interrupt to button
	//Reset System Time and Clear Concole Button
	pinMode(RESET_BUTTON, INPUT);                           //Set button to input mode
	pullUpDnControl(RESET_BUTTON, PUD_UP);                  //Set up pull-up resistor
	wiringPiISR(RESET_BUTTON, INT_EDGE_FALLING, resetTime); //Attach Interrupt to button
	//Change Reading Interval Button
	pinMode(INTERVAL_BUTTON, INPUT);                                    //Set button to input mode
	pullUpDnControl(INTERVAL_BUTTON, PUD_UP);                           //Set up pull-up resistor
	wiringPiISR(INTERVAL_BUTTON, INT_EDGE_FALLING, changeReadInterval); //Attach Interrupt to button
	
	setStarTime();
}

void monitor(void){
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200){
		if (monitoring == true){
			monitoring = false;
		}else{
			monitoring = true;
		}
    }

    lastInterruptTime = interruptTime;
}

void stopAlarm(void){
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200){
		if (alarmed == true){
			alarmed = false;
            		alarmStr = "";
			softPwmWrite(ALARM_SIGNAL, 0);
			//ALARM_SIGNAL 0;
		}
    }
	lastInterruptTime = interruptTime;
}

void resetTime(void){
    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200)
	{
		system("@cls||clear");
		
		 cout << left << setw(nameWidth) << setfill(separator) << "RTC Time";
    cout << left << setw(nameWidth) << setfill(separator) << "Sys Timer";
    cout << left << setw(nameWidth) << setfill(separator) << "Humidity";
    cout << left << setw(nameWidth) << setfill(separator) << "Temp";
    cout << left << setw(nameWidth) << setfill(separator) << "Light";
    cout << left << setw(nameWidth) << setfill(separator) << "DAC Out";
    cout << left << setw(nameWidth) << setfill(separator) << "Alarm";
    cout << endl;


	        resetTimer();
    }
	lastInterruptTime = interruptTime;
}

void changeReadInterval(void){

    long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200){
		if(frequency == 1)
		{
			frequency = 2;
		}
		else if (frequency == 2)
		{
			frequency = 5;
		}
		else if (frequency == 5)
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

 //   printf("|RTC Time|Sys Timer|Humidity|Temp|Light|DAC out|Alarm|\n"); //print the headings of the output table
    cout << left << setw(nameWidth) << setfill(separator) << "RTC Time";
    cout << left << setw(nameWidth) << setfill(separator) << "Sys Timer";
    cout << left << setw(nameWidth) << setfill(separator) << "Humidity";
    cout << left << setw(nameWidth) << setfill(separator) << "Temp";
    cout << left << setw(nameWidth) << setfill(separator) << "Light";
    cout << left << setw(nameWidth) << setfill(separator) << "DAC Out";
    cout << left << setw(nameWidth) << setfill(separator) << "Alarm";
    cout << endl;

    // Repeat this until we shut down	
for (;;)
    {
        if (monitoring)
        {
            toggleTime();

            //Get the data readings
            humidity = prec(analogRead(BASE + HUMID)/310.0);
            light =  analogRead(BASE + LIGHT);
            temperature = prec(5.0/9.0 * (analogRead(BASE + TEMP)-32));
           
	    DAC_out = prec((light/1023)*humidity);

	  timerSecond+=frequency;

	  // Set system timer
	  if(timerSecond >= 60){
	   timerSecond = timerSecond-60;
	   timerMinute++;
	   	if(timerMinute == 60){
		 timerMinute = 0;
                 timerHour = 0;
		}
	  }

	    cout << left << setw(nameWidth) << setfill(separator) << std::to_string(RTCTimeHour) +  ":" +std::to_string(RTCTimeMinute)+ ":" + std::to_string(RTCTimeSecond);
	    cout << left << setw(nameWidth) << setfill(separator) << std::to_string(timerHour) + ":" + std::to_string(timerMinute) + ":" + std::to_string(timerSecond);
	    cout << left << setw(nameWidth) << setfill(separator) << toStr(humidity) + " V";
	    cout << left << setw(nameWidth) << setfill(separator) << toStr(temperature) + " C";
	    cout << left << setw(nameWidth) << setfill(separator) << toStr(light);
	    cout << left << setw(nameWidth) << setfill(separator) << toStr(DAC_out) + " V";
	    cout << left << setw(nameWidth) << setfill(separator) << alarmStr;
	    cout << endl;
	   

	    //Convert the temperature to a value in degrees celcuis


            //trigger alarm if above or below threshold and if alarm wasnt triggered 3 mins ago
            if (DAC_out<0.65 || DAC_out>2.65)
            {
                if (lastAlarm > 180)  //3 minutes = 180 seconds
                {
                    soundAlarm();
                }
            }

            lastAlarm = lastAlarm + frequency;  //increase by frequency seconds after each read since that will be how many seconds has passed

            //Print The info to the Table
            //printf("The current time is: %x:%x:%x\n", hours, mins, secs);
            //Blynk.virtualWrite(V0, RTCTime);       //Display RTC Time on Blynk
            //printf("The current time is: %x:%x:%x\n", hours, mins, secs);
            //Blynk.virtualWrite(V1, systemTime);    //Display System Time on Blynk
            delay(frequency*1000);                   //Monitoring frequency, wait 1, 2 or 5 seconds
        }
	}
	return 0;
}


float prec(float var) 
{ 
    float value = (int)(var * 10 + .5); 
    return (float)value / 10; 
}

std::string toStr (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();   
} 

void soundAlarm()
{
	alarmed = true;
    alarmStr = "*";

softPwmCreate(ALARM_SIGNAL, 0, 10);
softPwmWrite(ALARM_SIGNAL, 5);

    lastAlarm = 0;
}

void resetTimer(void){
	timerSecond = 0;
        timerMinute = 0;
        timerHour = 0;
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

void setStarTime(void)
{
        time_t rawtime;
        struct tm * timeinfo;
        time (  &rawtime);
        timeinfo = localtime (  &rawtime);

        startTimeHour = timeinfo->tm_hour;
        startTimeMinute = timeinfo->tm_min;
        startTimeSecond = timeinfo->tm_sec;
}
