#include "button.h"
#include "LPC17xx.h"
#include "tetris_data/tetris_data.h"
#include "timer/timer.h"
#include <stdlib.h>
#include "adc/adc.h"


extern volatile GameState game_status;
extern int first_start;
extern int hard_drop_flag;

void EINT0_IRQHandler (void) {
    LPC_SC->EXTINT &= (1 << 0);
}


void EINT1_IRQHandler (void) {
		NVIC_DisableIRQ(EINT1_IRQn);
		enable_timer(1);
		LPC_PINCON->PINSEL4    &= ~(1 << 22);
	
		if (first_start) {
				first_start = 0;
				stopBackgroundTheme();
				unsigned int noise_seed = LPC_RIT->RICOUNTER + AD_current + LPC_TIM0->TC;
				srand(noise_seed); 
				new_random_tetromino();
				game_status = Playing;
		}
		else if (game_status == Game_Over) {
				unsigned int noise_seed = LPC_RIT->RICOUNTER + AD_current + LPC_TIM0->TC;
				srand(noise_seed);
				reset_game();
			
//				new_random_tetromino();
		}
		else {
				if (game_status == Playing) 
						game_status = Paused;
				else if (game_status == Paused) 
						game_status = Playing;
		}
		
    LPC_SC->EXTINT &= (1 << 1);
}


void EINT2_IRQHandler (void) {
    if (game_status == Playing) {
        hard_drop_flag = 1;
    }
		NVIC_DisableIRQ(EINT2_IRQn);
    enable_timer(2);
		LPC_PINCON->PINSEL4    &= ~(1 << 24);
		
    LPC_SC->EXTINT &= (1 << 2);     
}


