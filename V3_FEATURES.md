# v3.0 Feature Guide

## New Controls

| Key | Function |
|-----|----------|
| `F` | Toggle background effects on/off |
| `E` | Cycle through 7 background effect types |

## Background Effects (7 Types)

1. **None** - No background effects
2. **Ambient Field** - Floating twinkling particles across the screen
3. **Spectral Waves** - Frequency-reactive wave pulses from the bottom
4. **Energy Aura** - Pulsing ring of particles around the dancer
5. **Beat Burst** - Synchronized explosions on beat hits
6. **Frequency Ribbons** - Vertical frequency bars with particles
7. **Particle Rain** - Falling particles from top
8. **Spiral Vortex** - Rotating spiral arms

## Enhanced Info Display

The status bar now shows:
- **BPM with Confidence**: `120bpm(85%)` - Higher % means more reliable tempo
- **Energy Zone**: `Low`, `Medium`, `High`, or `Peak` based on audio intensity
- **[FX]** indicator when background effects are active

## Advanced Features

### BPM Tracker
- Multi-tap tempo averaging for accurate BPM
- Confidence scoring (0-100%)
- Stability tracking
- Tempo locking when confident + stable
- 40-240 BPM range (wider than v2.x)

### Energy Analyzer
- RMS energy calculation
- 5 intensity zones
- Spectral features (brightness, centroid)
- Dynamic thresholds that adapt to music
- 6-band energy tracking

### Background Particle Effects
- All effects are audio-reactive
- Intensity automatically adjusts to energy
- Synchronized with beats and frequency bands
- Uses the existing 256-particle system

## Usage Tips

1. **Start simple**: Press `F` to enable effects, they'll default to "Ambient Field"
2. **Try different types**: Press `E` repeatedly to cycle through all 7 effect types
3. **Watch the confidence**: BPM confidence builds over time as more beats are detected
4. **Energy zones adapt**: The zones (Low/Med/High/Peak) adjust to the dynamic range of your music

## Testing

Run the visualizer with music and try:
```bash
./asciidancer
# Press F to enable effects
# Press E to cycle through types
# Watch BPM confidence increase as beats are detected
# See energy zones change with music intensity
```

Enjoy the upgraded visualizer! 🎉
