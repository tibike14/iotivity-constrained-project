
#ifndef SERIAL_READ_MOTION_H
#define SERIAL_READ_MOTION_H


int serialOpen (const char *device, const int baud);

int serialDataAvail (const int fd);

int motion_read(void);


#endif 
