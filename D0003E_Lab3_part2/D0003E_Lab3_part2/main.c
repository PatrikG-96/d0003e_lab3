/*
 * D0003E_Lab3_part2.c
 *
 * Created: 2021-02-13 15:40:29
 * Author : shirt
 */ 

#include <avr/io.h>
#include "tinythreads.h"
#include <stdbool.h>
#include "LCDDrivers.h"



void init() {
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	initLCD();
	
	// Enable joystick
	PORTB = PORTB | (1<<7);
	
	EIMSK = (1<<7);
	PCMSK1 = (1<<7);
	
	// Enable OC1A pin
	PORTB = PORTB | (1<<5);
	
	// Timer initialization, set prescaler to 1024 and turn on CTC
	TCCR1B = (1<<CS12) | (1<<CS10)  | (1<<WGM12);
	
	// OC1A set on compare match
	TCCR1A = (1<<COM1A1) | (1<<COM1A0);
	
	// Enable out compare A match interrupt
	TIMSK1 |= (1<<OCIE1A);
	
	OCR1A = 8000000/1024/2;
	
	TCNT1 = 0;
}

void blink(int arg) {
	bool active = false;

	while(1)
	{

		active = !active;	// alternate state
					
		if (active)
		{
			setbits(&LCDDR3, 1, 0, 0x1);
		}
		else
		{
			setbits(&LCDDR3, 1, 0, 0x0);
		}
		lock(get_blink_mutex());
	}
}

void button(int arg) {
	
	long btn_count = 0;
	
	while(1) {
	
		printAt(btn_count, 4);
		btn_count++;
		lock(get_button_mutex());
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
	return 0;
	init();
	
	spawn(blink,0);
	spawn(button,0);
	primes(1);
}

