/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "tetris_data/tetris_data.h"
#include "tetris_data/tetris_ui.h"
#include "adc/adc.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


void RIT_IRQHandler (void) {			
    LPC_RIT->RICTRL |= 0x1;
		ADC_start_conversion();
    
    if (game_status == Playing){
			
				if (slow_down_active) {
            if (slow_down_timer > 0){
                slow_down_timer--;
            } else {
                slow_down_active = 0;
            }
        }
				
				
				if (lcd_lock) return;
			
				
				float freq = 1.0f + (((float)AD_current / 4095.0f) * 4.0f);
        int fall_limit = (int)(20.0f / freq);
				
				if (slow_down_active && fall_limit < 20) {
            fall_limit = 20;}
        
        if (hard_drop_flag) {
						if (lcd_lock) return;
            UI_DrawActivePiece(Black);
            while (collision_check(current_x, current_y + 1, current_rotation) == 0){
                current_y++;
            }
						req_move_down = 0;
						req_fall_update = 0;
						
            hard_drop_flag = 0;
            move_timer = 0;
						req_fall_update = 1;
        }

        if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){
						req_rotate = 1;
        }
        if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){
						req_move_left = 1;
        }
        else if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){
						req_move_right = 1;
        }

        if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
						req_move_down = 1;
				}

        
        move_timer++;
        if (move_timer >= fall_limit) {
            move_timer = 0;
						req_fall_update = 1;

        }
    }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
