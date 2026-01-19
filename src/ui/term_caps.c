/*
 * Terminal Capabilities Detector Implementation
 */

#include "term_caps.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* Send terminal query and read response */
static bool query_terminal(const char *query, char *response, int max_len) {
    /* Write query to terminal */
    write(STDOUT_FILENO, query, strlen(query));
    fflush(stdout);
    
    /* Try to read response (with timeout) */
    fd_set fds;
    struct timeval timeout = {0, 100000}; /* 100ms timeout */
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0) {
        ssize_t n = read(STDIN_FILENO, response, max_len - 1);
        if (n > 0) {
            response[n] = '\0';
            return true;
        }
    }
    
    return false;
}

TerminalCaps* term_caps_detect(void) {
    TerminalCaps *caps = calloc(1, sizeof(TerminalCaps));
    if (!caps) return NULL;
    
    /* Get TERM environment variable */
    const char *term = getenv("TERM");
    if (term) {
        strncpy(caps->term_name, term, sizeof(caps->term_name) - 1);
    } else {
        strncpy(caps->term_name, "unknown", sizeof(caps->term_name) - 1);
    }
    
    /* Check for true color support */
    const char *colorterm = getenv("COLORTERM");
    caps->supports_truecolor = (colorterm && 
                               (strstr(colorterm, "truecolor") || 
                                strstr(colorterm, "24bit")));
    
    /* Query for Sixel support (DA1 query) */
    char response[256] = {0};
    if (query_terminal("\033[c", response, sizeof(response))) {
        /* Look for Sixel indicator (4) in response */
        caps->supports_sixel = (strstr(response, ";4;") != NULL ||
                               strstr(response, ";4c") != NULL);
    }
    
    /* Check for Kitty graphics protocol */
    if (strstr(caps->term_name, "kitty") || strstr(caps->term_name, "xterm-kitty")) {
        caps->supports_kitty = true;
    }
    
    /* Check for iTerm2 */
    const char *term_program = getenv("TERM_PROGRAM");
    if (term_program && strcmp(term_program, "iTerm.app") == 0) {
        caps->supports_iterm2 = true;
    }
    
    /* Detect max colors */
    if (caps->supports_truecolor) {
        caps->max_colors = 16777216; /* 24-bit */
    } else if (strstr(caps->term_name, "256color")) {
        caps->max_colors = 256;
    } else {
        caps->max_colors = 16; /* Standard ANSI */
    }
    
    return caps;
}

bool term_caps_has_sixel(TerminalCaps *caps) {
    return caps ? caps->supports_sixel : false;
}

bool term_caps_has_kitty(TerminalCaps *caps) {
    return caps ? caps->supports_kitty : false;
}

bool term_caps_has_truecolor(TerminalCaps *caps) {
    return caps ? caps->supports_truecolor : false;
}

void term_caps_free(TerminalCaps *caps) {
    if (caps) free(caps);
}

void term_caps_print(TerminalCaps *caps) {
    if (!caps) return;
    
    printf("\nTerminal Capabilities:\n");
    printf("======================\n");
    printf("Terminal: %s\n", caps->term_name);
    printf("True Color: %s\n", caps->supports_truecolor ? "✓ Yes" : "✗ No");
    printf("Max Colors: %d\n", caps->max_colors);
    printf("Sixel Graphics: %s\n", caps->supports_sixel ? "✓ Yes" : "✗ No");
    printf("Kitty Graphics: %s\n", caps->supports_kitty ? "✓ Yes" : "✗ No");
    printf("iTerm2 Inline: %s\n", caps->supports_iterm2 ? "✓ Yes" : "✗ No");
    printf("\n");
    
    if (caps->supports_sixel || caps->supports_kitty || caps->supports_iterm2) {
        printf("🎉 Advanced graphics available!\n");
    } else {
        printf("ℹ️  Using braille Unicode rendering (still looks great!)\n");
    }
}
