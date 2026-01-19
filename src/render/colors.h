/*
 * 256-color theme system for ASCII Dancer
 * Provides energy-based color gradients and multiple themes
 */

#ifndef COLORS_H
#define COLORS_H

#include "../config/config.h"
#include <ncurses.h>

/* Color pair IDs (1-255 available in ncurses with 256 color support) */
#define COLOR_PAIR_DANCER_BASE     10   /* Base dancer colors (10-30) */
#define COLOR_PAIR_SHADOW_BASE     40   /* Shadow colors (40-50) */
#define COLOR_PAIR_GROUND          60   /* Ground line */
#define COLOR_PAIR_BAR_BASS        70   /* Bass bar gradient (70-80) */
#define COLOR_PAIR_BAR_MID         90   /* Mid bar gradient (90-100) */
#define COLOR_PAIR_BAR_TREBLE     110   /* Treble bar gradient (110-120) */
#define COLOR_PAIR_INFO           130   /* Info text */
#define COLOR_PAIR_BPM            131   /* BPM display */
#define COLOR_PAIR_ENERGY         132   /* Energy meter */

/* Number of gradient steps for smooth transitions */
#define GRADIENT_STEPS 10

/* Theme color structure */
typedef struct {
    /* Dancer body gradient (low energy to high energy) */
    short dancer_colors[GRADIENT_STEPS];
    
    /* Shadow color (dimmed version) */
    short shadow_color;
    
    /* Ground line color */
    short ground_color;
    
    /* Frequency bar colors */
    short bass_color;
    short mid_color;
    short treble_color;
    
    /* UI colors */
    short info_color;
    short bpm_color;
    
    /* Background (-1 for transparent) */
    short background;
} ThemeColors;

/* ============ Functions ============ */

/* Initialize color system (call after initscr, start_color) */
int colors_init(void);

/* Apply a color theme */
void colors_apply_theme(ColorTheme theme);

/* Get color pair for dancer based on energy level (0.0-1.0) */
int colors_get_dancer_pair(float energy);

/* Get color pair for shadow based on energy level */
int colors_get_shadow_pair(float energy);

/* Get color pair for ground line */
int colors_get_ground_pair(void);

/* Get color pairs for frequency bars */
int colors_get_bass_pair(float intensity);
int colors_get_mid_pair(float intensity);
int colors_get_treble_pair(float intensity);

/* Get UI color pairs */
int colors_get_info_pair(void);
int colors_get_bpm_pair(void);
int colors_get_energy_pair(float energy);

/* Check if 256 colors are supported */
int colors_has_256(void);

/* Get theme preview string (for theme selection UI) */
const char* colors_get_theme_preview(ColorTheme theme);

#endif /* COLORS_H */
