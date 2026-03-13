#ifndef __TETRIS_UI_H
#define __TETRIS_UI_H

#include "GLCD/GLCD.h"
#include <stdint.h>

/* UI Constants */
#define BLOCK_SIZE  10    
#define MAP_X_START 10    /* Game field */
#define MAP_Y_START 10    
#define INFO_X_START 120  /* Game Stats */


void UI_Init(void);
void UI_DrawBlock(int x, int y, uint16_t color);
void UI_RefreshStats(void);
void UI_RefreshRow(int row);
void UI_DrawActivePiece(uint16_t color);
void UI_ShowGameOver(void);

#endif