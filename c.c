#include<stdio.h>
int main(void) 
{
    long str_len = 15;
    const char* str = "hello world!\n\r";
    
	asm volatile(
	    "movl $4, %%eax\n\t"
		"movl $1, %%ebx\n\t"
		"movq %0, %%rcx\n\t"
		"movq %1, %%rdx\n\t"
		"int	  $0x80\n\t"
		:		
		:"r"(str), "r"(str_len)
		:"eax","ebx", "rcx", "rdx"
	);
	return 0;
}
