#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

// LCD display handling
#include "LCD/lcd44780.h"
#include "IR_DECODE/ir_decode.h"

int main(void) {

	// LCD backlight pin
	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7);

	// 'Recieved command' signal LED
	DDRD |= (1<<PD0);
	PORTD |= (1<<PD0);

	lcd_init();	// LCD display init
	ir_init();	// IR decoding init

	sei();	// enable interrupts

	char cmd[10]; // string for nice LCD command viewing

	while(1) {

		if(IR_cmd_pending) {	// On IR command recieval

			PORTD &= ~(1<<PD0);  // enable signalling LED

			// Convert and describe command on LCD
			lcd_cls(); lcd_locate(0,0);
			lcd_str("Rec cmd:"); lcd_locate(1,0);
			lcd_str("0x");
			sprintf(cmd, "%04x", irData);
			lcd_str(cmd);
			// Wait some time, disable signalling LED
			_delay_ms(200);
			PORTD |= (1<<PD0);

			// CLEAR PENDING FLAG
			IR_cmd_pending=0;
			// MANDATORY for further decoding
		}
	}
}
