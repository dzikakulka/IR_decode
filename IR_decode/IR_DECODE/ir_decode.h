/*
 * ir_decode.h
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */

#ifndef IR_DECODE_H_
#define IR_DECODE_H_



#define IR_PORT D
#define IR_PIN 6
#define IR_IN (1<<IR_PIN)

// sta³e i zmienne potrzebne na wewnêtrzne cele procedury obs³ugi RC5

#define TIMER1_PRESCALER 8

// przeliczanie tykniêæ Timera1 przy danym preskalerze na mikrosekundy
#define ir_micro_s(num) ( (num)*(F_CPU/1000000)/TIMER1_PRESCALER)

#define ticks_to_us(num) ( ( (num) * ((TIMER1_PRESCALER*1000000)/F_CPU)) )
#define ttus(num) ticks_to_us(num)
#define us_to_ticks(num) (((num)*F_CPU)/(1000000*TIMER1_PRESCALER))
// sta³e czasowa i tolerancja wyra¿one w us
#define TOLERANCE 200

#define HDR_L 13500
#define HDR_TOL 1500
#define HDR(num) ( ((num) > (HDR_L - HDR_TOL)) && ((num) < (HDR_L + HDR_TOL)) )

#define HGH_L 2250
#define HGH_TOL 350
#define HGH(num) ( ((num) > (HGH_L - HGH_TOL)) && ((num) < (HGH_L + HGH_TOL)) )

#define LOW_L 1120
#define LOW_TOL 250
#define LOW(num) ( ((num) > (LOW_L - LOW_TOL)) && ((num) < (LOW_L + LOW_TOL)) )

#define RPT_L 11400
#define RPT_TOL 1500
#define RPT(num) ( ((num) > (RPT_L - RPT_TOL)) && ((num) < (RPT_L + RPT_TOL)) )

#define TRL_L 600
#define TRL_TOL 150
#define TRL(num) ( ((num) > (TRL_L - TRL_TOL)) && ((num) < (TRL_L + TRL_TOL)) )

#define GAP_MIN_L 25000
#define GAP(num) ( (num) > (GAP_MIN_L ) )

#define MEAS_COUNT 80

#define MIN_HALF_BIT 	ir_micro_s(889	- TOLERANCE)
#define MAX_HALF_BIT 	ir_micro_s(889 + TOLERANCE)
#define MAX_BIT ir_micro_s((889+889) + TOLERANCE)




// Makra upraszczaj¹ce dostêp do portów
// *** PORT
//#define PORT(x) XPORT(x)
//#define XPORT(x) (PORT##x)
//// *** PIN
//#define PIN(x) XPIN(x)
//#define XPIN(x) (PIN##x)
//// *** DDR
//#define DDR(x) XDDR(x)
//#define XDDR(x) (DDR##x)





// ------------------------------------------- definicje klawiszy PILOTA IR
#define teletext_red 55
#define teletext_green 54
#define teletext_yellow 50
#define teletext_blue 52
#define menu 36
#define button_ok 59
#define power_off 12
#define mute 13
#define vol_up 16
#define vol_down 17
#define prog_up 32
#define prog_down 33
#define cursor_up prog_up
#define cursor_down prog_down
#define cursor_left vol_down
#define cursor_right vol_up
// ------------------------------------------- definicje klawiszy KONIEC

// zmienne charakterystyczne tylko dla obs³ugi RC5
extern volatile uint8_t toggle_bit;		// bit TOGGLE
extern volatile uint8_t address;		// adres
extern volatile uint8_t command;		// komenda
extern volatile uint16_t readWidth[MEAS_COUNT];
extern volatile char signal[MEAS_COUNT];
extern volatile uint16_t irData;

// flaga = 1 - informuje, ¿e odebrany zosta³ nowy kod z pilota
extern volatile uint8_t Ir_key_press_flag;

// deklaracje funkcji dla u¿ytkownika
void ir_init();


#endif /* IR_DECODE_H_ */
