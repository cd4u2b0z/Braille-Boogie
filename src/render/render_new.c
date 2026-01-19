// ncurses rendering implementation with UTF-8, 256-color, ground/shadow support
// v2.1 features: themes, ground line, shadow/reflection, terminal resize

#include "render.h"
#include "colors.h"
#include "../dancer/dancer.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <signal.h>

static int term_rows, term_cols;
static volatile int resize_pending = 0;
static int show_ground = 1;
static int show_shadow = 1;
static float current_energy = 0.0f;

/* SIGWINCH handler for terminal resize */
static void handle_resize(int sig) {
    (void)sig;
    resize_pending = 1;
}

int render_init(void) {
    // Enable UTF-8 support
    setlocale(LC_ALL, "");
    
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);  // Non-blocking getch
    curs_set(0);            // Hide cursor
    keypad(stdscr, TRUE);

    // Initialize 256-color theme system
    if (colors_init() != 0) {
        // Fallback to basic colors (handled internally)
    }

    getmaxyx(stdscr, term_rows, term_cols);
    
    // Set up resize handler
    signal(SIGWINCH, handle_resize);
    
    return 0;
}

void render_cleanup(void) {
    signal(SIGWINCH, SIG_DFL);
    endwin();
}

void render_set_theme(ColorTheme theme) {
    colors_apply_theme(theme);
}

void render_set_ground(int enabled) {
    show_ground = enabled;
}

void render_set_shadow(int enabled) {
    show_shadow = enabled;
}

void render_clear(void) {
    // Handle pending resize
    if (resize_pending) {
        resize_pending = 0;
        endwin();
        refresh();
        clear();
    }
    
    erase();
    getmaxyx(stdscr, term_rows, term_cols);
}

/* Draw a horizontal ground line */
static void render_ground_line(int ground_row) {
    if (!show_ground || ground_row >= term_rows - 4) return;
    
    attron(COLOR_PAIR(colors_get_ground_pair()) | A_DIM);
    
    // Draw ground line using Unicode box drawing
    int start = (term_cols - FRAME_WIDTH * 2) / 2;
    int end = start + FRAME_WIDTH * 2;
    if (start < 2) start = 2;
    if (end > term_cols - 2) end = term_cols - 2;
    
    mvhline(ground_row, start, ACS_HLINE, end - start);
    
    // Add some floor texture
    for (int x = start; x < end; x += 4) {
        mvaddch(ground_row, x, ACS_BTEE);
    }
    
    attroff(COLOR_PAIR(colors_get_ground_pair()) | A_DIM);
}

/* Draw shadow/reflection below ground line */
static void render_shadow(struct dancer_state *state, int start_row, int start_col) {
    if (!show_shadow) return;
    
    // Get the frame
    char frame[FRAME_WIDTH * FRAME_HEIGHT * 4 + FRAME_HEIGHT + 1];
    dancer_compose_frame(state, frame);
    
    // Calculate shadow position (mirrored, below ground)
    int ground_row = start_row + FRAME_HEIGHT;
    int shadow_start = ground_row + 1;
    
    if (shadow_start >= term_rows - 4) return;
    
    attron(COLOR_PAIR(colors_get_shadow_pair(current_energy)) | A_DIM);
    
    // Draw upside-down (mirrored) version, only partial
    char *lines[FRAME_HEIGHT];
    int line_count = 0;
    char *line = frame;
    
    while (*line && line_count < FRAME_HEIGHT) {
        lines[line_count] = line;
        char *newline = strchr(line, '\n');
        if (newline) {
            *newline = '\0';
            line = newline + 1;
        } else {
            break;
        }
        line_count++;
    }
    
    // Draw shadow (inverted, only top 3-4 rows of shadow visible, fading)
    int shadow_rows = (FRAME_HEIGHT < 4) ? FRAME_HEIGHT : 4;
    for (int i = 0; i < shadow_rows && (shadow_start + i) < term_rows - 4; i++) {
        int src_row = FRAME_HEIGHT - 1 - i;
        if (src_row >= 0 && src_row < line_count) {
            mvprintw(shadow_start + i, start_col, "%s", lines[src_row]);
        }
    }
    
    attroff(COLOR_PAIR(colors_get_shadow_pair(current_energy)) | A_DIM);
}

void render_dancer(struct dancer_state *state) {
    // Buffer for braille output (4 bytes per char + newlines + null)
    char frame[FRAME_WIDTH * FRAME_HEIGHT * 4 + FRAME_HEIGHT + 1];
    dancer_compose_frame(state, frame);

    // Calculate center position
    int start_row = (term_rows - FRAME_HEIGHT) / 2 - 4;
    int start_col = (term_cols - FRAME_WIDTH) / 2;

    if (start_row < 0) start_row = 0;
    if (start_col < 0) start_col = 0;

    // Calculate energy for color
    current_energy = (state->bass_intensity + state->mid_intensity + state->treble_intensity) / 3.0f;
    
    // Draw ground line first
    int ground_row = start_row + FRAME_HEIGHT;
    render_ground_line(ground_row);
    
    // Draw shadow (reflection) below ground
    render_shadow(state, start_row, start_col);

    // Get color pair based on energy
    int color_pair = colors_get_dancer_pair(current_energy);
    
    // Draw dancer with energy-based color
    attron(COLOR_PAIR(color_pair) | A_BOLD);

    char *line = frame;
    int row = 0;
    while (*line && row < FRAME_HEIGHT) {
        char *newline = strchr(line, '\n');
        if (newline) *newline = '\0';

        // Use mvprintw which handles UTF-8 with proper locale
        mvprintw(start_row + row, start_col, "%s", line);

        if (newline) {
            line = newline + 1;
        } else {
            break;
        }
        row++;
    }

    attroff(COLOR_PAIR(color_pair) | A_BOLD);
}

void render_bars(double bass, double mid, double treble) {
    int bar_width = 20;
    int bar_row = term_rows - 5;
    int total_width = bar_width * 3 + 10;
    int start_col = (term_cols - total_width) / 2;
    
    if (start_col < 0) start_col = 2;
    if (bar_row < FRAME_HEIGHT + 4) return;

    int bass_col = start_col;
    int mid_col = start_col + bar_width + 5;
    int treble_col = start_col + (bar_width + 5) * 2;

    // Draw labels with themed colors
    attron(COLOR_PAIR(colors_get_bass_pair((float)bass)));
    mvprintw(bar_row - 1, bass_col + bar_width/2 - 2, "BASS");
    attroff(COLOR_PAIR(colors_get_bass_pair((float)bass)));
    
    attron(COLOR_PAIR(colors_get_mid_pair((float)mid)));
    mvprintw(bar_row - 1, mid_col + bar_width/2 - 1, "MID");
    attroff(COLOR_PAIR(colors_get_mid_pair((float)mid)));
    
    attron(COLOR_PAIR(colors_get_treble_pair((float)treble)));
    mvprintw(bar_row - 1, treble_col + bar_width/2 - 3, "TREBLE");
    attroff(COLOR_PAIR(colors_get_treble_pair((float)treble)));

    // Draw bars using block characters
    int bass_len = (int)(bass * bar_width);
    int mid_len = (int)(mid * bar_width);
    int treble_len = (int)(treble * bar_width);

    // Bass bar
    attron(COLOR_PAIR(colors_get_bass_pair((float)bass)) | A_BOLD);
    mvprintw(bar_row, bass_col, "[");
    for (int i = 0; i < bar_width; i++) {
        if (i < bass_len)
            printw("█");
        else
            printw(" ");
    }
    printw("]");
    attroff(COLOR_PAIR(colors_get_bass_pair((float)bass)) | A_BOLD);

    // Mid bar
    attron(COLOR_PAIR(colors_get_mid_pair((float)mid)) | A_BOLD);
    mvprintw(bar_row, mid_col, "[");
    for (int i = 0; i < bar_width; i++) {
        if (i < mid_len)
            printw("█");
        else
            printw(" ");
    }
    printw("]");
    attroff(COLOR_PAIR(colors_get_mid_pair((float)mid)) | A_BOLD);

    // Treble bar
    attron(COLOR_PAIR(colors_get_treble_pair((float)treble)) | A_BOLD);
    mvprintw(bar_row, treble_col, "[");
    for (int i = 0; i < bar_width; i++) {
        if (i < treble_len)
            printw("█");
        else
            printw(" ");
    }
    printw("]");
    attroff(COLOR_PAIR(colors_get_treble_pair((float)treble)) | A_BOLD);
    
    // Energy indicator
    double total_energy = (bass + mid + treble) / 3.0;
    int energy_pair = colors_get_energy_pair((float)total_energy);
    attron(COLOR_PAIR(energy_pair));
    mvprintw(bar_row + 2, (term_cols - 30) / 2, "Energy: ");
    int energy_bar = (int)(total_energy * 20);
    for (int i = 0; i < 20; i++) {
        if (i < energy_bar)
            printw("▓");
        else
            printw("░");
    }
    attroff(COLOR_PAIR(energy_pair));
}

void render_frame_info(struct dancer_state *state) {
    // Show current frame number in bottom corner
    attron(COLOR_PAIR(colors_get_info_pair()));
    mvprintw(term_rows - 1, 2, "Frame: %d | B:%.2f M:%.2f T:%.2f | Press 'q' to quit",
             state->current_frame,
             state->bass_intensity,
             state->mid_intensity,
             state->treble_intensity);
    attroff(COLOR_PAIR(colors_get_info_pair()));
}

void render_refresh(void) {
    refresh();
}

void render_info(const char *text) {
    attron(COLOR_PAIR(colors_get_info_pair()));
    mvprintw(term_rows - 1, 2, "%s", text);
    attroff(COLOR_PAIR(colors_get_info_pair()));
}

int render_getch(void) {
    return getch();
}

/* Get terminal dimensions for adaptive scaling */
void render_get_size(int *rows, int *cols) {
    if (rows) *rows = term_rows;
    if (cols) *cols = term_cols;
}

/* Check if terminal was resized */
int render_check_resize(void) {
    if (resize_pending) {
        resize_pending = 0;
        getmaxyx(stdscr, term_rows, term_cols);
        return 1;
    }
    return 0;
}

/* Get 256-color support status */
int render_has_256_colors(void) {
    return colors_has_256();
}
