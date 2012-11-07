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
void test6(void);
void typoTest(void);
void splatTest(void);

int main(int argc, char** argv){
	
	splatTest();
	
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	
	typoTest();
	return 0;
}


// print out time in seconds and milliseconds
 
void printTime(size_t duration){
	size_t ms, seconds;
	ms = duration % 1000;
	seconds = (duration - ms) / 1000;
	printf("duration is %lu ," , duration);
	printf("time is %lu.%3lu\n", seconds, ms);
	return;
}


void splatTest(void) {
	printf("begin splat test ... \n");
	const char* cursor[4] = {"\b \b|","\b \b/","\b \b-","\b \b\\"};

	int counter = 0;
	write(1, " ", 1);
	while(counter < 100)
	{
		write(1, cursor[counter % 4], 4);
		counter ++;
		sleep(200);
	}
	printf("end splat test! \n");
	return;
}

void typoTest(void) {

	printf("begin typo test ... \n");
	size_t MAXLINE = 100;
	char buffer[MAXLINE];
	volatile size_t start_time;
	volatile size_t end_time;
	size_t duration;

	while(1)
	{
		start_time = time();
		int n = read(STDIN_FILENO, buffer, MAXLINE - 1);
		end_time = time();
		duration = end_time - start_time;
		write(STDOUT_FILENO, buffer, n);
		int ms, seconds;
		ms = duration % 1000;
		seconds = (duration - ms)/1000;
		printTime(duration);
	}
}


/*
	call time function 1000 times consecutively	
	make sure the new time is always greater than the old time
*/
void test1(void) {
	printf("starting test1() \n");
	size_t  prev_time;
	size_t  cur_time;
	prev_time = time();
	int i;
	for(i = 0; i <1000; i++){
		cur_time = time();
		printf("Testing at i : %d, cur time is : %lu \n", i, cur_time);
		if (prev_time > cur_time) {
			printf("Error in test1 at i = %d \n", i);
			printf("prev time is %lu, cur time is %lu", prev_time, cur_time);
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
printf("starting test2() \n");
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
		printf("Measured time at iteration %d:", i);
		printTime(duration);
	}
printf("end of test2() \n");
}


//	calls the sleep(10) 100 times to makes sure
void test3(void) {
printf("starting test3() \n");
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
		printf("At iteration %d, measured duration time:", i);
		printTime(duration);
	}
	printf("test3 passed \n");
}



/*
 	call sleep with increasing time and print out time difference
 	to show it is constantly increasing also
*/
void test4(void) {
printf("starting test4() \n");
	size_t  prev_time = 0;
	size_t  duration = 0;
	size_t  temp;
	int i;
	for (i = 10; i < 300; i += 10) {
		temp = time();
		sleep(i);
		duration = time() - temp;

		if (duration < prev_time) {
			printf("Error in test4 \n");
			printf("Measured prev time:");
			printTime(prev_time);
			printf("Measured duration time:");
			printTime(duration);
			return;
		}
		prev_time = duration;
		printf("i is %d, measured duration time:", i);
		printTime(duration);
	}	
	printf("test4 passed \n");
}


/*
	call sleep(0) 100 times and make sure the time elapse is close to zero
*/
void test5 (void) {
printf("starting test5() \n");
	size_t prev_time;
	size_t duration;
	int i;
	for (i = 0; i < 100; i ++) {
		prev_time = time();
		sleep(0);
		duration = time() - prev_time;
		
		printf("Iteration %d, Time should be close to 0 ms\n", i);
		printf("Measured duration:");
		printTime(duration);
	}
	printf("end of test 5()\n");
	return;
}


void test6(void) {
	size_t  prev_time;
	size_t  duration;
	size_t  sum = 0;
	
	int i = 0;
		
	for(i = 0; i < 10; i++){
		prev_time = time();	
		sleep(1000);	
		duration = time() - prev_time;
		printf("Iteration %d, Time should be close to 1s \n", i);
		printf("Measured duration time:");
		printTime(duration);	
		sum = sum + duration;
	}	
	sum = sum / 10;
	
	if (sum < 1000) {
		printf("Error in test6()");
	}

	printf("Expected average time: 1s \n");
	printf("Measured average time:");
	printTime(sum);
	printf("end of test 6()\n");
	return;
}
