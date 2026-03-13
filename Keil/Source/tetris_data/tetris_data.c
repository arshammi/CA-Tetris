#include <stdlib.h>
#include "GLCD.h"
#include "tetris_data.h"
#include "tetris_ui.h"
#include "music/music.h"

uint8_t game_field[ROWS][COLS] = {0};

int current_x = 0;
int current_y = 0;
int current_rotation = 0;
int current_shape = 0;

int powerup_line_counter = 0;
int slow_down_active = 0;
int slow_down_timer = 0;

int move_timer = 0;

unsigned int current_score = 0;
unsigned int best_score = 0;
unsigned int lines_count = 0;
unsigned int milestone_counter = 0;

volatile GameState game_status = Paused;
int first_start;
int hard_drop_flag = 0;

volatile int refresh_game_field_flag = 0;
volatile int refresh_stats_flag = 0;

volatile int refresh_game_over_flag = 0;
volatile int lcd_lock = 0;

volatile int req_rotate = 0;
volatile int req_move_left = 0;
volatile int req_move_right = 0;
volatile int req_move_down = 0;
volatile int req_fall_update = 0;

const Tetromino shapes_database[7][4] = {
    /* I-Shaped */
    {
        {{-1, 0, 1, 2}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {-1, 0, 1, 2}},
        {{-1, 0, 1, 2}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {-1, 0, 1, 2}}
    },
    /* O-shaped */
    {
        {{0, 1, 0, 1}, {0, 0, 1, 1}},
        {{0, 1, 0, 1}, {0, 0, 1, 1}},
        {{0, 1, 0, 1}, {0, 0, 1, 1}},
        {{0, 1, 0, 1}, {0, 0, 1, 1}}
    },
		
    /* T-shaped */
    {
        {{-1, 0, 1, 0}, {0, 0, 0, 1}},
        {{0, 0, 0, -1}, {-1, 0, 1, 0}},
        {{-1, 0, 1, 0}, {0, 0, 0, -1}},
        {{0, 0, 0, 1}, {-1, 0, 1, 0}}
    },
    /* J-shaped */
    {
        {{0, 0, 0, -1}, {0, 1, 2, 2}},
        {{-1, 0, 1, 1}, {0, 0, 0, 1}},
        {{0, 1, 0, 0}, {0, 0, 1, 2}},
        {{-1, -1, 0, 1}, {0, 1, 1, 1}}
    },
    /* L-shaped */
    {
        {{0, 0, 0, 1}, {0, 1, 2, 2}},
        {{-1, 0, 1, 1}, {1, 1, 1, 0}},
        {{-1, 0, 0, 0}, {0, 0, 1, 2}},
        {{-1, -1, 0, 1}, {1, 2, 1, 1}}
    },
    /* S-shaped */
    {
        {{0, 1, 0, -1}, {0, 0, 1, 1}},
        {{0, 0, 1, 1}, {0, 1, 1, 2}},
        {{0, 1, 0, -1}, {0, 0, 1, 1}},
        {{0, 0, 1, 1}, {0, 1, 1, 2}}
    },
		
    /* Z-shaped */
    {
        {{-1, 0, 0, 1}, {0, 0, 1, 1}},
        {{1, 1, 0, 0}, {0, 1, 1, 2}},
        {{-1, 0, 0, 1}, {0, 0, 1, 1}},
        {{1, 1, 0, 0}, {0, 1, 1, 2}}
    }
};

const uint16_t tetromino_color[7] = {Cyan, Yellow, Magenta, Blue, Orange, Green, Red};


void new_random_tetromino(void) {
    current_shape = rand() % 7;
    current_rotation = 0;

    current_x = 4; 
    current_y = 0;
		UI_DrawActivePiece(tetromino_color[current_shape]);
}


int collision_check(int next_x, int next_y, int next_rot) {
		int i;
    for (i = 0; i < 4; i++) {
        int target_x = next_x +shapes_database[current_shape][next_rot].x_offset[i];
        int target_y = next_y + shapes_database[current_shape][next_rot].y_offset[i];
        
        if (target_x < 0 || target_x >= COLS || target_y >= ROWS) {
            return 1; 
        }
        
        if (target_y >= 0) {
            if (game_field[target_y][target_x] != 0){
                return 1;
            }
        }
    }
    return 0;
}


void stop_tetromino(void) {
    int i;
    for (i = 0; i < 4; i++) {
        int tx = current_x + shapes_database[current_shape][current_rotation].x_offset[i];
        int ty = current_y + shapes_database[current_shape][current_rotation].y_offset[i];
        
        if (ty >= 0 && ty < ROWS && tx >= 0 && tx < COLS) {
            game_field[ty][tx] = current_shape + 1;
        }
    }
    current_score += 10;
}

static int occupied_cells[ROWS * COLS][2];
void spawn_powerup(void) {
    int count = 0;
    int r, c;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (game_field[r][c] > 0 && game_field[r][c] < 8) {
                occupied_cells[count][0] = r;
                occupied_cells[count][1] = c;
                count++;
            }
        }
    }

    if (count > 0) {
        int target = rand() % count;
        int p_type = (rand() % 2 == 0) ? POWERUP_HALF : POWERUP_SLOW;
        game_field[occupied_cells[target][0]][occupied_cells[target][1]] = p_type;
        refresh_game_field_flag = 1;
    }
}


static void inject_garbage_line(void) {
    int r, c, holes;

    for (c = 0; c < COLS; c++) {
        if (game_field[0][c] != 0) {
            game_status = Game_Over;
            refresh_game_over_flag = 1;
            return;
        }
    }

    for (r = 0; r < ROWS - 1; r++) {
        for (c = 0; c < COLS; c++) {
            game_field[r][c] = game_field[r + 1][c];
        }
    }
    for (c = 0; c < COLS; c++) {
        game_field[ROWS - 1][c] = (rand() % 7) + 1;
    }

    holes = 0;
    while (holes < 3) {
        int pos = rand() % COLS;
        if (game_field[ROWS - 1][pos] != 0) {
            game_field[ROWS - 1][pos] = 0;
            holes++;
        }
    }
    refresh_game_field_flag = 1;
}






void check_and_clear_lines(int landed_piece_y) {
    int r, c, row_up; 
    int lines_found_this_turn = 0;
    int half_powerup_found = 0;
    int slow_powerup_found = 0;
    int first_occupied_row = -1;
    int last_occupied_row = -1;

    refresh_stats_flag = 1;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (game_field[r][c] != 0) {
                if (first_occupied_row == -1) first_occupied_row = r;
                last_occupied_row = r;
                break; 
            }
        }
    }

    // Identify all full lines and check for power-ups only within those lines
    int full_line_indices[ROWS]; 
    int full_count = 0;

    for (r = 0; r < ROWS; r++) {
        int is_full = 1;
        int current_row_has_half = 0;
        int current_row_has_slow = 0;

        for (c = 0; c < COLS; c++) {
            if (game_field[r][c] == 0) {
                is_full = 0;
                break;
            }
            // Check for power-ups while scanning this specific row
            if (game_field[r][c] == POWERUP_HALF) current_row_has_half = 1;
            if (game_field[r][c] == POWERUP_SLOW) current_row_has_slow = 1;
        }

        if (is_full) {
            full_line_indices[full_count++] = r;
            // Only trigger flags if the line is actually full
            if (current_row_has_half) half_powerup_found = 1;
            if (current_row_has_slow) slow_powerup_found = 1;
        }
    }

    // Determine logic path: Half-Clear Powerup vs. Normal Clearing
    BOOL use_half_clear = FALSE;
    if (half_powerup_found && first_occupied_row != -1) {
        int total_occupied_count = last_occupied_row - first_occupied_row + 1;
        int half_to_clear = total_occupied_count / 2;

        if (half_to_clear < total_occupied_count && half_to_clear > 0) {
            use_half_clear = TRUE;
        }
    }

    // Execution phase
    if (use_half_clear) {
        apply_clear_half();
        playNote(sound_powerup[0]);
    } else if (full_count > 0) {
				int i;
        // Clear full lines from top to bottom
        for (i = 0; i < full_count; i++) {
            int row_to_clear = full_line_indices[i];
            lines_found_this_turn++;
            powerup_line_counter++;
            
            for (row_up = row_to_clear; row_up > 0; row_up--) {
                for (c = 0; c < COLS; c++) {
                    game_field[row_up][c] = game_field[row_up - 1][c];
                }
            }
            for (c = 0; c < COLS; c++) game_field[0][c] = 0;
        }
        
        // Scoring logic
        if (lines_found_this_turn == 4) {
            current_score += 600;
            playNote(sound_clear_4[0]);
        } else if (lines_found_this_turn > 0) {
            current_score += (100 * lines_found_this_turn);
            playNote(sound_clear_1[0]);
        }

        if (slow_powerup_found) {
            slow_down_active = 1;
            slow_down_timer = 300;
        }
    }

    // Update Global Counters and refresh
    if (lines_found_this_turn > 0 || use_half_clear) {
        lines_count += lines_found_this_turn;
        milestone_counter += lines_found_this_turn;

        if (powerup_line_counter >= 5) {
            spawn_powerup();
            powerup_line_counter %= 5;
            playNote(sound_powerup[0]);
        }
				
        while (milestone_counter >= 10) {
            inject_garbage_line();
            milestone_counter -= 10;
        }
        refresh_game_field_flag = 1;
    }
}



void apply_clear_half(void) {
    int r, c, row_up, i;
    int first_occupied = -1;
    int last_occupied = -1;

    // Identify the range of occupied rows
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (game_field[r][c] != 0) {
                if (first_occupied == -1) first_occupied = r;
                last_occupied = r;
                break;
            }
        }
    }

    if (first_occupied == -1) return;

    int total_occupied = last_occupied - first_occupied + 1;
    int lines_to_remove = total_occupied / 2;

    for (i = 0; i < lines_to_remove; i++) {
        for (row_up = last_occupied; row_up > 0; row_up--) {
            for (c = 0; c < COLS; c++) {
                game_field[row_up][c] = game_field[row_up - 1][c];
            }
        }
        for (c = 0; c < COLS; c++) game_field[0][c] = 0;
        
    }

    // Update game statistics
    if (lines_to_remove > 0) {
        int temp = lines_to_remove;

        while (temp >= 4) { 
            current_score += 600; 
            temp -= 4; 
        }
        current_score += (temp * 100);
        
        lines_count += lines_to_remove;
        powerup_line_counter += lines_to_remove;
        milestone_counter += lines_to_remove;
        refresh_game_field_flag = 1;
    }
}

void reset_game(void) {
    int r, c;
    
    // Clear Field
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            game_field[r][c] = 0;
        }
    }
    
    // Reset Counters
    current_score = 0;
    lines_count = 0;
    hard_drop_flag = 0;
    milestone_counter = 0;
    powerup_line_counter = 0;
    slow_down_active = 0;
    
    // Reset Logic Flags
    req_rotate = 0;
    req_move_left = 0;
    req_move_right = 0;
    req_move_down = 0;
    req_fall_update = 0;
		
		move_timer = 0;

    refresh_game_field_flag = 0;
    refresh_stats_flag = 0;
    refresh_game_over_flag = 0;
    lcd_lock = 0;

    UI_Init();
		new_random_tetromino();
    reset_RIT();
    enable_RIT();
		
		game_status = Playing;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
