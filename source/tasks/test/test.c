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

const char pass[] = "pass!\r\n";
const char fail[] = "fail... \r\n";
const char begin1[] = "begin testing1 ... \r\n";
const char begin2[] = "begin testing2 ... \r\n";
const char begin3[] = "begin testing3 ... \r\n";
const char begin4[] = "begin testing4 ... \r\n";
const char begin5[] = "begin testing5 ... \r\n";
const char done[] = "done!\r\n";

int main(int argc, char** argv){
	
	write(STDOUT_FILENO, begin1, sizeof(begin1) - 1); 
	test1();
	
	/*
	write(STDOUT_FILENO, begin2, sizeof(begin2) - 1);
	test2();
	*/
	
	write(STDOUT_FILENO, begin3, sizeof(begin3) - 1);
	test3();
	
	write(STDOUT_FILENO, begin4, sizeof(begin4) - 1);
	test4();
	
	write(STDOUT_FILENO, begin5, sizeof(begin5) - 1);
	test5();
	
	write(STDOUT_FILENO, done, sizeof(done) - 1);
	return 0;
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
	for(i = 0; i < 1000; i++){
		cur_time = time();
		if (prev_time > cur_time) {
			write(STDOUT_FILENO, fail, sizeof(fail) - 1); 
			return;
		}
		prev_time = cur_time;
	}
	write(STDOUT_FILENO, pass, sizeof(pass) - 1); 
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
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
			return;
		}
		
		if (duration > 12){
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
			return;
		}
	}
	write(STDOUT_FILENO, pass, sizeof(pass) - 1);
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
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
		//	printTime(duration);
			return;
		}
		if (duration >= 20){
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
		//	printTime(duration);
			return;
		}
	}
	write(STDOUT_FILENO, pass, sizeof(pass) - 1);
}



/*
 	call sleep with increasing time and print out time difference
 	to show it is constantly increasing also
*/
void test4(void) {
	size_t  prev_duration = 0;
	size_t  duration = 0;
	size_t  temp;
	int i;
	for(i = 10; i < 300; i += 10){
		temp = time();
		sleep(i);
		duration = time() - temp;

		if (duration < prev_duration){
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
			printf("i=%d, duration=%lu, prev=%lu\n",i, duration, prev_duration);
			return;
		}
		prev_duration = duration;
	}	
	write(STDOUT_FILENO, pass, sizeof(pass) - 1);
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
		
		if (duration > 10) {
			write(STDOUT_FILENO, fail, sizeof(fail) - 1);
			return;
		}
	}
	write(STDOUT_FILENO, pass, sizeof(pass) - 1);
	return;
}
