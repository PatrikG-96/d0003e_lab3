/*
 * D0003E_lab3.c
 *
 * Created: 2021-02-10 19:42:32
 * Author : shirt
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "tinythreads.h"
#include "utils.h"
#include "LCDDrivers.h"

void blink(int arg) {
	bool active = false;

	while(1)
	{
		if (get_counter() >= get_interrupt_cycle()/2)
		{
			reset_counter();
			active = !active;	// alternate state
			
			
			if (active)
			{
				setbits(&LCDDR3, 1, 0, 0x1);
			}
			else
			{
				setbits(&LCDDR3, 1, 0, 0x0);
			}
		}
	}
}

void button(int arg) {
	

	bool stick_active = false;
	long counter = 0;
	
	while(1) {
		int stick_down = joystick_down();
		
		// If joystick is currently in a down state and wasn't before
		if (stick_down == 0 && !stick_active)
		{
	
			stick_active = true;
			counter++;
		}

		// If stick is currently not in a down state
		else if (stick_down == 1){
			stick_active = false;
		}
		printAt(counter, 4);
	}
	
}

void primes(long i) {
	while (1)
	{
		if (is_prime(i))
		{
			
			printAt(i, 0);
		}
		i++;
	}
}

int is_prime(long i)
{
	long n = i-1;
	
	while (n>1)
	{
		if (i%n == 0)
		{
			return 0;
		}
		n--;
	}
	return 1;
}

void printAt(long num, int pos) {
	int pp = pos;
	writeChar( (num % 100) / 10 + '0', pp);
	pp++;
	writeChar( num % 10 + '0', pp);

}

int main(void)
{
    spawn(blink, 0);
	spawn(button, 0);
	primes(1);
}

