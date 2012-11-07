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
	volatile size_t start_time;
	volatile size_t duration;
	int ms;
	int seconds;

	while(1)
	{
		start_time = time();
		int n = read(STDIN_FILENO, buffer, MAXLINE - 1);
		duration = time() - start_time;
		write(STDOUT_FILENO, buffer, n);
		ms = duration % 1000;
		seconds = (duration - ms) / 1000;
		printf("Time taken: %d.%d (%lu)\n", seconds, ms, duration);
	}
	return 0;
}
