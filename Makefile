	
watchController: watchController.o mes_dtw.o 
	gcc -o watchController watchController.o mes_dtw.o -lm

watchController.o: watchController.c gesture_averaged_int.h 
	gcc -c watchController.c -I ./

mes_dtw.o: mes_dtw.c gesture_averaged_int.h
	gcc -c mes_dtw.c -I ./

clean:
	rm mes_dtw.o watchController.o watchController	
	
