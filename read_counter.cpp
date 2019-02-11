#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int main(){
	
	// opens serial port to device, sets read only, prevents assigment of tty as controlling terminal (ie replace user terminal)
	int fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY);
	
	if(fd == -1){
		perror("Error!");
	}
	else{
		printf("gieger counter streaming... \n");
	}
	
	struct termios g_counter;
	tcgetattr(fd, &g_counter);
	// input flags: default canonical input (processes data in line ending with \n, turns off software based flow control
	g_counter.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY);
	// output flags: none = no interference
	g_counter.c_oflag = 0;  	
	// control flags 
	// no parity, stop bit=1, clear mask 
	g_counter.c_cflag &= ~(PARENB | CSTOPB | CSIZE); 
	// sets data bits 
	g_counter.c_cflag |= CS8; 
	// turn off hardware based flow control
	g_counter.c_cflag &= ~CRTSCTS; 
	// turn on reciever for reading 
	g_counter.c_cflag |= CREAD | CLOCAL; 
	// local flags = none 
	g_counter.c_lflag = 0;
	// read infinite characters
	g_counter.c_cc[VMIN] = 0;		
	// wait indefinitely
	g_counter.c_cc[VTIME] = 0;	
	// set baud rate to 9600 
	cfsetispeed(&g_counter, B9600);
	cfsetospeed(&g_counter, B9600);
	
	// set attributes of struct for geiger counter
	tcsetattr(fd, TCSANOW, &g_counter);
	// check point 
	if(tcsetattr(fd, TCSANOW, &g_counter) != 0){
		perror("error setting attributes");
	}
		
	// NL is new line 
	// CR is carriage return 
	
	int i = 0, pulse = 0;
	char rad_bytes[10];
	
	time_t timet;
	int start = time(&timet);
	int cur = start, diff = 1;
	float rate, rate_per_min, microSievert, microRem;

	while(1){	
	
		cur = time(&timet);
		
		// read designated serial port and store n pulses of fd in rad_bytes
		// pulses correlate to reaction in geiger tube
		// declares reference to rad_bytes for future use
	
		pulse = read(fd, &rad_bytes, 10); 
		// *rad_bytes points to incoming bytes
		//char bytes = *rad_bytes;
		if(pulse > 0){		
			i++;
				
			// need to cast ints to float to get precision, as long as one value is float, the result is float
			//when cur is called, a new start time is taken, and the difference between that and the initial start time is taken
			//This resets the clock on first iteration and then counts indefinitely
			
			diff = cur-start; 		
			rate = float(i)/diff; 
			rate_per_min = rate*60;
			
			//120 CPM = 1 uSv/hr
			microSievert = rate_per_min*1/120; 
			//1 Sv = 100 rem
			microRem = microSievert*100;
			
			printf("pulse rate = %.4f CPM \n\n", rate_per_min);	
			printf("exposure per hour = %.4f uSv, %.4f uRem \n\n", microSievert, microRem);	
		}
	}

	close(fd);
	return 0;
}





