#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage int sys_hello(void)
{
	printk("HELLO SYSTEM CALL B03901018 B03901116\n");
	return 0;
}

asmlinkage long sys_multiply(long a, long b)
{
	return a * b;
}

asmlinkage long sys_min(long a, long b)
{
	if (a < b) {
		return a;
	}
	return b;
}
