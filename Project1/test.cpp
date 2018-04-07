#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	syscall(337);
	long mul = syscall(338, 18, 116);
	long min = syscall(339, 18, 116);
	printf ("mul: %li, min: %li \n", mul, min);
	return 0;
}
