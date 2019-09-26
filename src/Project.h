/*
 * Project.cpp
 *
 * Written for EEE3095S/EEE3096S
 * WLLCHE013 & PHHPET001
 * 25 September 2019
 */ 

#ifndef PROJECT_H
#define PROJECT_H

//Includes
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>
#include <vector>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <BlynkApiWiringPi.h>
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include <BlynkWidgets.h>

//Define buttons
#define MONITOR_BUTTON  
#define ALARM_BUTTON 
#define RESET_BUTTON 
#define INTERVAL_BUTTON

//Define alarm(s)
#define ALARM_SIGNAL

//SPI Settings               
#define SPI_CHAN 0
#define SPI_SPEED 12500000

//define Constants
const char RTCAddr = 0x6f;
const char SEC = 0x00; 
const char MIN = 0x01;
const char HOUR = 0x02;
const char TIMEZONE = 2;    // +02H00 (RSA)

//Function definitions
void initGPIO(void);
void monitor(void);
void stopAlarm(void);
void resetTime(void);
void changeReadInterval(void);
int main(void);


#endif