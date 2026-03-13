#include "music.h"
#include "timer.h"
#include "LPC17xx.h"
#include <stdio.h>


static NOTE* current_sound = NULL;
static int note_index = 0;


// move_piece
NOTE sound_move[] = {
    {c5, time_semibiscroma},
    {pause, 0}
};

// rotate_piece
NOTE sound_rotate[] = {
    {a4, time_semibiscroma},
    {c5, time_semibiscroma},
    {pause, 0}
};

// piece_landed
NOTE sound_landed[] = {
    {c3, time_semicroma},
    {pause, 0}
};

// tetris_4_lines
NOTE sound_clear_4[] = {
    {b4, time_semicroma}, {c5, time_semicroma}, {d5, time_semicroma}, 
    {g5, time_minima},
    {pause, 0}
};

// game_over
NOTE sound_game_over[] = {
    {c4, time_croma}, {b3, time_croma}, {a3, time_croma}, {g3, time_semiminima},
    {pause, 0}
};

// clear_1
NOTE sound_clear_1[] = { 
    {g4, time_semicroma}, 
    {c5, time_semicroma}, 
    {pause, 0} 
};

// powerup_inserted
NOTE sound_powerup[] = { 
    {c4, time_semibiscroma}, 
    {e4, time_semibiscroma}, 
    {g4, time_semibiscroma}, 
    {c5, time_semibiscroma}, 
    {pause, 0} 
};

// Background Music
NOTE theme_music[] = {
    {e5, time_semiminima}, {b4, time_croma}, {c5, time_croma}, {d5, time_semiminima}, {c5, time_croma}, {b4, time_croma},
    {a4, time_semiminima}, {a4, time_croma}, {c5, time_croma}, {e5, time_semiminima}, {d5, time_croma}, {c5, time_croma},
    {b4, time_semiminima}, {b4, time_croma}, {c5, time_croma}, {d5, time_semiminima}, {e5, time_semiminima},
    {c5, time_semiminima}, {a4, time_semiminima}, {a4, time_semiminima}, {pause, time_semiminima},
    {pause, 0} 
};

int theme_index = 0;
volatile BOOL background_music_enabled = FALSE;


void playBackgroundTheme(void) {
    background_music_enabled = TRUE;
    theme_index = 0;
    playNote(theme_music[theme_index]);
}

void stopBackgroundTheme(void) {
    background_music_enabled = FALSE;
    disable_timer(0);
    disable_timer(3);
}

void playNote(NOTE note)
{
    if(note.freq != pause) {
        reset_timer(0);
        init_timer(0, note.freq);
        enable_timer(0);
    }

    reset_timer(3);
    init_timer(3, note.duration);
    enable_timer(3);
}

BOOL isNotePlaying(void) {
    return ((LPC_TIM0->TCR != 0) || (LPC_TIM3->TCR != 0));
}