/*
 * micros.h
 *
 * Created: 09/08/2016 19:44:31
 *  Author: Muhammed Zia Dawood
 */ 


#ifndef MICROS_H_
#define MICROS_H_

#define F_CPU 16000000UL


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
//#include "uart.h"

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


#ifndef SREG
#  if __AVR_ARCH__ >= 100
#    define SREG _SFR_MEM8(0x3F)
#  else
#    define SREG _SFR_IO8(0x3F)
#  endif
#endif

#define sbi(port,bit)  (port) |= (1 << (bit))
#define cbi(port,bit)  (port) &= ~(1 << (bit))


#define _BV(bit)(1 << (bit))



unsigned long millis(void);
unsigned long micros(void);
void initTimer0(void);
SIGNAL(TIMER0_OVF_vect);


#endif /* MICROS_H_ */