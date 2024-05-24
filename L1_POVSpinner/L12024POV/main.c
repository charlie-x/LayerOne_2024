// define cpu frequency for delay calculations
#define F_CPU 1000000UL  // 1 MHz

#ifndef SCROLL_TEXT
#define SCROLL_TEXT ( "                   HACK THE PLANET...    PACK THE HLANET? \0" )
#endif

#ifndef GREET_TEXT
#define GREET_TEXT  ( "WELCOME TO LAYERONE 2024  \0")
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////
/// images
/////////////////////////////////////////////////////////////////////////

// these waste 4 bits per row.. consider packing down.
#include "images/hack_0.h"
#include "images/hack_1.h"
#include "images/hack_2.h"
#include "images/hack_3.h"


// pre-calculated sine wave values (scaled and quantized)
static const uint8_t sineWave[] PROGMEM = {
	128, 140, 152, 164, 175, 185, 195, 203, 210, 216, 220, 223,
	224, 223, 220, 216, 210, 203, 195, 185, 175, 164, 152, 140,
	128, 116, 104, 92,  81,  71,  61,  53,  46,  40,  36,  33,
	32,  33,  36,  40,  46,  53,  61,  71,  81,  92,  104, 116
};
const int sineWaveSize = sizeof(sineWave) / sizeof(sineWave[0]);

/////////////////////////////////////////////////////////////////////////
/// fonts
/////////////////////////////////////////////////////////////////////////


static const uint8_t fontMap[][8] PROGMEM = {

	// Numbers 0-9
	{
		0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110, 0b00000000, 0b00000000, 0b00000000
	}, // 0
	{
		0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000
	}, // 1
	{
		0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110, 0b00000000, 0b00000000, 0b00000000
	}, // 2
	{
		0b00100001,
		0b01000001,
		0b01000101,
		0b01001011,
		0b00110001,
		0b00000000,
		0b00000000,
		0b00000000
	}, // 3
	{
		0b00011000,
		0b00010100,
		0b00010010,
		0b01111111,
		0b00010000,
		0b00000000,
		0b00000000,
		0b00000000
	}, // 4
	{
		0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001, 0b00000000, 0b00000000, 0b00000000
	}, // 5
	{
		0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000, 0b00000000, 0b00000000, 0b00000000
	}, // 6
	{
		0b00000001, 0b01110001, 0b00001001, 0b00000101, 0b00000011, 0b00000000, 0b00000000, 0b00000000
	}, // 7
	{
		0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110, 0b00000000, 0b00000000, 0b00000000
	}, // 8
	{
		0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110, 0b00000000, 0b00000000, 0b00000000
	} ,// 9


	{
		0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110, 0b00000000
	}, // A
	{
		0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110, 0b00000000
	}, // B
	{
		0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010, 0b00000000
	}, // C
	{
		0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b00111110, 0b00000000
	}, // D
	{
		0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001, 0b00000000
	}, // E
	{
		0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001, 0b00000000
	}, // F
	{
		0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010, 0b00000000
	}, // G
	{
		0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111, 0b00000000
	}, // H
	{
		0b00000000, 0b01000001, 0b01111111, 0b01000001, 0b00000000, 0b00000000
	}, // I
	{
		0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001, 0b00000000
	}, // J
	{
		0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000
	}, // K
	{
		0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000
	}, // L
	{
		0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111, 0b00000000
	}, // M
	{
		0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111, 0b00000000
	}, // N
	{
		0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110, 0b00000000
	}, // O
	{
		0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110, 0b00000000
	}, // P
	{
		0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110, 0b00000000
	}, // Q
	{
		0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110, 0b00000000
	}, // R
	{
		0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001, 0b00000000
	}, // S
	{
		0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001, 0b00000000
	}, // T
	{
		0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111, 0b00000000
	}, // U
	{
		0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111, 0b00000000
	}, // V
	{
		0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111, 0b00000000
	}, // W
	{
		0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011, 0b00000000
	}, // X
	{
		0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111, 0b00000000
	}, // Y
	{
		0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011, 0b00000000
	} // Z

};


// define led pins
#define LED_PORT					PORTA
#define LED_DDR						DDRA

#define BUTTON						PD2
#define HALL_EFFECT_POWER			PB5

#define BIT_SET(port, bit)			((port) |= (1 << (bit)))
#define BIT_CLEAR(port, bit)		((port) &= ~(1 << (bit)))
#define BIT_TEST(port, bit)			((port) & (1 << (bit)))

// SLEEP_MODE_PWR_DOWN
// SLEEP_MODE_STANDBY
// SLEEP_MODE_IDLE

#define SLEEP_MODE					SLEEP_MODE_PWR_DOWN

#define true						( 1 )
#define false						( 0 )

// set to actual effects + 1
#define MAX_EFFECTS					( 10 )

// main loop delay ms*COUNTDOWN_LENGTH
#define COUNTDOWN_LENGTH			( 50000 )


// set the delay we're using
#define _delay_adaptive(x) { customDelayMicroseconds((delayAdj));}


void drawNumber(uint16_t num);
void setLEDS( uint16_t mask );

/////////////////////////////////////////////////////////////////////////
/// handling rpm
/////////////////////////////////////////////////////////////////////////


// out here for debugging mostly
volatile uint16_t lastTimerValue = 0;
volatile uint16_t rpmValue = 0;
volatile uint32_t revolutionDurationMs = 0;
uint16_t currentTimerValue;

// effects count
uint8_t count = 0;


// how much to delay by
volatile uint32_t delayAdj = 0;

// internal counter for timers
volatile uint32_t totalTimerTicks = 0;

// used for toggling animations etc
volatile uint8_t fillModeEffects = 0;

// effect index to display
volatile uint8_t currentEffect = 7;

// for knowing if we were just asleep
volatile uint8_t isSleeping = false;

// software counter for powering down delay
volatile uint32_t powerDownCountdown = COUNTDOWN_LENGTH;

// did we just hit the hall effect
volatile uint8_t  isTriggered = false;

// read the button
// 1 if pressed, 0 if not
uint8_t readButton() {
	return ((PIND & (1 << BUTTON)) == 0);
}


// arduino delay code
/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void customDelayMicroseconds(unsigned int us)
{

	// call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

	// calling avrlib's delay_us() function with low values (e.g. 1 or
	// 2 microseconds) gives delays longer than desired.
	//delay_us(us);
	#if F_CPU >= 24000000L
	// for the 24 MHz clock for the adventurous ones, trying to overclock

	// zero delay fix
	if (!us) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/6 of a microsecond (4 cycles)
	// per iteration, so execute it six times for each microsecond of
	// delay requested.
	us *= 6; // x6 us, = 7 cycles

	// account for the time taken in the preceding commands.
	// we just burned 22 (24) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can subtract 5
	us -= 5; //=2 cycles

	#elif F_CPU >= 20000000L
	// for the 20 MHz clock on rare Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 18 (20) cycles, which is 1us
	__asm__ __volatile__ (
	"nop" "\n\t"
	"nop" "\n\t"
	"nop" "\n\t"
	"nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/5 of a microsecond (4 cycles)
	// per iteration, so execute it five times for each microsecond of
	// delay requested.
	us = (us << 2) + us; // x5 us, = 7 cycles

	// account for the time taken in the preceding commands.
	// we just burned 26 (28) cycles above, remove 7, (7*4=28)
	// us is at least 10 so we can subtract 7
	us -= 7; // 2 cycles

	#elif F_CPU >= 16000000L
	// for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/4 of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2; // x4 us, = 4 cycles

	// account for the time taken in the preceding commands.
	// we just burned 19 (21) cycles above, remove 5, (5*4=20)
	// us is at least 8 so we can subtract 5
	us -= 5; // = 2 cycles,

	#elif F_CPU >= 12000000L
	// for the 12 MHz clock if somebody is working with USB

	// for a 1 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1.5us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/3 of a microsecond (4 cycles)
	// per iteration, so execute it three times for each microsecond of
	// delay requested.
	us = (us << 1) + us; // x3 us, = 5 cycles

	// account for the time taken in the preceding commands.
	// we just burned 20 (22) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can subtract 5
	us -= 5; //2 cycles

	#elif F_CPU >= 8000000L
	// for the 8 MHz internal clock

	// for a 1 and 2 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 2us
	if (us <= 2) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/2 of a microsecond (4 cycles)
	// per iteration, so execute it twice for each microsecond of
	// delay requested.
	us <<= 1; //x2 us, = 2 cycles

	// account for the time taken in the preceding commands.
	// we just burned 17 (19) cycles above, remove 4, (4*4=16)
	// us is at least 6 so we can subtract 4
	us -= 4; // = 2 cycles

	#else
	// for the 1 MHz internal clock (default settings for common Atmega micro controllers)

	// the overhead of the function calls is 14 (16) cycles
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to subtract 22)

	// compensate for the time taken by the preceding and next commands (about 22 cycles)
	us -= 22; // = 2 cycles
	// the following loop takes 4 microseconds (4 cycles)
	// per iteration, so execute it us/4 times
	// us is at least 4, divided by 4 gives us 1 (no zero delay bug)
	us >>= 2; // us div 4, = 4 cycles
	

	#endif

	// busy wait
	__asm__ __volatile__ (
	"1: sbiw %0,1" "\n\t" // 2 cycles
	"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
}

// calculate delay time for 1 degree of rotation based on RPM
float calculateMSDelayPerDegree(uint32_t rpm) {
	if (rpm == 0) {
		return 100;														// avoid division by zero
	}

	float  timePerRevolutionSeconds = 60.0f / rpm;						// time per revolution in seconds
	float delayPerDegreeSeconds = timePerRevolutionSeconds / 360.0f;	// time for 1 degree in seconds

	return (uint32_t) ((float)delayPerDegreeSeconds * 1000.0f);			// convert to milliseconds
}

// calculate the adjustable delay based on RPM
uint32_t calculateAdjustableDelay() {
	
	//if (rpm == 0) return 500;  // Max delay to prevent operation halt

	return (revolutionDurationMs / 128)*4;
}

// figure out RPM

// calculate RPM from time per revolution in milliseconds
uint32_t calculateRPM() {
	if (revolutionDurationMs == 0) {
		return 0; // Avoid division by zero
	}

	return (uint32_t) ((float)(600000.0f / revolutionDurationMs));
}

//draw a single glyph
void drawDigitR(uint8_t num) {

	for (uint8_t row = 0; row < 8; row++) {
		setLEDS( pgm_read_byte( &fontMap[num][row]) );
		_delay_adaptive(500);
	}
}


// draw a single char, 0-9 A-Z
void drawChar(uint8_t c) {
	if (c >= '0' && c <= '9') {
		drawDigitR(c - '0');
		} else if (c >= 'A' && c <= 'Z') {
		drawDigitR(c - 'A' + 10);
		// delay for space
		}else if( c == ' ')  {
		_delay_us(500);
	}
}

// expensive.. draw a 16 bit decimal number
void drawNumber(uint16_t num) {
	drawDigitR(num / 1000);							// thousands
	_delay_adaptive(100);
	drawDigitR((num / 100) - (num / 1000) * 10);	// hundreds
	_delay_adaptive(100);
	drawDigitR((num / 10) - (num / 100) * 10);		// tens
	_delay_adaptive(100);
	drawDigitR(num - (num / 10) * 10);				// ones
	_delay_adaptive(100);
}

// draw a single glyph with scroll offset
// @param num Glyph number to draw
// @param offset Pixel offset for scrolling
void drawDigitScrollable(uint8_t num, int8_t offset) {
	for (uint8_t row = 0; row < 8; row++) {
		uint8_t rowData = pgm_read_byte(&fontMap[num][row]);
		// Apply scroll offset
		if (offset > 0) {
			rowData <<= offset; // Scroll left
			} else if (offset < 0) {
			rowData >>= -offset; // Scroll right
		}
		setLEDS(rowData);
		_delay_adaptive(500);
	}
}

// drawChar to include scroll offset
// @param c Character to draw
// @param offset Pixel offset for scrolling
void drawCharScrollable(uint8_t c, int8_t offset) {
	if (c >= '0' && c <= '9') {
		drawDigitScrollable(c - '0', offset);
		} else if (c >= 'A' && c <= 'Z') {
		drawDigitScrollable(c - 'A' + 10, offset);
		} else if (c == ' ') {
		_delay_us(500); // Delay for space, consider scrolling space if needed
	}
}

// drawString to include vertically scroll 
// @param str Pointer to the string to be drawn
// @param offset Pixel offset for scrolling
void drawStringScrollableV(const char* str, int8_t offset) {
	while (*str) {
		drawCharScrollable(*str++, offset);
		_delay_adaptive(100); // delay between characters for readability
	}
}

// draw a string
void drawString(const char* str) {
	while (*str) {
		drawChar(*str++);
		_delay_adaptive(100);  // delay between characters for readability
	}
}



// Draw a single glyph with horizontal scroll offset for a POV spinner
// @param num Glyph number to draw
// @param column Column of the glyph to display for horizontal scrolling
void drawDigitPOV(uint8_t num, uint8_t column) {
	// Assuming 8 LEDs in vertical array and 8 bits per glyph row
	for (uint8_t row = 0; row < 8; row++) {
		uint8_t rowData = pgm_read_byte(&fontMap[num][row]);
		
		// Extract the bit for the current column
		uint8_t columnData = (rowData >> (7 - column)) & 0x01;
		
		// Convert columnData to vertical LED pattern
		uint16_t ledPattern = columnData ? (1 << row) : 0;
		
		setLEDS(ledPattern);
		
		// Adjust delay for POV display timing
		_delay_adaptive(50); // This delay might need tuning based on actual POV speed
	}
}

// Modified drawChar to handle POV horizontal scrolling
// @param c Character to draw
// @param column Column of the character to display for scrolling
void drawCharPOV(uint8_t c, uint8_t column) {
	if (c >= '0' && c <= '9') {
		drawDigitPOV(c - '0', column);
		} else if (c >= 'A' && c <= 'Z') {
		drawDigitPOV(c - 'A' + 10, column);
		} else if (c == ' ') {
		// Handle space character for POV display
		setLEDS(0); // Clear LEDs for space
		_delay_adaptive(50); // Maintain timing consistency
	}
}

// Function to scroll a string horizontally on a POV display
// @param str Pointer to the string to be scrolled
void scrollStringPOV(const char* str)
{
	static uint8_t column = 0;
	const char *rstr = str;
	
	// add on offset to string
	str+=column;
	
	// if hit the end of the string, reset it back it the start, add a visible delay and go back to outer loop
	if( *str == 0 ) {
			
		str = rstr;
		column=0;
			
		_delay_adaptive(500); // Maintain timing consistency
		return; //otherwise it will loop forever
	}
	
	// the outside main loop counter for the effects
//	if (count == 1 ) {
//		column++;
//	}
	
	column++;
	uint8_t chars = 0;
	while (*str) {
		// For each character, scroll through its columns
		{
			if(*str == '@' ) {
				_delay_adaptive(500);  // delay between characters for readability 
			} else {
				drawChar(*str);
			
				_delay_adaptive(100);  // delay between characters for readability
				}
			chars++;
			
			// don't wrap over the start, adjust this for length of text drawn at once
			if( chars == 15 ) break;

			
		}
		
		str++; // Move to next character
		
	
	}
}

// draw an image from an array
// The array should have 8 pairs of uint8_t values, each pair representing 12 bits (for 12 LEDs)
void drawImage(const uint16_t image_data[],uint8_t height)
{
	for (uint8_t row = 0; row < height; ++row) {
		
		uint16_t rowData =pgm_read_word( &image_data[row]);
		
		// call setLEDs to display this row
		setLEDS(rowData);

		// delay for persistence of vision
		_delay_adaptive(5700);
	}
}

/////////////////////////////////////////////////////////////////////////
/// effects
/////////////////////////////////////////////////////////////////////////

void sineWaveEffect(uint8_t mode ) {
	static int waveIndex = 0;
	const int numLEDs = 12;
	uint16_t ledValue = 0;
	const uint8_t threshold = 128; // Mid-point of the scaled sine wave

	
	for(uint8_t i = 0 ; i < 250; i++)
	{
		
		if(readButton())break;
		
		ledValue = 0;

		for (int i = 0; i < numLEDs; i++) {
			int index = (waveIndex + i) % sineWaveSize;
			if (pgm_read_byte( &sineWave[index] ) > threshold) {
				ledValue |= (1 << i);
				if( mode) {
					setLEDS(ledValue);
					for( uint16_t o = 0; o  <delayAdj/4  ; o++ )
					_delay_us(1);
				}
			}
		}

		if( !mode)
		setLEDS(ledValue);

		waveIndex++;
		
		if (waveIndex >= sineWaveSize) {
			waveIndex = 0;
		}

		// Simple delay
		//customDelayMicroseconds(delayAdj/4);
		//_delay_adaptive(800); // Adjust the delay as needed
		
		//i++;
		//if( i>=250) i =0;
	}
}

/////////////////////////////////////////////////////////////////////////
/// timer
/////////////////////////////////////////////////////////////////////////

void setupTimer() {
	// configure Timer1
	TCCR1B |= (1 << CS11);			// 8 prescaler
	TCNT1 = 0;
}

// set the leds.
void setLEDS( uint16_t mask )
{

	#define inv  (11)
	
	if (BIT_TEST(mask,inv -  0) ) BIT_SET( PORTD, PD0) ; else BIT_CLEAR (PORTD , PD0);
	if (BIT_TEST(mask,inv -  1) ) BIT_SET( PORTD, PD1) ; else BIT_CLEAR (PORTD , PD1);
	if (BIT_TEST(mask,inv -  2) ) BIT_SET( PORTD, PD4) ; else BIT_CLEAR (PORTD , PD4);
	if (BIT_TEST(mask,inv -  3) ) BIT_SET( PORTD, PD5) ; else BIT_CLEAR (PORTD , PD5);
	if (BIT_TEST(mask,inv -  4) ) BIT_SET( PORTD, PD6) ; else BIT_CLEAR (PORTD , PD6);
	if (BIT_TEST(mask,inv -  5) ) BIT_SET( PORTB, PB0) ; else BIT_CLEAR (PORTB , PB0);
	if (BIT_TEST(mask,inv -  6) ) BIT_SET( PORTB, PB1) ; else BIT_CLEAR (PORTB , PB1);
	if (BIT_TEST(mask,inv -  7) ) BIT_SET( PORTB, PB2) ; else BIT_CLEAR (PORTB , PB2);
	if (BIT_TEST(mask,inv -  8) ) BIT_SET( PORTB, PB3) ; else BIT_CLEAR (PORTB , PB3);
	if (BIT_TEST(mask,inv -  9) ) BIT_SET( PORTB, PB4) ; else BIT_CLEAR (PORTB , PB4);
	if (BIT_TEST(mask,inv - 10) ) BIT_SET( PORTA, PA0) ; else BIT_CLEAR (PORTA , PA0);
	if (BIT_TEST(mask,inv - 11) ) BIT_SET( PORTA, PA1) ; else BIT_CLEAR (PORTA , PA1);
	#undef inv
}

// setup hardware , timers and sleep mode
void setup()
{
	// disable analog comparator
	ACSR |= (1 << ACD);
	// disable unused modules in PRR, we are using timer1
	PRR |= (1 << PRUSI);  // disable timer/Counter1 and USI

	//PRR &= ~(1 << PRTIM1); // Ensure Timer1 is enabled

	// set led pins as output
	DDRA |= 0b00000011;				// for PA1 and PA0
	DDRB |= 0b00111111;				// for PB4 to PB0 and pb5 for mosi
	DDRD |= 0b01110011;				// for PD6, PD5, PD4, PD1 and PD0
	
	//power up mosi
	BIT_SET( PORTB, PB5);
	
	// we're on the saving bytes route
	// set hall effect sensor pin PD3 as input
	//DDRD &= ~(1 << PD3);
	
	// button
	//DDRD &= ~(1 << BUTTON);			// set as input
	PORTD |= (1 << BUTTON);			// enable internal pull-up resistor

	// Configure external interrupt for button PD2
	// Set the interrupt to trigger wake mode
	//	MCUCR |=  (1 << ISC01);
	//	MCUCR &= ~(1 << ISC01);

	//	GIMSK |= (1 << INT0);			// Enable INT0
	GIMSK |= (1 << INT0) | (1 << INT1);

	// enable external interrupt on PD3 (combine to previous)
	MCUCR |= (1 << ISC11);			// trigger on rising edge only
	MCUCR &= ~(1 << ISC10);

	
	// enable global interrupts
	sei();

	// set up timer
	setupTimer();

	// set sleep mode to one of
	set_sleep_mode(SLEEP_MODE);

}

// simple trail/cylon effect
void bounce()
{
	static uint16_t leds = 0x07; // starting pattern for the trail effect

	// direction of movement (1 for right, -1 for left)
	static int direction = 1;

	for( uint16_t i = 0; i < 254*2 ; i++)
	{
		
		if(readButton())break;
		
		setLEDS(leds);
		if (direction == 1) {
			if ((leds & 0xE000) == 0xE000) {
				direction = -1;
				} else {
				leds <<= 1;
			}
			} else {
			if ((leds & 0x0007) == 0x0007) {
				direction = 1;
				} else {
				leds >>= 1;
			}
		}
	}
}


/// draw one frame of data
void lightLedsInSequence() {
	
	switch( currentEffect  ) {
		
		case 0:
		// draw RPM meter
		drawString("RPM");
		drawNumber( rpmValue );
		_delay_adaptive(1);
		_delay_adaptive(1);
		_delay_adaptive(1);
		drawNumber( delayAdj );
		break;
		
		case 1:
		sineWaveEffect(0);
		break;
		
		case 2:
		//              THANK YOU 0123456789
		{

			drawString( GREET_TEXT );
			customDelayMicroseconds(	delayAdj * 100 );
			break;
		}
		
		case 3:
		//              THANK YOU 0123456789
		{

			scrollStringPOV(SCROLL_TEXT);
		
			customDelayMicroseconds(	delayAdj * 100 );
			break;
		}

		case 4:
		{
			// draw animated hearts
			switch(fillModeEffects) {
				case 0:
				drawImage(hack_0, sizeof(hack_0)/2);
				break;
				case 1:
				drawImage(hack_1, sizeof(hack_1)/2);
				break;
				case 2:
				drawImage(hack_2, sizeof(hack_2)/2);
				break;
				case 3:
				drawImage(hack_3, sizeof(hack_3)/2);
				break;
			}
		}
		break;
		case 5:
		bounce();
		break;
		case 6:
		sineWaveEffect(1);
		break;
		
		case 7:
		for(uint8_t i = 0 ; i < 154 ; i++ ) {
			
			setLEDS(rand());
			
			// break out when you see a HFT, but draw a couple more first
			if(isTriggered) { i=155 ;}
			if(readButton())break;
		}
		break;
		
		case 8:
		for(uint16_t i = 0 ; i < 1000 ; i++ ) {
			
			setLEDS(TCNT1);
			_delay_adaptive(i);
			
			// break out when you see a HFT, but draw a couple more first
			if(isTriggered) {i=1000 ;}
			
			// since it breaks out and sets trigger to be false, it misses the next loop... so  play around with setting isTriggered to false
			if(readButton())break;
		}
		break;
		case 9:
		for(uint16_t i = 0 ; i < 1000 ; i++ ) {
			
			setLEDS((  TCNT1/100 ) );
			_delay_adaptive(i);
			
			// break out when you see a HFT, but draw a couple more first
			if(isTriggered) {i=1000;}
			if( readButton() )break;
		}
		break;
		
		default:
		sei();
		
		//power down mosi
		BIT_CLEAR( PORTB, PB5);
		
		isSleeping = true;
		sleep_cpu();
		
		break;
		
	}
	
	// last set to turn them all off
	setLEDS(0);
}

// ISR for the button press interrupt
ISR(INT0_vect) {
	
	sleep_disable();
	
	// reset the power off timer
	powerDownCountdown = COUNTDOWN_LENGTH;
	
	//power up MOSI and the hall effect sensor
	BIT_SET( PORTB, PB5);
	
	sleep_enable();
}


// ISR for Hall effect sensor interrupt
ISR(INT1_vect) {
	
	// disable sleep on wake-up
	sleep_disable();
	
	// reset the power off timer
	powerDownCountdown = COUNTDOWN_LENGTH;
	
	// calculate us per revolution from timer
	currentTimerValue = TCNT1;
	uint16_t ticksThisRevolution;
	
	if (currentTimerValue >= lastTimerValue) {
		ticksThisRevolution = currentTimerValue - lastTimerValue;
		} else {
		ticksThisRevolution = (UINT16_MAX - lastTimerValue) + currentTimerValue + 1;
	}

	revolutionDurationMs = ticksThisRevolution / (12500 / 1000);

	// calculate led delay here
	delayAdj = (revolutionDurationMs / 4);

	rpmValue = (600000 / revolutionDurationMs);

	// mark as triggered
	isTriggered = true;

	// store for next loop
	lastTimerValue = currentTimerValue;
	totalTimerTicks += ticksThisRevolution;

	
	// enable sleep
	sleep_enable();
}

/////////////////////////////////////////////////////////////////////////
/// main
/////////////////////////////////////////////////////////////////////////

int main(void) {
	
	setup();
	
	// all off
	setLEDS(0);
	
	
	while (1) {
		
		sei();

		// if power is on, and button pressed  then change mode
		
		if (isSleeping == false ) {
			
			if ( readButton() == true ) {
				
				do {
					currentEffect ++;
					currentEffect %= MAX_EFFECTS;
					
					setLEDS(0b11<<currentEffect);

					// wait to let use see leds
					_delay_ms(300);


					setLEDS(0);
					
				} while(readButton() == true);
			}
			} else {
			
			// indicate we just woke up
			setLEDS(0xfff);
			
			// woken
			isSleeping = false;
			
			_delay_ms(300);
			
			setLEDS(0);
		}
		
		// power down hall effect sensor if counter has finished
		if( powerDownCountdown-- == 0  ) {
			
			// enable sleep mode
			sleep_enable();
			
			// indicate we are going to sleep
			setLEDS(0xaa);
			
			// turn off MOSI
			BIT_CLEAR( PORTB, PB5);
			_delay_ms(50);
			
			powerDownCountdown = COUNTDOWN_LENGTH;
			
			isSleeping = true;

			// LEDS of before sleep
			setLEDS(0x0);
			
			// put CPU to sleep
			sleep_cpu();
		}
		
		// range check
		if( powerDownCountdown >COUNTDOWN_LENGTH  ){
			powerDownCountdown = COUNTDOWN_LENGTH;
		}
		
		// run sequence if hall effect was set
		if( isTriggered ) {
			
			// reset state for hall effect. moved it here so we can use a trigger to break out of an effect and not overrun
			isTriggered = false;
			
			lightLedsInSequence();
			
			// simple timer for the animated demos
			if( count == 10 ) {
				
				// change sub mode
				fillModeEffects ++;
				
				if( fillModeEffects == 4) {
					fillModeEffects = 0;
				}
				count = 0;
			}
			
			count ++;
		}
	}
}

