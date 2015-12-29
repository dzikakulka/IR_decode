#include <avr/io.h>
#include <avr/interrupt.h>

#include "ir_decode.h"
#include "../makrau.h"

// 16 bit variable for storing incoming data
volatile uint16_t irData;

// Succesful transmission decoding flag
// MUST BE CLEARED TO DECODE ANOTHER TRANSMISSION
// Immediately after it is set, recieved command is available at irData variable
volatile uint8_t IR_cmd_pending;

void ir_init() {
	DDR(IR_PORT) &= ~IR_IN;		// IR pin as input
	PORT(IR_PORT) |= IR_IN;		// IR pin pull-up

	// TIMER1 PRESCALER SETTINGS
	#if TIMER1_PRESCALER == 1
		TCCR1B |= (1<<CS10);
		TCCR1B &= ~((1<<CS11) | 1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 8
		TCCR1B |= (1<<CS11);
		TCCR1B &= ~((1<<CS10) | 1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 64
	  	TCCR1B |= (1<<CS11)|(1<<CS10);
	  	TCCR1B &= ~(1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 256
		TCCR1B |= (1<<CS12);
		TCCR1B &= ~((1<<CS10) | 1<<CS11);
	#endif

	TIMSK |= (1<<TICIE1);        // Interrupt at ICP pin
	TCCR1B &= ~(1<<ICES1);      // Interrupt at falling slope

	IR_cmd_pending = 0;	  // Clear pending cmd flag
}


// INTERRUPT HANDLING
// Triggered on 36kHz IR signal detection
ISR(TIMER1_CAPT_vect) {

#define FRAME_STARTED 1
#define PRE_OK 2
#define FRAME_RESTART 0


	static uint16_t LastCapture;
	uint32_t PulseWidth;

	static uint16_t irPre = IR_PRE;

	static uint8_t frame_status = 0;
	static uint8_t bitCount=0;

	PulseWidth = ICR1 - LastCapture;
	LastCapture = ICR1;

	if(!IR_cmd_pending)
	{
		if(!frame_status && HDR(PulseWidth))
		{
			frame_status = FRAME_STARTED; bitCount=0;
		}
		else if(frame_status == FRAME_STARTED)
		{
			if(HGH(PulseWidth))
			{
				irData |= (1 << (15-bitCount)); bitCount++;
			}
			else if(LOW(PulseWidth))
			{
				irData &= ~(1 << (15-bitCount)); bitCount++;
			}
			else frame_status = FRAME_RESTART;
			if(bitCount == 16)
			{
				if(irData == irPre) frame_status = PRE_OK;
				else frame_status = FRAME_RESTART;
				bitCount=0;
			}
		}
		else if(frame_status == PRE_OK)
		{
			if(HGH(PulseWidth))
			{
				irData |= (1 << (15-bitCount)); bitCount++;
			}
			else if(LOW(PulseWidth))
			{
				irData &= ~(1 << (15-bitCount)); bitCount++;
			}
			else frame_status = FRAME_RESTART;
			if(bitCount == 16 && !frame_status){ IR_cmd_pending=1; bitCount=0; }
		}
	}
}

