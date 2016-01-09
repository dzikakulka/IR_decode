#include <avr/io.h>
#include <avr/interrupt.h>

#include "ir_decode.h"
#include "../makrau.h"
#include "../UART/uart.h"

	volatile uint16_t msgHist[50];

#if FUNCTION == PATTERN_LOG

// Duration of each read impulse will be stored here
// THIS IMPLIES MAX 65.5ms IMPULSE TIME
// Change type to uint32_t if needed
volatile uint16_t readWidth[MEAS_COUNT];

// This will be set to 1 when logging data is finished
// Logging will start (again) every time it is set to 0 and IR input changes
volatile uint8_t LOG_DONE_flag;

#endif

#if FUNCTION == DECODE_LGAKB
// 16 bit variable for storing incoming data
volatile uint16_t irData;

// Succesful transmission decoding flag
// MUST BE CLEARED TO DECODE ANOTHER TRANSMISSION
// Immediately after it is set, recieved command is available at irData variable
volatile uint8_t IR_cmd_pending;
#endif

#if FUNCTION == DECODE_RC5
volatile uint8_t irAddress;
volatile uint8_t irCommand;

volatile uint8_t IR_cmd_pending;
#endif

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

	#if FUNCTION > 0
	IR_cmd_pending = 0;	  // Clear pending cmd flag
	#else
	LOG_DONE_flag = 0;
	#endif
}

#if FUNCTION == PATTERN_LOG
// INTERRUPT HANDLING
// Triggered on 36kHz IR signal detection
ISR(TIMER1_CAPT_vect) {

	static uint16_t LastCapture;
	uint32_t PulseWidth;

	static uint16_t pulseCount=0;

	PulseWidth = ICR1 - LastCapture;
	LastCapture = ICR1;
	TCCR1B ^= (1<<ICES1);

	if(pulseCount>0) readWidth[pulseCount-1]=ttus(PulseWidth);
	if( (pulseCount-1) == (MEAS_COUNT-1))
	{
		pulseCount=0;
		TCCR1B &= ~(1<<ICES1);
		LOG_DONE_flag=1;
	}
	else pulseCount++;
}
#endif


#if FUNCTION == DECODE_LGAKB
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
#endif

#if FUNCTION == DECODE_RC5
// INTERRUPT HANDLING
// Triggered on 36kHz IR signal detection
ISR(TIMER1_CAPT_vect) {

#define FRAME_BEGIN 1
#define FRAME_RESTART 0
#define bitStatus (TCCR1B & (1<<ICES1))?0:1

#define FLD ((message >> 12) & 1)
#define RPT ((message >> 11) & 1)

	static uint16_t LastCapture;
	uint32_t PulseWidth;

	static uint8_t frame_status = 0;
	static int8_t bitCount = 13;
	static uint16_t message = 0;
	static int8_t halfBit = 1;
	static uint8_t lastRepeat;
	static uint8_t first = 1;

	PulseWidth = ICR1 - LastCapture;

	PORTD ^= (1<<PD4);

	if(!IR_cmd_pending)
	{
		//PORTD ^= (1<<PD5);
		if(frame_status == FRAME_RESTART)
		{
			bitCount = 13;
			message = 0;
			message |= (bitStatus<<bitCount);
			bitCount--;
			halfBit = 1;
			frame_status = FRAME_BEGIN;
		}
		else if(frame_status == FRAME_BEGIN)
		{
			if(halfBit == 1)
			{
				if(HBT(ttus(PulseWidth)))
				{
					halfBit = 0;
				}
				else if(FBT(ttus(PulseWidth)))
				{
					message |= (bitStatus<<bitCount);
					bitCount--;
					halfBit = 1;
				}
				else frame_status = FRAME_RESTART;
			}
			else
			{
				if(HBT(ttus(PulseWidth)))
				{
					message |= (bitStatus<<bitCount);
					bitCount--;
					halfBit = 1;
				}
				else frame_status = FRAME_RESTART;
			}

		}

		if(bitCount < 0)
		{
			if(frame_status != FRAME_RESTART)
			{
				//PORTD ^= (1<<PD5);
				if( (RPT != lastRepeat) || first )
				{
				irAddress = (message & 0b0000011111000000)>>6;
				irCommand = (message & 0b0000000000111111);
				irCommand |= (!FLD)<<6;
				IR_cmd_pending = 1;
				lastRepeat = RPT;
				first = 0;
				}
			}
			frame_status = FRAME_RESTART;

		}
		//msgHist[dCount] = message;
		//dCount++;
		//if(frame_status == FRAME_RESTART){ IR_cmd_pending = 1; msgHist[dCount+1] = 0xffff;
		//msgHist[dCount+2] = bitCount;}

	}
	if(frame_status == FRAME_RESTART) TCCR1B &= ~(1<<ICES1);
	else TCCR1B ^= (1<<ICES1);

}
#endif
