#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "serial_read_temp.h" 


int serialOpen (const char *device, const int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:        myBaud =     B50 ; break ;
    case     75:        myBaud =     B75 ; break ;
    case    110:        myBaud =    B110 ; break ;
    case    134:        myBaud =    B134 ; break ;
    case    150:        myBaud =    B150 ; break ;
    case    200:        myBaud =    B200 ; break ;
    case    300:        myBaud =    B300 ; break ;
    case    600:        myBaud =    B600 ; break ;
    case   1200:        myBaud =   B1200 ; break ;
    case   1800:        myBaud =   B1800 ; break ;
    case   2400:        myBaud =   B2400 ; break ;
    case   4800:        myBaud =   B4800 ; break ;
    case   9600:        myBaud =   B9600 ; break ;
    case  19200:        myBaud =  B19200 ; break ;
    case  38400:        myBaud =  B38400 ; break ;
    case  57600:        myBaud =  B57600 ; break ;
    case 115200:        myBaud = B115200 ; break ;
    case 230400:        myBaud = B230400 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
   return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;
	options.c_cflag &= ~HUPCL;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;        // Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;      // 10mS

  return fd ;
}

int serialDataAvail (const int fd)
{
  int result ;

  if (ioctl (fd, FIONREAD, &result) == -1)
    return -1 ;

  return result ;
}


float serialReadTemp(void)
{
    int fd;
    int n;
    int i;
    int buf[128];
	int nw;
	int adc_value;

    if ((fd = serialOpen ("/dev/ttyUSB1", 115200)) < 0)
    {
	    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }

    tcflush(fd, TCIFLUSH);

    nw = write(fd, "r", 1);
	printf("number of writes: %d\n", nw); 

	usleep(3000);

    n = read(fd, buf, 4);

	adc_value = atoi(buf);
	printf("the ADC value of temp: %d\n", adc_value);
	
	float voltage = adc_value / 204.8;
	printf("votlage: %.2f\n", voltage);	

	float temperatureC = (voltage - 0.5) * 100 ; // Converting to Celsius
	printf("Temperature: %.2f\n", temperatureC);	


	close(fd);

	return temperatureC;

}

