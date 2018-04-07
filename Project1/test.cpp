#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	syscall(337);

	bool correct = true;
	for (long i = 0; i < 100; ++i) {
		for (long j = 0; j < 100; ++j) {
			long mul = syscall(338, i, j);
			long min = syscall(339, i, j);

			long cMul = i * j;
			long cMin = (i < j) ? i : j;

			if (mul != cMul || min != cMin) {
				correct = false;
				break;
			}
		}
	}
	if (correct)
		printf ("10000 tests of mul, min correct.\n");
	return 0;
}
