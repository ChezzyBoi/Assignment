/*
 * Project.cpp
 *
 * Written for EEE3095S/EEE3096S
 * WLLCHE013 & PHHPET001
 * 25 September 2019
 */ 

using namespace std;

#ifndef PROJECT_H
#define PROJECT_H

//Includes
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>
#include <mcp3004.h>
#include <vector>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <time.h> 
#include <iomanip>
#include <sstream>

/*
#include <BlynkApiWiringPi.h>
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include <BlynkWidgets.h>
*/

//Define buttons
#define MONITOR_BUTTON 7
#define ALARM_BUTTON 0
#define RESET_BUTTON 2
#define INTERVAL_BUTTON 3

//Define alarm(s)
#define ALARM_SIGNAL 23

//SPI Settings               
#define SPI_CHAN 0
#define BASE 100
#define SPI_SPEED 1250000
#define HUMID 2          //chanel number of ADC
#define LIGHT 0
#define TEMP  1
  
//Function definitions
void initGPIO(void);
void monitor(void);
void stopAlarm(void);
void resetTime(void);
void changeReadInterval(void);
void setStarTime(void);
void soundAlarm(void);
void toggleTime(void);
int main(void);
float prec(float);
void setStarTime(void);
void resetTimer(void);
std::string toStr (float);


#endif
