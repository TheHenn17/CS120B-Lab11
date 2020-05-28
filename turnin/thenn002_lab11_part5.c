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

unsigned char playerPos;
unsigned char gamePlay;
unsigned char objectsPos[] = { 7, 32 };
const unsigned char saying[] = "   Thanks for      playing!!!  ";

enum PausePlayStates{ PP_Pause, PP_Play, PP_Trans };
int PausePlaySMTick(int state) {
	switch(state) {
		case PP_Pause:
			if((~PINA & 0x04) == 0x04) { state = PP_Trans; gamePlay = 1; }
			else { state = PP_Pause; }
			break;
		case PP_Play:
			if(gamePlay == 0) { state = PP_Pause; }
			else if((~PINA & 0x04) == 0x04) { state = PP_Trans; gamePlay = 0; }
                        else { state = PP_Play; }
			break;
		case PP_Trans:
			if(((~PINA & 0x04) == 0x00) && (gamePlay == 1)) { state = PP_Play; }
			else if(((~PINA & 0x04) == 0x00) && (gamePlay == 0)) { state = PP_Pause; }
                        else { state = PP_Trans; }
			break;
		default:
			state = PP_Pause;
			gamePlay = 0;
			break;
	}
	return state;
}

enum MovePlayerStates{ MP_Up, MP_Down, MP_Pause };
int MovePlayerSMTick(int state) {
        switch(state) {
                case MP_Up:
                        if(gamePlay == 0) { state = MP_Pause; }
                        else if((~PINA & 0x07) == 0x02) { state = MP_Down; }
                        else { state = MP_Up; }
                        break;
                case MP_Down:
                        if(gamePlay == 0) { state = MP_Pause; }
                        else if((~PINA & 0x07) == 0x01) { state = MP_Up; }
                        else { state = MP_Down; }
                        break;
                case MP_Pause:
                        if(gamePlay == 1) { state = (playerPos == 2) ? MP_Up : MP_Down; }
                        else { state = MP_Pause; }
                        break;
                default:
                        state = MP_Pause;
                        playerPos = 2;
                        break;
        }
        switch(state) {
                case MP_Up:
                        playerPos = 2;
                        break;
                case MP_Down:
                        playerPos = 18;
                        break;
                case MP_Pause:
                        break;
                default:
                        break;
        }
        return state;
}

enum GameboardStates{ GB_Play, GB_Pause, GB_GameOver };
int GameboardSMTick(int state) {
	switch(state) {
		case GB_Pause:
			if(gamePlay == 1) { state = GB_Play; }
			else { state = GB_Pause; }
			break;
		case GB_Play:
			if((playerPos == objectsPos[0]) || (playerPos == objectsPos[1])) {
				state = GB_GameOver;
				gamePlay = 0;
				LCD_ClearScreen();
				LCD_DisplayString(1, saying);
			}
			else if(gamePlay == 1) { state = GB_Play; }
                        else { state = GB_Pause; }
                        break;
		case GB_GameOver:
			if(gamePlay == 1) {
				state = GB_Play;
				LCD_ClearScreen();
				objectsPos[0] = 7;
				objectsPos[1] = 32;
			}
                        else { state = GB_GameOver; }
			break;
		default:
			state = GB_Pause;
			LCD_Cursor(objectsPos[0]);
			LCD_WriteData('#');
			LCD_Cursor(objectsPos[1]);
                        LCD_WriteData('#');
			LCD_Cursor(playerPos);
			break;
	}
	switch(state) {
        	case GB_Pause:
                        break;
                case GB_Play:
			LCD_Cursor(objectsPos[0]);
                        LCD_WriteData(' ');
			if(objectsPos[0] > 1) { objectsPos[0]--; }
			else { objectsPos[0] = 16; }
			LCD_Cursor(objectsPos[0]);
                        LCD_WriteData('#');
			LCD_Cursor(objectsPos[1]);
                        LCD_WriteData(' ');
                        if(objectsPos[1] > 17) { objectsPos[1]--; }
                        else { objectsPos[1] = 32; }
                        LCD_Cursor(objectsPos[1]);
                        LCD_WriteData('#');
			LCD_Cursor(playerPos);
                        break;
		case GB_GameOver:
			break;
                default:
                        break;
        }
	return state;
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xF0; PORTC = 0x0F;
        DDRB = 0xFF; PORTB = 0x00;
        DDRD = 0xFF; PORTD = 0x00;	

	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned short numTasks = 3;

	task1.state = -1;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &PausePlaySMTick;

	task2.state = -1;
        task2.period = 50;
        task2.elapsedTime = task2.period;
        task2.TickFct = &MovePlayerSMTick;

	task3.state = -1;
        task3.period = 100;
        task3.elapsedTime = task3.period;
        task3.TickFct = &GameboardSMTick;

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
