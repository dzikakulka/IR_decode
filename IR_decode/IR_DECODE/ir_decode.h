#ifndef IR_DECODE_H_
#define IR_DECODE_H_

// IR reciever & ICP1 port
#define IR_PORT D
#define IR_PIN 6
#define IR_IN (1<<IR_PIN)

// Timer 1 prescaler setting
#define TIMER1_PRESCALER 8

// Timer 1 ticks to microseconds macro
#define ticks_to_us(num) ( ( (num) * ((TIMER1_PRESCALER*1000000)/F_CPU)) )
#define ttus(num) ticks_to_us(num)

// Timer 1 microseconds to ticks macro
#define us_to_ticks(num) (((num)*F_CPU)/(1000000*TIMER1_PRESCALER))

// SETTINGS FOR IR TRANSMISSION W/ LG AKB 729152xx REMOTE
// Information coding: interval between LOW-HIGH slopes (HIGH-LOW on recieving end)

// Transmission format:
// [HEADER][PRE_BITx16][MSG_BITx16][GAP][REPEAT](+[GAP][REPEAT] repeatedly on button hold)
// TRAIL signal detection was omitted - my remote seems to not send it at all

// Full doc to be found at http://lirc.sourceforge.net/remotes/lg/AKB72915207

//// Signals definitions - all constants in microseconds :

// HEADER signal
#define HDR_L 13500  // length
#define HDR_TOL 1500 // length tolerance
// Compare to HEADER signal pattern (input pulse width in microseconds)
#define HDR(num) ( ((num) > (HDR_L - HDR_TOL)) && ((num) < (HDR_L + HDR_TOL)) )

// PRE_BITs patttern for that specific format
// Must match or transmission is invalid
#define IR_PRE 0x20df

// HIGH BIT signal
#define HGH_L 2250
#define HGH_TOL 350
#define HGH(num) ( ((num) > (HGH_L - HGH_TOL)) && ((num) < (HGH_L + HGH_TOL)) )

// LOW BIT signal
#define LOW_L 1120
#define LOW_TOL 250
#define LOW(num) ( ((num) > (LOW_L - LOW_TOL)) && ((num) < (LOW_L + LOW_TOL)) )

// REPEAT signal
#define RPT_L 11400
#define RPT_TOL 1500
#define RPT(num) ( ((num) > (RPT_L - RPT_TOL)) && ((num) < (RPT_L + RPT_TOL)) )

// TRAIL signal (omitted in implementation)
#define TRL_L 600
#define TRL_TOL 150
#define TRL(num) ( ((num) > (TRL_L - TRL_TOL)) && ((num) < (TRL_L + TRL_TOL)) )

// GAP between transmission(s) and REPEAT signals if button is held
#define GAP_MIN_L 25000
#define GAP(num) ( (num) > (GAP_MIN_L ) )

// 16 bit variable for storing incoming data
extern volatile uint16_t irData;

// Succesful transmission decoding flag
// MUST BE CLEARED TO DECODE ANOTHER TRANSMISSION
// Immediately after it is set, recieved command is available at irData variable
extern volatile uint8_t IR_cmd_pending;

// IR decoding process initialization
void ir_init();


#endif /* IR_DECODE_H_ */
