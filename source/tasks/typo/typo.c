/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 *
 * Links to libc.
 */
#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	/* Add code here -- put your names at the top. */
	unsigned int MAXLINE = 100;
	char buffer[MAXLINE];
	size_t start_time, duration;
	//const char* timeString = "Time taken: ";
	//char temp[10];

	while(1)
	{
		start_time = time();
		int n = read(STDIN_FILENO, buffer, MAXLINE - 1);
		duration = time() - start_time;
		write(STDOUT_FILENO, buffer, n);
		int ms, seconds;
		seconds = duration / 1000;
		ms = duration % 1000;
	/*	write(1, timeString, 12);
		snprintf(temp, 10, "%d", seconds);
		int i = 0;
		while(temp[i] != '\0')
		write(1, &temp[i++], 1);
		write(1, ".",1);
		snprintf(temp, 10, "%d", ms);
		i = 0;
		while(temp[i] != '\0')
		write(1, &temp[i++], 1);
		write(1, "\n", 1);*/
		printf("Time taken: %i.%i \n", seconds, ms);
	}
	return 0;
}
