
#include <stdbool.h>
#include <avr/io.h>
#include "LCDDrivers.h"

bool active = false;
long btn_count = 0;

void blink(int arg) {

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

void button(int arg) {
	
	printAt(btn_count, 4);
	btn_count++;

	
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