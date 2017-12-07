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
	int start_recording =0;
	int rms_array[200][3];

	int8_t x=0, y=0, z=0, u=0;
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
	//printf("press start button on watch now!!!!\n");	
	//sleep(1);
	//printf("hope you already pressed button!\n");

	int d = 0;
	int sampleCounter = 0;
	int reqCounter = 0;
	int minus_counter=0;
	int plus_counter=0;
	
	
	while(1)
	{
		
		usleep(25000); //assuming sampling rate is 33Hz
		strcpy(response,"");

		//data request sequence: 0xFF 0x08 0x07 0x00 0x00 0x00 0x00;
		write(wFile,dataRequest,sizeof(dataRequest));
		//printf("\nRequest sent!\n");
	
		read(wFile,response,255);		
	        u = response[3];
		x = response[4];
		y = response[5];
		z = response[6];

		
		
		if (u ==1){
		        if(minus_counter>=0) minus_counter=0;
			start_recording =1;
		}	
			
			
		if(start_recording==1){		
		        
			if(u==-1){
		           minus_counter++;
		           if(minus_counter>=5){
		           	printf("button press case: samples collected: %d\n",reqCounter);
		           	start_recording=0;
		            break;
		           }
		        }		
			
		     
		       if(u==1){
		       
		       rms_array[reqCounter][0]= x;		       
		       rms_array[reqCounter][1]= y;
		       rms_array[reqCounter][2]= z;
		       reqCounter++;
		       }
		       
		       if(reqCounter >=100){
			    printf("end case: samples collected: %d\n",reqCounter);
			    start_recording =0;
			    break;
		       }
		       
		           
		}	
			
		//printf("%d,%d,%d,%hhd,%hhd,%hhd,%hhd\n",sampleCounter,reqCounter,start_recording,x,y,z,u);
		sampleCounter++;
		

	}
        
        //FILE *fFile = fopen("data.txt", "w");
	int n;

	// for(n=0;n<reqCounter;n++) {
	//	 fprintf(fFile,"%d,%d\n",n ,rms_array[n]);
	// }
	
	 if(reqCounter< 30) {printf("not enough data points!Please redo gesture!\n");}
	 else{
	 	 printf("{");
          for(n=0;n<reqCounter;n++) {
		      printf("{%d,%d,%d},\n",rms_array[n][0],rms_array[n][1],rms_array[n][2]);
	      }
	      printf("};\n");
          }
        //fclose(fFile);  
  
	close(wFile);
	
	return 0;
}