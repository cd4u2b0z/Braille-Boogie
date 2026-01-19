/*
 * Performance Profiler - ASCII Dancer v3.0+
 *
 * Real-time performance metrics display
 */

#ifndef PROFILER_H
#define PROFILER_H

#include <stdbool.h>
#include <stdint.h>

#define PROF_HISTORY_SIZE 120  /* 2 seconds at 60fps */

typedef struct {
    /* Timing */
    double frame_times[PROF_HISTORY_SIZE];
    int frame_index;
    
    /* Statistics */
    double fps_current;
    double fps_average;
    double fps_min;
    double fps_max;
    
    double frame_time_ms;
    double frame_time_avg_ms;
    
    /* Component breakdown */
    double audio_time_ms;
    double update_time_ms;
    double render_time_ms;
    
    /* Memory */
    int active_particles;
    int trail_segments;
    
    /* Display */
    bool enabled;
    int x, y;  /* Display position */
    
} Profiler;

/* Create profiler */
Profiler* profiler_create(void);

/* Destroy profiler */
void profiler_destroy(Profiler *prof);

/* Start/end frame timing */
void profiler_frame_start(Profiler *prof);
void profiler_frame_end(Profiler *prof);

/* Time specific components */
void profiler_mark_audio(Profiler *prof, double ms);
void profiler_mark_update(Profiler *prof, double ms);
void profiler_mark_render(Profiler *prof, double ms);

/* Update particle/trail counts */
void profiler_set_counts(Profiler *prof, int particles, int trails);

/* Toggle display */
void profiler_toggle(Profiler *prof);
bool profiler_is_enabled(Profiler *prof);

/* Render overlay */
void profiler_render(Profiler *prof);

/* Get stats */
void profiler_get_stats(Profiler *prof, double *fps, double *frame_ms);

#endif /* PROFILER_H */
