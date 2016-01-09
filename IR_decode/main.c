#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

// LCD display handling
#include "LCD/lcd44780.h"
#include "IR_DECODE/ir_decode.h"
#include "UART/uart.h"

#define KEY_PRVS (1<<PD1)
#define KEY_PRVS_ON !(PIND & KEY_PRVS)
#define KEY_NEXT (1<<PD2)
#define KEY_NEXT_ON !(PIND & KEY_NEXT)
#define KEY_END (1<<PD3)
#define KEY_END_ON !(PIND & KEY_END)

#define LCD_BL (1<<PA7)

#define LED_CMD (1<<PD0)



void IR_monitor();

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

	IR_monitor();

}

#if(FUNCTION == PATTERN_LOG)
void IR_monitor()
{
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

		if(LOG_DONE_flag) {

			PORTD &= ~(LED_CMD);  // enable signalling LED

			if(USART_TRANSMIT)
			{
				USART_Init(__UBRR);
				for(i=0; i<MEAS_COUNT; i++)
				{
					uart_putint(i, 10);
					uart_puts(": ");
					uart_putint(readWidth[i], 10);
					uart_putc('\r');
					uart_putc('\n');
				}
			}


			i=-1; lcd_cls(); lcd_locate(0,0);
			if(USART_TRANSMIT) lcd_str("Log done & sent:");
			else lcd_str("Log done:"); lcd_locate(1,0);
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
#endif
#if(FUNCTION == DECODE_LGAKB)
void IR_monitor()
{

	char cmd[15]; // string for nice LCD command viewing
	int i;

	PORTA |= (LCD_BL);   // enable LCD backlight

	while(1) {

			if(IR_cmd_pending) {	// On IR command recieval

			PORTD &= ~(LED_CMD);  // enable signalling LED

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

			PORTD |= (LED_CMD);
			// Keep backlight on for 12s if no new signal
			for(i=0; i<120 && !IR_cmd_pending; i++)	_delay_ms(100);
			if(!IR_cmd_pending) PORTA &= ~(LCD_BL);

		}
	}

}
#endif
#if(FUNCTION == DECODE_RC5)
void IR_monitor()
{

	DDRD |= ((1<<PD4)|(1<<PD5));
	PORTD |= ((1<<PD4)|(1<<PD5));

	uint8_t lastCmd=0;
	uint8_t lastAddr=0;
	char cmd[15]; // string for nice LCD command viewing
	uint8_t i=1, j;

	PORTA |= (LCD_BL);   // enable LCD backlight
	//uart_puts("Monitoring...\n");

	while(1) {

			if(IR_cmd_pending) {	// On IR command recieval
			PORTA |= (LCD_BL);
			PORTD &= ~(LED_CMD);  // enable signalling LED
			if( (irCommand == lastCmd) && (irAddress == lastAddr) ) i++;
			else i=1;
			lastCmd=irCommand; lastAddr=irAddress;
			// Convert and describe command on LCD
			lcd_cls(); lcd_locate(0,0);
			lcd_str("Rec addr, cmd:"); lcd_locate(1,0);
			sprintf(cmd, "0x%02x, 0x%02x", irAddress, irCommand);
			lcd_str(cmd);
			if(i>1){ sprintf(cmd, " x%u", i); lcd_str(cmd); }
			// Wait some time, disable signalling LED
			_delay_ms(200);
			PORTD |= (LED_CMD);

			// CLEAR PENDING FLAG
			IR_cmd_pending=0;
			// MANDATORY for further decoding

			PORTD |= (LED_CMD);
			// Keep backlight on for 12s if no new signal
			for(j=0; j<120 && !IR_cmd_pending; j++)	_delay_ms(100);
			if(!IR_cmd_pending) PORTA &= ~(LCD_BL);

			/*int MEAS_COUNT = 50;

			while(IR_cmd_pending)
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
								sprintf(cmd, "%04x us", msgHist[i]);
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
								sprintf(cmd, "%04x us", msgHist[i]);
								lcd_str(cmd);
								_delay_ms(200);
							}
							if(KEY_END_ON)
							{
								IR_cmd_pending=0;
								_delay_ms(200);
							}
							_delay_ms(100);
						}*/

		}
			_delay_ms(20);
	}

}
#endif
