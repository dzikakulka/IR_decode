#ifndef IR_DECODE_H_
#define IR_DECODE_H_


extern volatile uint16_t msgHist[50];

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
//                        PROGRAM SETUP                        //

// Available IR signal handling presets:
#define PATTERN_LOG 0   // LOG set number of impulses for review
#define DECODE_LGAKB 1  // DECODE LG AKB 729152xx remote signal
#define DECODE_RC5 2    // DECODE RC5 standard compliant signal

// Select one of above presets here:
#define FUNCTION PATTERN_LOG

//                                                             //
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //

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

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
// SIGNAL PATTERN LOGGING FUNCTION declarations
#if FUNCTION == PATTERN_LOG
// How many input pulses to log
// Each change of input (lo-hi, hi-lo) is a pulse
// Counting starts at first hi-lo transition
#define MEAS_COUNT 128

#define USART_TRANSMIT 1

// Duration of each read impulse will be stored here
// THIS IMPLIES MAX 65.5ms IMPULSE TIME
// Change type to uint32_t if needed
extern volatile uint16_t readWidth[MEAS_COUNT];
// To determine state of each pulse, remember that:
// Indexing pulses from 1, first has carrier wave present
// Each pulse with ODD index took place when carrier was present
// Each pulse with EVEN index took place when carrier was NOT present

// This will be set to 1 when logging data is finished
// Logging will start (again) every time it is set to 0 and IR input changes
extern volatile uint8_t LOG_DONE_flag;

#endif
// END OF SIGNAL PATTERN LOGGING FUNCTION declarations
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
// LG AKB REMOTE DECODING decarations
#if FUNCTION == DECODE_LGAKB

// SETTINGS FOR IR TRANSMISSION W/ LG AKB 729152xx REMOTE
// Information coding: interval between LOW-HIGH slopes (HIGH-LOW on recieving end)

// Transmission format:
// [HEADER][PRE_BITx16][MSG_BITx16][TRAIL][GAP][REPEAT](+[GAP][REPEAT] repeatedly on button hold)
// TRAIL signal detection was omitted - it is not needed it application

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

#endif
// END OF LG AKB REMOTE DECODING declarations
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
// RC% SIGNAL DECODING declarations
#if FUNCTION == DECODE_RC5


#define HBT_L 890
#define HBT_TOL 200
#define HBT(num) ( ((num) > (HBT_L - HBT_TOL)) && ((num) < (HBT_L + HBT_TOL)) )

#define FBT_L 1780
#define FBT_TOL 300
#define FBT(num) ( ((num) > (FBT_L - FBT_TOL)) && ((num) < (FBT_L + FBT_TOL)) )

#define GAP_L 4000
#define GAP(num) ( (num) > GAP_L )

extern volatile uint8_t irAddress;
extern volatile uint8_t irCommand;

extern volatile uint8_t IR_cmd_pending;

#endif

// IR decoding process initialization
void ir_init();


#endif /* IR_DECODE_H_ */
