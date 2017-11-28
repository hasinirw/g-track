#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h> 
#include <string.h>
#include <stdint.h> //int8_t

#include <unistd.h> //sleep
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h> // serial ports

/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE 115200         
/* change this definition for the correct port */
#define DEVICEPORT "/dev/ttyACM0"



int main(int argc, char **argv) 
{
	char command[128];
	char response[2560];
	char start[3];
	char dataRequest[7];
	int cFlag = 0;
	int r, c;

	int8_t x=0, y=0, z=0, u=-1;
	int8_t filter_x=0, filter_y=0, filter_z=0;

	start[0] = 0xFF;
	start[1] = 0x07;
	start[2] = 0x03; 

	dataRequest[0] = 0xFF;
	dataRequest[1] = 0x08;
	dataRequest[2] = 0x07; 
	dataRequest[3] = 0x00;
	dataRequest[4] = 0x00; 	
	dataRequest[5] = 0x00;
	dataRequest[6] = 0x00; 
	
	// open port for watch RF Communication
	struct termios  config;
	int wFile;
	wFile = open(DEVICEPORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if(wFile == -1)
	{
		printf( "failed to open port\n" );
		return -1;
	}


	bzero(&config, sizeof(config));
	config.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	config.c_oflag = 0;
	tcflush(wFile, TCIFLUSH);
	tcsetattr(wFile,TCSANOW,&config);

	//start sequence: 0xFF 0x07 0x03;
	write(wFile,start,sizeof(start));
	printf("press start button on watch now!!!!\n");	
	sleep(1);
	printf("hope you already pressed button!\n");

	int d = 0;
	int sampleCounter = 0;
	int reqCounter = 0;
	
	while(1)
	{
		
		usleep(25000);
		strcpy(response,"");

		//data request sequence: 0xFF 0x08 0x07 0x00 0x00 0x00 0x00;
		write(wFile,dataRequest,sizeof(dataRequest));
		//printf("\nRequest sent!\n");
	
		read(wFile,response,255);		
	        u = response[3];
		x = response[4];
		y = response[5];
		z = response[6];

		if (u ==1)
		{		
		
			reqCounter++;
			
			if(reqCounter >=100){
			    return 0;
		        }
		        
		}
		else{
		       // if((reqCounter>0) && (u!=1) && (u!=0)) break;
		}
		printf("%d,%d,%hhd,%hhd,%hhd,%hhd\n",sampleCounter,reqCounter,x,y,z,u);
		sampleCounter++;
		

	}


	close(wFile);
	
	return 0;
}
