/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "timer.h"
#include "io.h"
#include "bit.h"
#include "keypad.h"
#include "queue.h"
#include "scheduler.h"
#include "seven_seg.h"
#include "stack.h"

const unsigned char phrase[] = "CS120B is Legend... wait for it DARY!";
unsigned char column, num;

enum States{Init, Go} state;
void Tick() {
	switch(state) {
		case Init:
			column = 32;
			num = 0;
			LCD_ClearScreen();
			state = Go;
			break;
		case Go:
			break;
		default:
			break;
	}
	switch(state) {
		case Init:
                        break;
                case Go:
			if(column > 0) {
				LCD_DisplayString(column, &phrase[0]);
				column--;
			}
			else {
				if(num < 36) {
					num++;
					LCD_DisplayString(0, &phrase[num]);
				}
				else {
					column = 32;
                        		num = 0;
                        		LCD_ClearScreen();
				}
			}
                        break;
                default:
                        break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0x00; PORTC = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	TimerSet(300);
	TimerOn();
	
	state = Init;
	LCD_init();
	LCD_ClearScreen();

    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
