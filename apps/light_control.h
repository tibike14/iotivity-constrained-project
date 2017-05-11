#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#define BUFSIZE 32

#define LEDPin1 7
#define LEDPin2 2
#define LEDPin3 3


//void init(void);
float request_temperature(void);
int request_motion(void);
void light_on(void);
void light_off(void);

#endif
