/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 *
 * Links to libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	/* Add code here -- put your names at the top. */
	unsigned int MAXLINE = 100;
	char buffer[MAXLINE];
	size_t start_time, duration;

	while(1)
	{
		start_time = time();
		int n = read(STDIN_FILENO, buffer, MAXLINE - 1);
		duration = time() - start_time;
		write(STDOUT_FILENO, buffer, n);
		int ms, seconds;
		seconds = duration / 1000;
		ms = duration % 1000;
		printf("Time taken: %d.%d \n", seconds, ms);
	}
	return 0;
}
