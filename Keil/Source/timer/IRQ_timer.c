/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "GLCD.h" 
#include "TouchPanel.h"
#include <stdio.h>
#include "tetris_data/tetris_data.h"
#include "music/music.h"


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};


void TIMER0_IRQHandler (void)
{
	static int sineticks=0;
	// DAC management
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue -= 410;
	currentValue /= 2;
	currentValue += 410;
	LPC_DAC->DACR = currentValue <<6;
	sineticks++;
	if(sineticks==45) sineticks=0;

  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		LPC_TIM1->IR = 1;
		return;
	}
	
	else {	// button released
		disable_timer(1);
		reset_timer(1);
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
	}
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}



/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
		LPC_TIM2->IR = 1;
		return;
	}
	
	else {	// button released	
		disable_timer(2);
		reset_timer(2);
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
	}
	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
    disable_timer(0);
    reset_timer(0);
    disable_timer(3);
    reset_timer(3);

    if (background_music_enabled) {
        theme_index++;
        
        if (theme_music[theme_index].duration == 0) {
            theme_index = 0;
        }
        
        playNote(theme_music[theme_index]);
    }

    LPC_TIM3->IR = 1; 
    return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
