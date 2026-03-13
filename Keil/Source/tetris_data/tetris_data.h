#ifndef __TETRIS_DATA_H
#define __TETRIS_DATA_H

#include <stdint.h>

#define ROWS 20
#define COLS 10

/* Shape IDs */
#define SHAPE_I 0
#define SHAPE_O 1
#define SHAPE_T 2
#define SHAPE_J 3
#define SHAPE_L 4
#define SHAPE_S 5
#define SHAPE_Z 6

#define POWERUP_HALF 8
#define POWERUP_SLOW 9

extern uint8_t game_field[ROWS][COLS];

extern int current_x;
extern int current_y;
extern int current_rotation;
extern int current_shape;

extern unsigned int current_score;
extern unsigned int best_score;
extern unsigned int lines_count;

typedef struct {
    int8_t x_offset[4];
    int8_t y_offset[4];
} Tetromino;

extern const Tetromino shapes_database[7][4];
extern const uint16_t tetromino_color[7];

void new_random_tetromino(void);

typedef enum {
    Paused = 0,
    Playing,
    Game_Over
} GameState;

extern volatile GameState game_status;

extern int first_start;
extern int hard_drop_flag;

int collision_check(int next_x, int next_y, int next_rot);

extern volatile int refresh_game_field_flag;
extern volatile int refresh_stats_flag;

extern volatile int refresh_game_over_flag; 
extern volatile int lcd_lock;

extern int powerup_line_counter;
extern int slow_down_active;
extern int slow_down_timer;

void spawn_powerup(void);
void apply_clear_half(void);

extern int move_timer;

extern volatile int req_rotate;
extern volatile int req_move_left;
extern volatile int req_move_right;
extern volatile int req_move_down;
extern volatile int req_fall_update;

#endif
/*****************************************************************************
**                            End Of File
******************************************************************************/
