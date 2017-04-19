


#ifndef SERIAL_READ_TEMP_H
#define SERIAL_READ_TEMP_H


int serialOpen (const char *device, const int baud);

int serialDataAvail (const int fd);

float serialReadTemp(void);


#endif 
