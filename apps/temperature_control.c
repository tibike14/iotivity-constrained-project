
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wiringPi.h>
#include "temperature_control.h"

void heat_on(void){
	digitalWrite(HeatPin1, HIGH);
	digitalWrite(HeatPin2, HIGH);
	digitalWrite(HeatPin3, HIGH);
	digitalWrite(CoolPin, LOW);
}

void heat_off(void){
	digitalWrite(HeatPin1, LOW);
	digitalWrite(HeatPin2, LOW);
	digitalWrite(HeatPin3, LOW);
	digitalWrite(CoolPin, LOW);
}

void cool_on(void){
	digitalWrite(HeatPin1, LOW);
	digitalWrite(HeatPin2, LOW);
	digitalWrite(HeatPin3, LOW);
	digitalWrite(CoolPin, HIGH);
}

void init(void)
{
	wiringPiSetup();

	pinMode(HeatPin1, OUTPUT);
	pinMode(HeatPin2, OUTPUT);
	pinMode(HeatPin3, OUTPUT);
	pinMode(CoolPin, OUTPUT);
}


int main(void)
{
	init();

	char *cmd = "./client_temp";
	char buf[BUFSIZE];
	FILE *fp;
	float temp_value;
	float ref_temp = 33.3;

	while(1){
		if ((fp = popen(cmd, "r")) == NULL) {
				printf("Error opening pipe!\n");
				return -1;
		}

		while (fgets(buf, BUFSIZE, fp) != NULL) {
				//printf("OUTPUT: %s", buf);
				temp_value = atof(buf);
				printf("%.1f\n", temp_value);

		}

		if(pclose(fp))  {
				printf("Command not found or exited with error status\n");
				return -1;
		}

		if (temp_value < ref_temp) {
			printf("heating process..\n");
			heat_on();
		}
		else if (temp_value > ref_temp) {
			printf("cooling process..\n");
			cool_on();
		}
		else {
			printf("nothing to do here..\n");
			heat_off();
			//break;
		}
	}

	return 0;
}


