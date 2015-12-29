/*
 * ir_decode.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ir_decode.h"
#include "../makrau.h"

volatile uint16_t irData;		// komenda RC5
volatile uint16_t readWidth[MEAS_COUNT];
volatile char signal[MEAS_COUNT];

// flaga = 1 - informuje, ¿e odebrany zosta³ nowy kod z pilota
// po odczytaniu danych ze zmiennych nale¿y wyzerowaæ flagê aby
// zosta³y przyjête kolejne kody z pilota
volatile uint8_t Ir_key_press_flag;

volatile uint8_t rc5cnt;

void ir_init() {
	DDR(IR_PORT) &= ~IR_IN;		// pin jako wejœcie
	PORT(IR_PORT) |= IR_IN;		// podci¹gniêcie pinu do VCC
	#if TIMER1_PRESCALER == 1
		TCCR1B |= (1<<CS10);         	// Timer1 / 1
		TCCR1B &= ~((1<<CS11) | 1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 8
		TCCR1B |= (1<<CS11);         	// Timer1 / 8
		TCCR1B &= ~((1<<CS10) | 1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 64
	  	TCCR1B |= (1<<CS11)|(1<<CS10); // Timer1 / 64
	  	TCCR1B &= ~(1<<CS12);
	#endif
	#if TIMER1_PRESCALER == 256
		TCCR1B |= (1<<CS12);         	// Timer1 / 256
		TCCR1B &= ~((1<<CS10) | 1<<CS11);
	#endif

	TCCR1B &= ~(1<<ICES1);      	// Zbocze opadaj¹ce na ICP
	rc5cnt = 0;						// zerowanie licznika wystêpuj¹ych zboczy

	TIMSK |= (1<<TICIE1);        	// Przerwanie od ICP
	Ir_key_press_flag = 0;			// zerowanie flagi otrzymania kodu z pilota
}


// procedura obs³ugi przerwania ICP1
ISR(TIMER1_CAPT_vect) {

#define FRAME_STARTED 1
#define PRE_OK 2
#define FRAME_RESTART 0

	static uint16_t LastCapture;
	uint32_t PulseWidth;
	//static uint8_t IrPulseCount;
	static uint16_t irPre = 0x20df;
	static uint8_t frame_status = 0;
	//static uint8_t byteCount=0;
	static uint8_t bitCount=0;

	PulseWidth = ICR1 - LastCapture;
	LastCapture = ICR1;

	if(!Ir_key_press_flag)
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
		if(bitCount == 16 && !frame_status){ Ir_key_press_flag=1; bitCount=0; }
	}
	}

	/*readWidth[byteCount] = ttus(PulseWidth);
	if(!Ir_key_press_flag)
	{
		if(HDR(ttus(PulseWidth)))
		{
		signal[byteCount] = 'H';
		}
		else if(LOW(ttus(PulseWidth)))
		{
			signal[byteCount] = '0';
		}
		else if(HGH(ttus(PulseWidth)))
		{
			signal[byteCount] = '1';
		}
		else if(RPT(ttus(PulseWidth)))
		{
			signal[byteCount] = 'R';
		}
		else if(TRL(ttus(PulseWidth)))
		{
			signal[byteCount] = 'T';
		}
		else if(GAP(ttus(PulseWidth)))
		{
			signal[byteCount] = 'G';
		}
		else
		{
			signal[byteCount] = 'x';
		}
		byteCount++;
		if(byteCount == (MEAS_COUNT - 1)){ Ir_key_press_flag = 1; byteCount=0; }
	}*/


/*
	if(!frame_status && (ticks_to_us(PulseWidth) > (HDR_L - HDR_TOL)) && (ticks_to_us(PulseWidth) < (HDR_L + HDR_TOL)) )
	{
		frame_status = FRAME_STARTED; bitCount=0;
	}
	if(frame_status == FRAME_STARTED)
	{
		if( (ticks_to_us(PulseWidth) > (HGH_L - HGH_TOL)) && (ticks_to_us(PulseWidth) < (HGH_L + HGH_TOL)) )
		{

		}
	}*/
	/*TCCR1B ^= (1<<ICES1);	// zmiana zbocza wyzwalaj¹cego na przeciwne

	if (PulseWidth > MAX_BIT) rc5cnt = 0;// PORTD ^= (1<<PD1); }

	if (rc5cnt > 0) frame_status = FRAME_OK;

	if (rc5cnt == 0) {
		IrData = 0;
		IrPulseCount = 0;
		TCCR1B |= (1<<ICES1);
		rc5cnt++;
		frame_status = FRAME_END;
	}

	if (frame_status == FRAME_OK) {

	    // gdy zak³ócenia (szpilki) - RESTART
		if(PulseWidth<MIN_HALF_BIT) frame_status = FRAME_RESTART;// PORTD ^= (1<<PD2); }

			// gdy b³¹d ramki danych (mo¿e inny standard ni¿ RC5) RESTART
			if( PulseWidth > MAX_BIT ) frame_status = FRAME_RESTART;

			if (frame_status == FRAME_OK) 	{
				if (PulseWidth > MAX_HALF_BIT) rc5cnt++;

					if (rc5cnt > 1)
					if ( (rc5cnt % 2) == 0 ) {
						IrData = IrData << 1;
						if((TCCR1B & (1<<ICES1))) IrData |= 0x0001;
						IrPulseCount++;

						if (IrPulseCount > 12) 	{
							if (Ir_key_press_flag == 0) {
								command = IrData & 0b0000000000111111;
								address = (IrData & 0b0000011111000000) >> 6;
								toggle_bit = (IrData & 0b0000100000000000) >> 11;
							}
							frame_status = FRAME_RESTART;
							Ir_key_press_flag = 1;
						}
					}
					rc5cnt++;
			}
	}

	if (frame_status == FRAME_RESTART)	{
		rc5cnt = 0;
		TCCR1B &= ~(1<<ICES1);
	}

	PORTD ^= 1<<frame_status;*/
}

