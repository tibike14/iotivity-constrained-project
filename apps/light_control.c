

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//#include <math.h>

//#include <wiringPi.h>
#include "light_control.h"


void light_on(void)
{/*
	digitalWrite(LEDPin1, HIGH);
	digitalWrite(LEDPin2, HIGH);
	digitalWrite(LEDPin3, HIGH);
*/
	printf("light ON\n");
}


void light_off(void)
{
	/*
	digitalWrite(LEDPin1, LOW);
	digitalWrite(LEDPin2, LOW);
	digitalWrite(LEDPin3, LOW);
	*/
	printf("light OFF\n");
}
/*
void init(void)
{
	wiringPiSetup();

	pinMode(LEDPin1, OUTPUT);
	pinMode(LEDPin2, OUTPUT);
	pinMode(LEDPin3, OUTPUT);
}
*/

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
				//printf("OUTPUT: %s", buf);
		tempValue = atof(buf);
		printf("%.1f\n", tempValue);

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
				//printf("OUTPUT: %s", buf);
		motionState = atoi(buf);
		printf("what got from pipe: %d\n", motionState);

	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}

	return motionState;
}	//EOF




int main(void)
{
//	init();

	float tempValue;
	float oldTempValue;
	float tempChange = 1.1;
	float tempChangeLimit = 10.1;
	int motionState;
	int timeout;
	int refTimeout = 7;
	int startup = 1;

	while(1) {
		//tempValue = request_temperature();
		motionState = request_motion();
		printf("DBG_CTRL --> motionState: %d\n", motionState);

		if (startup == 1 && !motionState) {
			delay(500);
			oldTempValue = tempValue;
			tempValue = request_temp();
			startup = 0;
		}

		tempChange = abs(oldTempValue - tempValue);

		if (motionState == 1 || tempChange >= tempChangeLimit || timeout >= refTimeout){
			light_on();
			timeout--;
			printf("DBG_CTRL --> timeout: %d\n", timeout);
			//delay(50);
		}
		else {
			light_off();
			timeout = 0;
			//delay(50);
		}

		tempValue = request_temp();
		oldTempValue = tempValue;
		delay(500);


	} //end of while

	return 0;
}	//EOF

