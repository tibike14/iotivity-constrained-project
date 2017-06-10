
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <wiringPi.h>
#include "light_control.h"


void light_on(void)
{
	digitalWrite(LEDPin1, HIGH);
	digitalWrite(LEDPin2, HIGH);
	digitalWrite(LEDPin3, HIGH);
	printf("light ON\n");
}


void light_off(void)
{
	digitalWrite(LEDPin1, LOW);
	digitalWrite(LEDPin2, LOW);
	digitalWrite(LEDPin3, LOW);
	printf("light OFF\n");
}

void init(void)
{
	wiringPiSetup();
	pinMode(LEDPin1, OUTPUT);
	pinMode(LEDPin2, OUTPUT);
	pinMode(LEDPin3, OUTPUT);
}

float request_temperature(void)
{

	char *cmd = "./client_temp";
	char buf[BUFSIZE];
	FILE *fp;
	float tempValue;


	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		tempValue = atof(buf);
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}

	return tempValue;
}	//EOF


int request_motion(void)
{

	char *cmd = "./client_motion";
	char buf[BUFSIZE];
	FILE *fp;
	int motionState;


	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		motionState = atoi(buf);
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}
	return motionState;
}	//EOF




int main(void)
{
	init();
	struct timespec tstart={0,0}, tend={0,0};
	float tempValue;
	float oldTempValue;
	float tempChange ;
	float tempChangeLimit = 2.5;
	int motionState;
	int timeout = 10;
	int refTimeout = 10;
	int startup = 1;

	while(1) {

		clock_gettime(CLOCK_MONOTONIC, &tstart);
		motionState = request_motion();
		clock_gettime(CLOCK_MONOTONIC, &tend);
		double elapsedTime = (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
					((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)) ;

		printf("RESPONE TIME..: %.5f\n", elapsedTime);

		tempValue = request_temperature();
		printf("DBG --> tempValue: %.1f\n", tempValue);
		printf("DBG --> motionState: %d\n", motionState);

		if (startup == 1 && !motionState) {
			delay(1000);
			oldTempValue = tempValue;
			tempValue = request_temperature();
			startup = 0;
		}

		tempChange = oldTempValue - tempValue;
        if (tempChange < 0){
        	tempChange = tempChange * (-1);
        }
        printf("DBG --> CHANGE...: %.1f\n", tempChange);

		if (motionState == 1 || tempChange >= tempChangeLimit || timeout < refTimeout){
			light_on();
			timeout--;
			if (timeout <= 0) {
				timeout = refTimeout;
			}
			printf("DBG --> timeout: %d\n", timeout);

		}
		else {
			light_off();
			timeout = refTimeout;
		}

		tempValue = request_temperature();
		oldTempValue = tempValue;
		delay(500);
	}

	return 0;
}	//EOF

