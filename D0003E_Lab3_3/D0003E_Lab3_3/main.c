
#include "tinythreads.h"
#include "functions.h"



int main(void)
{
	init();
    spawn(blink, 0);
	spawn(button, 0);
	primes(1);
}

