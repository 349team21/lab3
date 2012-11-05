/** @file splat.c
 *
 * @brief Displays a spinning cursor.
 *
 * Links to libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	/* Add code here -- put your names at the top. */
	const char* cursor[4] = {"\b \b|","\b \b/","\b \b-","\b \b\\"};

	int counter = 0;
	write(1, " ", 1);
	while(1)
	{
		write(1, cursor[counter % 4], 4);
		counter ++;
		sleep(200);
	}	
	return 0;
}

