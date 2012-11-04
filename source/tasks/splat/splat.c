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
	const char cursor[4] = "|/-\\";

	int counter = 0;
	printf("start! \n");
	printf(" ");
	while(1)
	{
		printf("\b\b%c\r", cursor[counter % 4]);
		counter ++;
		int i = 0;
		int sum = 0;
		while (i < 100000) {
			sum = sum + i;
			i += sum;
		}
	}	
	return 0;
}