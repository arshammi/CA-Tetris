#include "tetris_ui.h"
#include "tetris_data.h"
#include <stdio.h>


void UI_Init(void) {
    LCD_Clear(Black);
    
    LCD_DrawLine(MAP_X_START - 1, MAP_Y_START - 1, MAP_X_START + 100, MAP_Y_START - 1, White);
    LCD_DrawLine(MAP_X_START - 1, MAP_Y_START - 1, MAP_X_START - 1, MAP_Y_START + 200, White);
    LCD_DrawLine(MAP_X_START + 100, MAP_Y_START - 1, MAP_X_START + 100, MAP_Y_START + 200, White);
    LCD_DrawLine(MAP_X_START - 1, MAP_Y_START + 200, MAP_X_START + 100, MAP_Y_START + 200, White);
    
    UI_RefreshStats();
}

void UI_DrawBlock(int x, int y, uint16_t color) {
    if (y < 0) return;
    int px = MAP_X_START + (x * BLOCK_SIZE);
    int py = MAP_Y_START + (y * BLOCK_SIZE);
    
		int i;
		int j;
    for(i = 0; i < BLOCK_SIZE - 1; i++) {
        for(j = 0; j < BLOCK_SIZE - 1; j++) {
            LCD_SetPoint(px + i, py + j, color);
        }
    }
}


void UI_RefreshStats(void) {
    char label[20];
    
    sprintf(label, "Top: %06d", best_score);
    GUI_Text(INFO_X_START, 20, (uint8_t *)label, White, Black);
    
    sprintf(label, "Score: %06d", current_score);
    GUI_Text(INFO_X_START, 50, (uint8_t *)label, White, Black);
    
    sprintf(label, "Lines: %03d", lines_count);
    GUI_Text(INFO_X_START, 80, (uint8_t *)label, White, Black);
}


void UI_DrawPowerup(int x, int y, uint8_t type) {
    int px = MAP_X_START + (x * BLOCK_SIZE);
    int py = MAP_Y_START + (y * BLOCK_SIZE);
    int i, j;

    for(i = 0; i < BLOCK_SIZE - 1; i++) {
        for(j = 0; j < BLOCK_SIZE - 1; j++) {
            LCD_SetPoint(px + i, py + j, White);
        }
    }
    
    if (type == POWERUP_HALF) {
        for(j = 2; j < BLOCK_SIZE - 3; j++) LCD_SetPoint(px + 4, py + j, Black);

        for(i = 2; i < BLOCK_SIZE - 3; i++) LCD_SetPoint(px + i, py + 4, Black);

        LCD_SetPoint(px + 2, py + 2, Black);
        LCD_SetPoint(px + 6, py + 2, Black);
        LCD_SetPoint(px + 2, py + 6, Black);
        LCD_SetPoint(px + 6, py + 6, Black);
    } 
    else if (type == POWERUP_SLOW) {

        for(i = 2; i < BLOCK_SIZE - 3; i++) {
            LCD_SetPoint(px + i, py + 4, Black);
        }
    }
}


void UI_RefreshRow(int row) {
    int col;
    for (col = 0; col < COLS; col++) {
        uint8_t val = game_field[row][col];
        
        if (val == 0) {
            UI_DrawBlock(col, row, Black);
        } else if (val < 8) {
            // Standard Tetromino (1-7)
            UI_DrawBlock(col, row, tetromino_color[val - 1]);
        } else {
            // Powerup (8 or 9)
            UI_DrawPowerup(col, row, val);
        }
    }
}


void UI_DrawActivePiece(uint16_t color) {
		int i;
    for (i = 0; i < 4; i++) {
        int tx = current_x + shapes_database[current_shape][current_rotation].x_offset[i];
        int ty = current_y + shapes_database[current_shape][current_rotation].y_offset[i];
        UI_DrawBlock(tx, ty, color);
    }
}


void UI_ShowGameOver(void) {
    LCD_Clear(Black);
    GUI_Text(65, 100, (uint8_t *) " * GAME OVER * ", Red, White);
    
    char final_score[20];
    sprintf(final_score, "Score: %06d", current_score);
    GUI_Text(65, 130, (uint8_t *) final_score, White, Black);
    
    GUI_Text(35, 160, (uint8_t *) "PRESS KEY1 TO RESTART", Cyan, Black);
}
/******************************************************************************
**                            End Of File
******************************************************************************/
