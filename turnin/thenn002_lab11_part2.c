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

//Demo: https://drive.google.com/open?id=1GktyGAJ-CF-1QNAzvPXEflayEDsIigpP

const unsigned char phrase[] = "CS120B is Legend... wait for it DARY!";
unsigned char column, num;

enum DisplayPhraseStates{D_Display};
int DisplayPhraseSMTick(int state) {
        switch(state) {
                case D_Display:
                        break;
                default:
			column = 32;
                        num = 0;
                        LCD_ClearScreen();
                        state = D_Display;
                        break;
        }
        switch(state) {
                case D_Display:
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
	return state;
}

int main(void){
	DDRC = 0x00; PORTC = 0xFF;
        DDRB = 0xFF; PORTB = 0x00;
        DDRD = 0xFF; PORTD = 0x00;	

	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = 1;

	task1.state = -1;
	task1.period = 300;
	task1.elapsedTime = task1.period;
	task1.TickFct = &DisplayPhraseSMTick;

	TimerSet(300);
	TimerOn();
        LCD_init();
        LCD_ClearScreen();

	unsigned short i;
	while(1) {
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period )
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 300;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
