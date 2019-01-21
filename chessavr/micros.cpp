/*
 * micros.cpp
 *
 * Created: 09/08/2016 19:44:00
 *  Author: Muhammed Zia Dawood
 */ 
#include "micros.h"


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define sbi(port,bit)  (port) |= (1 << (bit))
#define cbi(port,bit)  (port) &= ~(1 << (bit))

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>


#define sbi(port,bit)  (port) |= (1 << (bit))
#define cbi(port,bit)  (port) &= ~(1 << (bit))

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

unsigned long micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
	
	cli();
	m = timer0_overflow_count;
	t = TCNT0;

	if ((TIFR & _BV(TOV0)) && (t & 255))
	m++;

	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

SIGNAL(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;
	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}
	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}

void initTimer0(void)
{
	sbi(TCCR0, CS01);
	sbi(TCCR0, CS00);
	// enable overflow interrupt
	
	TCNT0 =0;
	TIMSK |= (1 << TOIE0);
	
	sei();
	timer0_overflow_count = 0;
}
