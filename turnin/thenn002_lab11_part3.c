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

unsigned char input, data;

enum KeypadStates{K_Wait, K_Retrieve};
int KeypadSMTick(int state) {
	switch(state) {
		case K_Wait:
			if(input == '\0') {
				state = K_Wait;
			}
			else {
				state = K_Retrieve;
			}
			break;
		case K_Retrieve:
			state = K_Wait;
			break;
		default:
			state = K_Wait;
			input = data = '\0';
			break;
	}
	switch(state) {
		case K_Wait:
			input = GetKeypadKey();
			break;
		case K_Retrieve:
			data = input;
			input = '\0';
			break;
		default: break;
	}
	return state;
}

enum DisplayDataStates{D_Display};
int DisplayDataSMTick(int state) {
	switch(state) {
		case D_Display:
			break;
		default:
			state = D_Display;
			break;
	}
	switch(state) {
		case D_Display:
			if(data != '\0') { LCD_Cursor(1); LCD_WriteData(data); }
                        break;
                default:
                        break;
	}
	return state;
}

int main(void){
	DDRC = 0xF0; PORTC = 0x0F;
        DDRB = 0xFF; PORTB = 0x00;
        DDRD = 0xFF; PORTD = 0x00;	

	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = 2;

	task1.state = -1;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &KeypadSMTick;

	task2.state = -1;
        task2.period = 50;
        task2.elapsedTime = task2.period;
        task2.TickFct = &DisplayDataSMTick;

	TimerSet(50);
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
			tasks[i]->elapsedTime += 50;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
