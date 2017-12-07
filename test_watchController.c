#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> //int8_t
#include <math.h>

#include <gesture_averaged_int.h>

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

	static int no_of_gestures=8;
    int distances1[] ={0,0,0,0,0,0,0,0};
    int xsize1= sizeof gesture1/ sizeof gesture1[0];
    int xsize2= sizeof gesture2/ sizeof gesture2[0];
    int xsize3= sizeof gesture3/ sizeof gesture3[0];
    int xsize4= sizeof gesture4/ sizeof gesture4[0];
    int xsize5= sizeof gesture5/ sizeof gesture5[0];
    int xsize6= sizeof gesture6/ sizeof gesture6[0];
    int xsize7= sizeof gesture7/ sizeof gesture7[0];
    int xsize8= sizeof gesture8/ sizeof gesture8[0];
    

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
    int templateg[reqCounter][3];
	for(n=0;n<reqCounter;n++) {
		 templateg[reqCounter][0] =rms_array[reqCounter][0];
	     templateg[reqCounter][1] =rms_array[reqCounter][1];
		 templateg[reqCounter][2] =rms_array[reqCounter][2];
		 	 
	}

	int ysize= sizeof templateg/ sizeof templateg[0];
         
    distances1[0] =mes_dtw(gesture1,templateg,xsize1,ysize,3);
    distances1[1] =mes_dtw(gesture2,templateg,xsize2,ysize,3);
    distances1[2] =mes_dtw(gesture3,templateg,xsize3,ysize,3);
    distances1[3] =mes_dtw(gesture4,templateg,xsize4,ysize,3);
    distances1[4] =mes_dtw(gesture5,templateg,xsize5,ysize,3);
    distances1[5] =mes_dtw(gesture6,templateg,xsize6,ysize,3);
    distances1[6] =mes_dtw(gesture7,templateg,xsize7,ysize,3);
    distances1[7] =mes_dtw(gesture8,templateg,xsize8,ysize,3);
    //distances1[8] =mes_dtw(gesture9,templateg,xsize9,ysize,3);
    int i;
    for(i =0; i<sizeof(distances1)/sizeof(distances1[0]) ; i++) {fprintf(stdout,"distances %d : %d\n", i,distances1[i]);}
     
	
    if(reqCounter< 30) {printf("not enough data points!Please redo gesture!\n");}
	else{
         for(n=0;n<reqCounter;n++) {
		 printf("%d\n",rms_array[n]);
	}
    }
        //fclose(fFile);  
  	close(wFile);
	
	return 0;
}