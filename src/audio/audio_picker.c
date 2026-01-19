/*
 * Audio Source Picker Implementation
 */

#include "audio_picker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>

#ifdef PULSE
#include <pulse/pulseaudio.h>
#endif

#ifdef PIPEWIRE
#include <pipewire/pipewire.h>
#endif

/* Simple enumeration - in a real implementation, would use PA/PW API */
AudioSourceList* audio_picker_enumerate(bool prefer_pulse) {
    AudioSourceList *list = calloc(1, sizeof(AudioSourceList));
    if (!list) return NULL;
    
    list->use_pulse = prefer_pulse;
    list->count = 0;
    list->selected = 0;
    
    /* Default/auto source */
    strncpy(list->sources[list->count].name, "default", 
            sizeof(list->sources[list->count].name) - 1);
    strncpy(list->sources[list->count].description, 
            "Auto-detect (recommended)",
            sizeof(list->sources[list->count].description) - 1);
    list->sources[list->count].index = list->count;
    list->count++;
    
    /* Common PulseAudio monitor sources */
    if (prefer_pulse) {
        const char *pulse_sources[] = {
            "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor",
            "alsa_output.usb-0000_00_14.0.analog-stereo.monitor",
            "combined.monitor"
        };
        
        for (int i = 0; i < 3 && list->count < MAX_AUDIO_SOURCES; i++) {
            strncpy(list->sources[list->count].name, pulse_sources[i],
                    sizeof(list->sources[list->count].name) - 1);
            snprintf(list->sources[list->count].description,
                    sizeof(list->sources[list->count].description),
                    "PulseAudio: %s", pulse_sources[i]);
            list->sources[list->count].index = list->count;
            list->count++;
        }
    } else {
        /* PipeWire node names */
        const char *pipewire_sources[] = {
            "alsa_output.monitor",
            "bluez_output.monitor",
            "v4l2_input.monitor"
        };
        
        for (int i = 0; i < 3 && list->count < MAX_AUDIO_SOURCES; i++) {
            strncpy(list->sources[list->count].name, pipewire_sources[i],
                    sizeof(list->sources[list->count].name) - 1);
            snprintf(list->sources[list->count].description,
                    sizeof(list->sources[list->count].description),
                    "PipeWire: %s", pipewire_sources[i]);
            list->sources[list->count].index = list->count;
            list->count++;
        }
    }
    
    return list;
}

char* audio_picker_show_menu(AudioSourceList *list) {
    if (!list || list->count == 0) return NULL;
    
    /* Initialize ncurses for menu */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    int selection = 0;
    int ch;
    bool done = false;
    
    while (!done) {
        clear();
        
        /* Draw title */
        attron(A_BOLD);
        mvprintw(2, 2, "ASCII Dancer - Audio Source Selection");
        attroff(A_BOLD);
        
        mvprintw(4, 2, "Use UP/DOWN arrows to select, ENTER to confirm, 'q' to quit");
        mvprintw(5, 2, "Backend: %s", list->use_pulse ? "PulseAudio" : "PipeWire");
        
        /* Draw sources */
        for (int i = 0; i < list->count; i++) {
            int y = 7 + i;
            
            if (i == selection) {
                attron(A_REVERSE);
                mvprintw(y, 2, "> ");
            } else {
                mvprintw(y, 2, "  ");
            }
            
            mvprintw(y, 4, "[%d] %s", i, list->sources[i].description);
            
            if (i == selection) {
                attroff(A_REVERSE);
            }
        }
        
        /* Instructions */
        mvprintw(LINES - 2, 2, "Selected: %s", list->sources[selection].name);
        
        refresh();
        
        /* Handle input */
        ch = getch();
        switch (ch) {
            case KEY_UP:
                selection = (selection - 1 + list->count) % list->count;
                break;
            case KEY_DOWN:
                selection = (selection + 1) % list->count;
                break;
            case 10:  /* Enter */
            case KEY_ENTER:
                list->selected = selection;
                done = true;
                break;
            case 'q':
            case 'Q':
            case 27:  /* ESC */
                endwin();
                return NULL;
        }
    }
    
    endwin();
    
    /* Return selected source name */
    static char selected_name[256];
    strncpy(selected_name, list->sources[selection].name, sizeof(selected_name) - 1);
    return selected_name;
}

void audio_picker_free(AudioSourceList *list) {
    if (list) {
        free(list);
    }
}
