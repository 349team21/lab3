/** @file hello.c
 *
 * @brief Prints out Hello world using the syscall interface.
 *
 * Links to libc.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-10-29
 */
#include <unistd.h>

int main(int argc, char** argv)
{
	int x = 43;
	int i;
	char hello[] = "Hello World\r\n";
	while(1){
	for(i = 0; i<300000000; i++){
		x = 41 * x;
	}
	hello[0] = 'b';
	if((x % 4) < 2){
	hello[0] = 'a';
	}
	write(STDOUT_FILENO, hello, sizeof(hello) - 1); 
	}
	return 0;
}
