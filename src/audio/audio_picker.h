/*
 * Audio Source Picker - ASCII Dancer v3.0+
 *
 * Interactive menu for selecting audio sources at runtime
 */

#ifndef AUDIO_PICKER_H
#define AUDIO_PICKER_H

#include <stdbool.h>

#define MAX_AUDIO_SOURCES 64

typedef struct {
    char name[256];
    char description[512];
    int index;
} AudioSource;

typedef struct {
    AudioSource sources[MAX_AUDIO_SOURCES];
    int count;
    int selected;
    bool use_pulse;  /* true = PulseAudio, false = PipeWire */
} AudioSourceList;

/* Enumerate available audio sources */
AudioSourceList* audio_picker_enumerate(bool prefer_pulse);

/* Display interactive picker and return selected source name */
char* audio_picker_show_menu(AudioSourceList *list);

/* Free source list */
void audio_picker_free(AudioSourceList *list);

#endif /* AUDIO_PICKER_H */
