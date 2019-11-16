#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"

const int max_revs_per_second = 2;   // Hysteresis through rate limiting
const int write_every_n_cycles = 1;  // Write less frequently to prolong EEPROM life
const int clockPin = 0;
const int latchPin = 1;
const int dataPin = 2;
const int buttonPin = 3;
const int magnetPin = 4;

uint32_t EEMEM storedRevs;

void shiftOutMSB(uint8_t data)
{
	PORTB &= ~(1 << latchPin);  // Latch low
	for (int i = 0; i < 8; i++)
	{
		PORTB &= ~(1 << clockPin);  // Clock low
		PORTB &= ~(1 << dataPin);  // Data low
		PORTB |= ((data & 0x80) >> 7 << dataPin);  // Set the data pin to the value of the first bit, i.e. MSB first
		data <<= 1;
		PORTB |= (1 << clockPin);  // Clock high
	}
	PORTB |= (1 << latchPin);  // Latch high
}

void delay(double ms)
{
	while(--ms > 0)
	{
		_delay_ms(1);
	}
}

void showRevs(uint32_t revs, uint16_t dispTime_ms)
{
	shiftOutMSB((revs & 0xFF000000) >> 24);
	delay(dispTime_ms);
	shiftOutMSB((revs & 0x00FF0000) >> 16);
	delay(dispTime_ms);
	shiftOutMSB((revs & 0x0000FF00) >> 8);
	delay(dispTime_ms);
	shiftOutMSB(revs & 0x000000FF);
	delay(dispTime_ms);
	shiftOutMSB(0);
}

void showNumber(uint8_t num)
{
	/*
	Mapped as shift register -> 7 segment
	0 -> G
	1 -> F
	2 -> A
	3 -> B
	4 -> E
	5 -> D
	6 -> C
	7 -> P
	*/
	switch (num)
	{
		case 0:
			shiftOutMSB(0b01111110);
			break;
		case 1:
			shiftOutMSB(0b00010010);
			break;
		case 2:
			shiftOutMSB(0b10111100);
			break;
		case 3:
			shiftOutMSB(0b10110110);
			break;
		case 4:
			shiftOutMSB(0b11010010);
			break;
		case 5:
			shiftOutMSB(0b11100110);
			break;
		case 6:
			shiftOutMSB(0b11101110);
			break;
		case 7:
			shiftOutMSB(0b00110010);
			break;
		case 8:
			shiftOutMSB(0b11111110);
			break;
		case 9:
			shiftOutMSB(0b11110110);
			break;
		default:
			shiftOutMSB(0b00000101);  // Underscore and period
	}
}

int main(void)
{
	DDRB = 0;  // Default all to read, except the following
	DDRB |= (1<<clockPin);
	DDRB |= (1<<latchPin);
	DDRB |= (1<<dataPin);
	uint32_t revs = eeprom_read_dword(&storedRevs) * write_every_n_cycles;
	uint8_t magnetState = 0;
	shiftOutMSB(0);
	while(1)
	{
		showNumber(0);
		_delay_ms(500);
		showNumber(1);
		_delay_ms(500);
		showNumber(2);
		_delay_ms(500);
		showNumber(3);
		_delay_ms(500);
		showNumber(4);
		_delay_ms(500);
		showNumber(5);
		_delay_ms(500);
		showNumber(6);
		_delay_ms(500);
		showNumber(7);
		_delay_ms(500);
		showNumber(8);
		_delay_ms(500);
		showNumber(9);
		_delay_ms(500);
	}

	
	/*
	while (1)
	{
		if(PINB & (1 << buttonPin))  // Display button pushed
		{
			showRevs(revs, 1000);
		}
		
		if(PINB & (1 << magnetPin))  // Magnet triggered
		{
			if (!magnetState)  // Only act when the flag goes from low to high
			{
				revs++;

				
				if (revs % write_every_n_cycles == 0)  // Only store every nth rotation
				{
					eeprom_write_dword(&storedRevs, revs / write_every_n_cycles);
				}
				#if (DEBUG)
				showRevs(revs, 1000 / max_revs_per_second / 4);
				#else
				_delay_ms(1000 / max_revs_per_second);
				#endif
				
			}
			magnetState = 1;  // Set the flag any time the magnet is triggered
		}
		else  // Reset the flag when the magnet isn't triggered
		{
			magnetState = 0;
		}
	}
	*/
}

