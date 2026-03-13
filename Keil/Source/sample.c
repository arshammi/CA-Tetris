/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "tetris_data/tetris_data.h"
#include "tetris_data/tetris_ui.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "music/music.h"

//#ifdef __USE_CMSIS
//#include "LPC17xx.h"
//#endif


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();
  LCD_Initialization();
	UI_Init();
	joystick_init();
	LPC_PINCON->PINSEL1 |= (2 << 20);
	
	init_timer(0, 0);
	init_timer (1, 0x1312D0);
	init_timer (2, 0x1312D0);
	init_timer(3, 0);
	
	ADC_init();
	playBackgroundTheme();
	ADC_start_conversion();
	
	game_status = Paused;
  first_start = 1;
	
	init_RIT (0x004C4B40);
	enable_RIT();
	
	__enable_irq();
	
	while (1) {
			if (game_status == Playing) {
          if (background_music_enabled) {
                stopBackgroundTheme();
            }
					
					// Rotate
					if (req_rotate) {
							req_rotate = 0;
							int next_rot = (current_rotation + 1) % 4;
							if (collision_check(current_x, current_y, next_rot) == 0) {
									UI_DrawActivePiece(Black);
									current_rotation = next_rot;
									UI_DrawActivePiece(tetromino_color[current_shape]);
									playNote(sound_rotate[0]);
							}
					}

					// Left Move
					if (req_move_left) {
							req_move_left = 0;
							if (collision_check(current_x - 1, current_y, current_rotation) == 0) {
									UI_DrawActivePiece(Black);
									current_x--;
									UI_DrawActivePiece(tetromino_color[current_shape]);
									playNote(sound_move[0]);
							}
					}

					// Right Move
					if (req_move_right) {
							req_move_right = 0;
							if (collision_check(current_x + 1, current_y, current_rotation) == 0) {
									UI_DrawActivePiece(Black);
									current_x++;
									UI_DrawActivePiece(tetromino_color[current_shape]);
									playNote(sound_move[0]);
							}
					}
					
					// Down Move
					if (req_move_down) {
							req_move_down = 0;
							
							// Collision Check
							if (collision_check(current_x, current_y + 1, current_rotation) == 0) {
									UI_DrawActivePiece(Black);
									current_y++;
									UI_DrawActivePiece(tetromino_color[current_shape]);
									playNote(sound_move[0]);
							} 
					}

					// Falling
					if (req_fall_update) {
							req_fall_update = 0;
							if (collision_check(current_x, current_y + 1, current_rotation) == 0) {
									UI_DrawActivePiece(Black);
									current_y++;
									UI_DrawActivePiece(tetromino_color[current_shape]);
							} else {
									// Piece landed
									UI_DrawActivePiece(tetromino_color[current_shape]);
									stop_tetromino();
									playNote(sound_landed[0]);
									check_and_clear_lines(current_y);
									new_random_tetromino();
									
									// Game Over Check
									if (collision_check(current_x, current_y, current_rotation)) {
											game_status = Game_Over;
											playNote(sound_game_over[0]);
											playNote(sound_game_over[0]);
											playNote(sound_game_over[0]);
											playNote(sound_game_over[0]);
											playNote(sound_game_over[0]);
											refresh_game_over_flag = 1;
											playBackgroundTheme();
											if (current_score > best_score) best_score = current_score;
											disable_RIT();
									}
							}
					}

					
					if (refresh_stats_flag) {
							lcd_lock = 1;
							UI_RefreshStats();
							refresh_stats_flag = 0;
							lcd_lock = 0;
					}

					if (refresh_game_field_flag) {
							lcd_lock = 1;
							int row;
							for(row = 0; row < ROWS; row++) {
									UI_RefreshRow(row);
							}
							UI_RefreshStats();
							refresh_game_field_flag = 0;
							lcd_lock = 0;
					}
			} 
      else if (game_status == Game_Over && refresh_game_over_flag) {
					lcd_lock = 1;
					UI_ShowGameOver();
					refresh_game_over_flag = 0;
					lcd_lock = 0;
			}
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
