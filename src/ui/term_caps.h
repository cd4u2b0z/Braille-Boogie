/*
 * Terminal Capabilities Detector - ASCII Dancer v3.0+
 *
 * Detect advanced terminal features (Sixel, Kitty, iTerm2)
 */

#ifndef TERM_CAPS_H
#define TERM_CAPS_H

#include <stdbool.h>

typedef struct {
    bool supports_sixel;
    bool supports_kitty;
    bool supports_iterm2;
    bool supports_truecolor;
    int max_colors;
    char term_name[256];
    char term_version[64];
} TerminalCaps;

/* Detect terminal capabilities */
TerminalCaps* term_caps_detect(void);

/* Query specific capability */
bool term_caps_has_sixel(TerminalCaps *caps);
bool term_caps_has_kitty(TerminalCaps *caps);
bool term_caps_has_truecolor(TerminalCaps *caps);

/* Free caps */
void term_caps_free(TerminalCaps *caps);

/* Print capability report */
void term_caps_print(TerminalCaps *caps);

#endif /* TERM_CAPS_H */
