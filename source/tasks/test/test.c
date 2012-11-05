// test.c

// Extra Credit test cases

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);

int main(int argc, char** argv){
	test1();
	test2();
	test3();
	test4();
	test5();
	return 0;
}

// print out time in seconds and milliseconds
 
void printTime(size_t duration){
	size_t ms, seconds;
	ms = duration % 1000;
	seconds = (duration - ms) / 1000;
	printf("time is %ld.%3ld\n", seconds, ms);
	return;
}


/*
	call time function 1000 times consecutively	
	make sure the new time is always greater than the old time
*/
void test1(void) {
	size_t  prev_time;
	size_t  cur_time;
	prev_time = time();
	int i;
	for(i = 0; i <1000; i++){
		cur_time = time();
		printf("Testing at i : %d \n", i);
		if (prev_time > cur_time) {
			printf("Error in test1 at i = %d \n",i);
			return;
		}
		prev_time = cur_time;
	}
	printf("test1 passed \n");
}




/*
	calls the sleep(1) function 100 times in a row and measure the elapsed time.
	The result should be exactly 10ms. Repeat the test for 10 times to show its
	consistency.
*/
void test2(void) {
	size_t prev_time;
	size_t duration;
	int i;
	int j;
	for (i = 0; i < 100; i ++) {
		prev_time = time();
		for (j = 0; j < 10; j++) {
			sleep(1);
		}	
		duration = time() - prev_time;
		
		if (duration < 10){
			printf("Error in test2 \n");
			return;
		}
		printf("Time should be 10 ms\n");
		printf("Measured time:");
		printTime(duration);
	}
}


//	calls the sleep(10) 100 times to makes sure
void test3(void) {
	size_t prev_time;
	size_t duration;
	int i;
	for (i = 0; i < 100; i++){
		prev_time = time();
		sleep(10);
		duration = time() - prev_time;

		if (duration < 10){
			printf("Error in test3 at i : %d \n", i);
			printf("Measured duration time:");
			printTime(duration);
			return;
		}
		printTime(duration);
	}
	printf("test3 passed \n");
}



/*
 	call sleep with increasing time and print out time difference
 	to show it is constantly increasing also
*/
void test4(void) {
	size_t  prev_time = 0;
	size_t  duration = 0;
	size_t  temp;
	int i;
	for(i = 10; i < 300; i += 10){
		temp = time();
		sleep(i);
		duration = time() - temp;

		if(duration < prev_time){
			printf("Error in test4 \n");
			printf("Measured prev time:");
			printTime(prev_time);
			printf("Measured duration time:");
			printTime(duration);
			return;
		}
		prev_time = duration;
		printTime(duration);
	}	
	printf("test4 passed \n");
}


/*
	call sleep(0) 100 times and make sure the time elapse is close to zero
*/
void test5 (void) {
	size_t prev_time;
	size_t duration;
	int i;
	for (i = 0; i < 100; i ++) {
		prev_time = time();
		sleep(0);
		duration = time() - prev_time;
		
		printf("Time should be close to 0 ms\n");
		printf("Measured duration:");
		printTime(duration);
	}
	return;
}
