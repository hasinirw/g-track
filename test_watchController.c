#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> //int8_t
#include <math.h>

#include "templates.h"

#include <unistd.h> //sleep
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h> // serial ports



/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE 115200         
/* change this definition for the correct port */
#define DEVICEPORT "/dev/ttyACM0"


int mes_dtw(int (*drg)[3], int (*drg1)[3],int n, int m, int dim);
int distanceBetween(int p1, int p2);
int fimin(int x,int y);

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
	
	//write to buffer
	static int pFile;
	pFile =open("/dev/gesture",O_RDWR);
	if(pFile<0){
	    printf("gesture module not loaded\n");
	    return -1;
	    }
	
	char buff[5];
	memset(buff,0,5);
	
	
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
	
while(1){
d = 0;
sampleCounter = 0;
reqCounter = 0;
minus_counter=0;
plus_counter=0;
	
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
		       printf("%d %d %d\n", rms_array[reqCounter][0],rms_array[reqCounter][1],rms_array[reqCounter][2]);

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
		// printf("test %d ,%d, %d\n",  templateg[n][0], templateg[n][1],  templateg[n][2]); 
	}
	
       
        
        
	//int ysize= sizeof templateg/ sizeof templateg[0];
	int ysize= reqCounter;
         
    distances1[0] =mes_dtw(gesture1,rms_array,xsize1,ysize,3);
    distances1[1] =mes_dtw(gesture2,rms_array,xsize2,ysize,3);
    distances1[2] =mes_dtw(gesture3,rms_array,xsize3,ysize,3);
    distances1[3] =mes_dtw(gesture4,rms_array,xsize4,ysize,3);
    distances1[4] =mes_dtw(gesture5,rms_array,xsize5,ysize,3);
    distances1[5] =mes_dtw(gesture6,rms_array,xsize6,ysize,3);
    distances1[6] =mes_dtw(gesture7,rms_array,xsize7,ysize,3);
    distances1[7] =mes_dtw(gesture8,rms_array,xsize8,ysize,3);
    //distances1[8] =mes_dtw(gesture9,templateg,xsize9,ysize,3);
    int i;
    
    int gesture_selected =1;
    
    int mindis=distances1[0];
    
    for(i =0; i<sizeof(distances1)/sizeof(distances1[0]) ; i++) {
      
     if(distances1[i]<mindis) {
         gesture_selected = i+1;
         mindis=distances1[i];
         // printf("gtest: %d %d\n",i+1, mindis);
         }
         
     
         
   //  fprintf(stdout,"distances %d : %d\n", i+1,distances1[i]);
     }
     
     printf("gesture_selected: %d\n",gesture_selected);
	
    if(reqCounter< 30) {printf("not enough data points!Please redo gesture!\n");}
    else{
        sprintf(buff,"%d",gesture_selected);
	printf("buff=%s\n",buff);
        write(pFile,buff,5);
	
    }

        
}  // end of outer while loop      
  	close(wFile);
	close(pFile);
	
	return 0;
}  // end of main




    int mes_dtw(int (*drg)[3], int (*drg1)[3],int n, int m, int dim) {

        int n1 =n;
        int n2 = m;
        int x;
        int y;
      
        int *lastRow = (int *)malloc(sizeof(int) * n );
        int *curRow = (int *)malloc(sizeof(int) * n );
        int *temp = (int *)malloc(sizeof(int) * n );
        int minCost=0;

        
	    curRow[0] = sqrt(distanceBetween(drg[0][0], drg1[0][0])+distanceBetween(drg[0][1], drg1[0][1])+distanceBetween(drg[0][2], drg1[0][2]));

	    for (x = 1; x < n1; x++) {
	    	   
		      curRow[x] = curRow[x-1] + sqrt(distanceBetween(drg[x][0], drg1[0][0])+distanceBetween(drg[x][1], drg1[0][1])+distanceBetween(drg[x][2], drg1[0][2]));
	    }
        
        for (y = 1; y < n2; y++) {
            temp = curRow;
            curRow = lastRow;
            lastRow = temp;

		    curRow[0] = lastRow[0] + sqrt(distanceBetween(drg[0][0], drg1[y][0])+distanceBetween(drg[0][1], drg1[y][1])+distanceBetween(drg[0][2], drg1[y][2]));
		    for (x = 1; x < n1; x++ ){
			    minCost = fimin(curRow[x-1], fimin(lastRow[x], lastRow[x-1]));
			    curRow[x] = minCost + sqrt(distanceBetween(drg[x][0], drg1[y][0])+distanceBetween(drg[x][1], drg1[y][1])+distanceBetween(drg[x][2], drg1[y][2]));
		    }
	    }
        return curRow[n1-1];
	}	

	int distanceBetween(int p1, int p2) {
		return (p1 - p2) * (p1 - p2);
	}


      
	int fimin(int x,int y){
	     int k;
	     if(x<y) k=x;
	     else k=y;
	     
	     return k;
	}
	
