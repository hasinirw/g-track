#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gesture_averaged_int.h>

int mes_dtw(int (*drg)[3], int (*drg1)[3],int n, int m, int dim);
int distanceBetween(int p1, int p2);


   int main(void){

          
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
         //int xsize9= sizeof gesture9/ sizeof gesture9[0];
         
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
       
         return 0;
   }


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
			    minCost = fmin(curRow[x-1], fmin(lastRow[x], lastRow[x-1]));
			    curRow[x] = minCost + sqrt(distanceBetween(drg[x][0], drg1[y][0])+distanceBetween(drg[x][1], drg1[y][1])+distanceBetween(drg[x][2], drg1[y][2]));
		    }
	    }
        return curRow[n1-1];
	}	

	int distanceBetween(int p1, int p2) {
		return (p1 - p2) * (p1 - p2);
	}



	
	
