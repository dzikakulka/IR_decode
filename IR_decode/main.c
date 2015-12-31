#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

// LCD display handling
#include "LCD/lcd44780.h"
#include "IR_DECODE/ir_decode.h"

#define KEY_PRVS (1<<PD1)
#define KEY_PRVS_ON !(PIND & KEY_PRVS)
#define KEY_NEXT (1<<PD2)
#define KEY_NEXT_ON !(PIND & KEY_NEXT)
#define KEY_END (1<<PD3)
#define KEY_END_ON !(PIND & KEY_END)

#define LCD_BL (1<<PA7)

#define LED_CMD (1<<PD0)

int main(void) {

	// LCD backlight pin out, low init
	DDRA |= (LCD_BL);
	PORTA &= ~(LCD_BL);

	// Signal LED pin out, high init
	DDRD |= (LED_CMD);
	PORTD |= (LED_CMD);

	// Microswitch keys pins in, pullup
	DDRD &= ~(KEY_PRVS | KEY_NEXT | KEY_END);
	PORTD |= (KEY_PRVS | KEY_NEXT | KEY_END);

	lcd_init();	// LCD display init
	ir_init();	// IR decoding init

	sei();	// enable interrupts

	char cmd[15]; // string for nice LCD command viewing
	int i;

	int j=0;
	PORTA |= (LCD_BL);   // enable LCD backlight

	while(1) {

		if(!j)
		{
			lcd_cls(); lcd_locate(0,0);
			lcd_str("Ready to log"); lcd_locate(1,0);
			sprintf(cmd, "%u readings", MEAS_COUNT);
			lcd_str(cmd);
			j=1;
		}

		//if(IR_cmd_pending) {	// On IR command recieval
		if(LOG_DONE_flag) {

			PORTD &= ~(LED_CMD);  // enable signalling LED

			/*
			// Convert and describe command on LCD
			lcd_cls(); lcd_locate(0,0);
			lcd_str("Rec cmd:"); lcd_locate(1,0);
			lcd_str("0x");
			sprintf(cmd, "%04x", irData);
			lcd_str(cmd);
			// Wait some time, disable signalling LED
			_delay_ms(200);
			PORTD |= (LED_CMD);

			// CLEAR PENDING FLAG
			IR_cmd_pending=0;
			// MANDATORY for further decoding

			// Keep backlight on for 12s if no new signal
			for(i=0; i<120 && !IR_cmd_pending; i++)	_delay_ms(100);
			if(!IR_cmd_pending) PORTA &= ~(LCD_BL);
			*/

			i=-1; lcd_cls(); lcd_locate(0,0);
			lcd_str("Log done:"); lcd_locate(1,0);
			sprintf(cmd, "%u readings", MEAS_COUNT);
			lcd_str(cmd);

			while(LOG_DONE_flag)
			{
				if(KEY_NEXT_ON)
				{
					if((i<(MEAS_COUNT-1))) i++;
					else i=0;
					lcd_cls();
					lcd_locate(0,0);
					sprintf(cmd, "Width [%d]:", i+1);
					lcd_str(cmd);
					lcd_locate(1,0);
					sprintf(cmd, "%u us", readWidth[i]);
					lcd_str(cmd);
					_delay_ms(200);
				}
				if(KEY_PRVS_ON)
				{
					if(i>0) i--;
					else i=(MEAS_COUNT-1);
					lcd_cls();
					lcd_locate(0,0);
					sprintf(cmd, "Width [%d]:", i+1);
					lcd_str(cmd);
					lcd_locate(1,0);
					sprintf(cmd, "%u us", readWidth[i]);
					lcd_str(cmd);
					_delay_ms(200);
				}
				if(KEY_END_ON)
				{
					j=0;
					LOG_DONE_flag=0;
					_delay_ms(200);
				}
				_delay_ms(100);

			}

			PORTD |= (1<<PD0);
		}
	}
}
