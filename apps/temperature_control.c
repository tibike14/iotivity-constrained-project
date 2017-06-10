
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include <wiringPi.h>
#include "temperature_control.h"

float offset = 1.5;
int heatVar = 0;
int coolVar = 0;


void heat_on(void){
	digitalWrite(HeatPin1, HIGH);
	digitalWrite(HeatPin2, HIGH);
	digitalWrite(HeatPin3, HIGH);
	digitalWrite(CoolPin, LOW);
	printf("heating..\n");
}

void switch_off(void){
	digitalWrite(HeatPin1, LOW);
	digitalWrite(HeatPin2, LOW);
	digitalWrite(HeatPin3, LOW);
	digitalWrite(CoolPin, LOW);
	printf("hvac system off..\n");
}

void cool_on(void){
	digitalWrite(HeatPin1, LOW);
	digitalWrite(HeatPin2, LOW);
	digitalWrite(HeatPin3, LOW);
	digitalWrite(CoolPin, HIGH);
	printf("cooling..\n");
}

void init(void)
{
	wiringPiSetup();
	pinMode(HeatPin1, OUTPUT);
	pinMode(HeatPin2, OUTPUT);
	pinMode(HeatPin3, OUTPUT);
	pinMode(CoolPin, OUTPUT);
}


float request_temperature(void)
{
	char *cmd = "./client_temp";
	char buf[BUFSIZE];
	FILE *fp;
	float tempValue;
//	float refTemp = 33.3;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		//printf("OUTPUT: %s", buf);
		tempValue = atof(buf);
		//printf("%.1f\n", tempValue);
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
//		printf("OUTPUT: %s", buf);
		motionState = atoi(buf);
		printf("moitonState (PIPE): %d\n", motionState);
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}

	return motionState;
}	//EOF



void control_temperature(void)
{
	float refTemp = 33.5;
	float tempValue;

	tempValue = request_temperature();
	printf("TEMPERATURE: %.1f\n", tempValue);

	if (tempValue < (refTemp - offset) || heatVar == 1) {
		if (tempValue < refTemp){
			heatVar = 1;
			heat_on();
			delay(500);
		}
		else{
			heatVar = 0;
		}
	}

	else if (tempValue > (refTemp + offset) || coolVar == 1) {
		if (tempValue > refTemp){
			coolVar = 1;
			cool_on();
			delay(500);
		}
		else{
			coolVar = 0;
		}

	}
	else {
		switch_off();
		delay(500);
	}
}	//EOF

void standby_control_temperature(void)
{
	float standbyRefTemp = 29.5;
	float tempValue;
	tempValue = request_temperature();
	printf("TEMPERATURE: %.1f\n", tempValue);
	if (tempValue < (standbyRefTemp - offset) || heatVar == 1) {
		if (tempValue < standbyRefTemp){
			heatVar = 1;
			heat_on();
			delay(500);
		}
		else
			heatVar = 0;
	}
	else {
		switch_off();
		delay(1000);
	}
}	//EOF


int main(void)
{
	struct timespec tstart={0,0}, tend={0,0};
	float motionState;
	init();
	while(1){
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		motionState = request_motion();
		clock_gettime(CLOCK_MONOTONIC, &tend);
		double elapsedTime = (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
                		((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)) ;

		printf("RESPONSE TIME: %.5f\n", elapsedTime);

		if (motionState == -1) {
			printf("no motion data --> standalone mode\n");
			control_temperature();
		}
		else {
			printf("motion data available --> JOINT-OP mode\n");
			if (motionState == 1){
				printf("ACTIVE\n");
				control_temperature();
			}
			else if (motionState == 0) {
				printf("INACTIVE\n");
				standby_control_temperature();
			}
			else {
				printf("ELSE BRANCH...\n");
				delay(1000);
				//continue;
			}
		}
	}
	return 0;
}


