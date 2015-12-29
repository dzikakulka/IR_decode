/*
 * main.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>

#include "LCD/lcd44780.h"
#include "IR_DECODE/ir_decode.h"

#define K_NXT (1<<PD0)
#define K_PRV (1<<PD1)
#define K_END (1<<PD2)

#define K_NXT_PRESS !(PIND & K_NXT)
#define K_PRV_PRESS !(PIND & K_PRV)
#define K_END_PRESS !(PIND & K_END)


int main(void) {

	DDRA |= (1<<PA7);	// ustawiamy kierunek linii podœwietlenia LCD jako WYJŒCIE
	PORTA |= (1<<PA7);	// za³¹czamy podœwietlenie LCD - stan wysoki

	DDRC |= (1<<PC0);
	PORTC |= (1<<PC0);

	int i,j,k;
	DDRD |= 0x38;
	PORTD |= 0x3f;

		for(i=3; i<=5; i++)
		{
			PORTD &= ~(1<<i);
			_delay_ms(50);
			PORTD |= (1<<i);
			_delay_ms(50);
		}



	lcd_init();	/* inicjalizacja LCD */
	ir_init();	/* inicjalizacja dekodowania IR */

	sei();	/* w³¹czamy globalne przerwania */

	lcd_locate(0,0);
	//lcd_str_P(PSTR("Kody pilota RC5")); /* wyœwietl napis w zerowej linii LCD */
//char sigs[12];
//char length[10];
char cmd[10];
	/* pêtla nieskoñczona */
	while(1) {

		/*for(i=0; i<MEAS_COUNT; i++)
		{
			PORTC ^= (1<<PC0);
			_delay_us(490);
		}*/

		if(Ir_key_press_flag) {	/* jeœli odebrano prawid³owe kody z pilota */
			PORTD &= ~(1<<PD3);
			lcd_cls(); lcd_locate(0,0);
			lcd_str("Rec cmd:"); lcd_locate(1,0);
			lcd_str("0x");
			sprintf(cmd, "%04x", irData);
			lcd_str(cmd);
			_delay_ms(200);
			/*i=0;
			PORTD &= ~(1<<PD3);
			while(!K_END_PRESS)
			{
				if(K_NXT_PRESS)
				{
					if((i+11)<(MEAS_COUNT-1)) i++;
					if(k<(MEAS_COUNT-1)) k++;
					for(j=i; j<(i+12); j++) sigs[j-i] = signal[j];
					lcd_cls();
					lcd_locate(0,0);
					lcd_str(sigs);
					lcd_locate(1,0);
					sprintf(length, "[%c]: %u", signal[k], readWidth[k]);
					lcd_str(length);
					_delay_ms(200);
				}
				if(K_PRV_PRESS)
				{
					if(i>0) i--;
					if(k>0) k--;
					for(j=i; j<(i+12); j++) sigs[j-i] = signal[j];
					lcd_cls();
					lcd_locate(0,0);
					lcd_str(sigs);
					lcd_locate(1,0);
					sprintf(length, "[%c]: %u", signal[k], readWidth[k]);
					lcd_str(length);
					_delay_ms(200);
				}
				_delay_ms(100);
			}*/



			/*lcd_cls(); lcd_locate(0,0);
			sprintf(width, "%d", (int) readWidth[0]);//, (int) readWidth[1]);
			//lcd_str(width); lcd_locate(2,0);
			//sprintf(width, "%d %d   ", (int) readWidth[2], (int) readWidth[3]);
			lcd_str(width); lcd_locate(0,0);
			PORTD &= ~(1<<PD5);
			_delay_ms(2000);
			PORTD |= (1<<PD5);*/
				PORTD |= (1<<PD3);
			Ir_key_press_flag=0;

		}

	}
}
