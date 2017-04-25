
#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#define BUFSIZE 256

#define HeatPin1 7
#define HeatPin2 2
#define HeatPin3 3
#define CoolPin 0

void init(void);
void heat_on(void);
void heat_off(void);
void cool_on(void);

#endif
