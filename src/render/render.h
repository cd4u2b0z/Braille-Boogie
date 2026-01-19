// Render interface with v2.1 features
#pragma once

#include "../dancer/dancer.h"
#include "../config/config.h"

// Initialize rendering (includes 256-color setup)
int render_init(void);

// Cleanup rendering
void render_cleanup(void);

// Set color theme
void render_set_theme(ColorTheme theme);

// Enable/disable ground line
void render_set_ground(int enabled);

// Enable/disable shadow/reflection
void render_set_shadow(int enabled);

// Clear the screen (handles terminal resize)
void render_clear(void);

// Draw the dancer (with energy-based colors)
void render_dancer(struct dancer_state *state);

// Draw the frequency bars
void render_bars(double bass, double mid, double treble);

// Draw frame info (debug info)
void render_frame_info(struct dancer_state *state);

// Refresh the screen
void render_refresh(void);

// Draw info text at bottom
void render_info(const char *text);

// Get keyboard input (non-blocking)
int render_getch(void);

// Get terminal dimensions
void render_get_size(int *rows, int *cols);

// Check if terminal was resized (returns 1 if resize occurred)
int render_check_resize(void);

// Check for 256-color support
int render_has_256_colors(void);
