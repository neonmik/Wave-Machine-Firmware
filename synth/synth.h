#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../debug.h"
#include "../config.h"               

#include "../random.h"

#include "adsr.h"

#include "wavetable.h"
#include "resources.h"

#include "../queue.h"


namespace SYNTH {

  namespace {
    //  Q8 midi note freqs
    constexpr uint8_t Q_SCALING_FACTOR = 12;
  }

  extern uint16_t volume;

  extern uint8_t _released;

  // used oscillator types, this can use multiple oscillators, although can't be currently adjusted by the hardware
  extern uint16_t   _oscillator;      // bitmask for enabled oscillator types (see Oscillator enum for values)

  // variables for the wavetable oscillator
  extern uint16_t   _wave_shape;
  extern uint16_t   _wave_vector;
  extern uint16_t   _vector_mod;

  extern uint32_t   _attack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint32_t   _decay;      // decay period
  extern uint32_t   _sustain;   // sustain volume
  extern uint32_t   _release;      // release period
  extern uint16_t   lastAttack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint16_t   lastDecay;      // decay period
  extern uint16_t   lastSustain;   // sustain volume
  extern uint16_t   lastRelease;      // release period

  extern int16_t    _vibrato;
  extern uint16_t   _tremelo;

  extern uint16_t   pitchBend;

  extern uint8_t    _octave;

  static bool playSoftStart = true;
  static uint8_t softStartIndex = 0;
  static int16_t softStartSample = -32768;

  struct Oscillators {

    bool      changed           = false;  

    uint16_t  volume            = 0x7fff;         // channel volume (default 50%) - also could be called velocity

    bool      gate              = false;          // used for tracking a note that's released, but not finished.
    bool      active            = false;          // used for whole duration of note, from the very start of attack right up until the voise is finished

    uint8_t   note;                               // Midi Note number - used for filter voice
    uint32_t  frequency         = 0;              // Frequency in Hz << 8

    uint32_t  phaseIncrement    = 0;
    uint32_t  phaseAccumulator  = 0; 

    void noteOn (uint8_t input_note) {
      gate = true; // wouldn't be needed if core moved
      active = true;

      // Original octave code - updates whenever its changed
      note = input_note;

      // Newer octave code - updates only with note on call
      // note = (input_note + (_octave * 12)); // sets the octave at the outset of the note...

      frequency = getFrequency(note);
    
      ADSR.triggerAttack();
      
      // changed = true; // for eventual performance improvement of pitch fixing. 
    }
    inline void calcIncrement (void) {
      // if (!changed) return; // if the frequency or pitch hasn't changed, return

      // Original octave code - updates whenever its changed
      phaseIncrement = ((((frequency * pitchBend) >> 10) << _octave) << Q_SCALING_FACTOR) / SAMPLE_RATE;

      // Newer octave code - updates only with note on call
      // phaseIncrement = (((frequency * pitchBend) >> 10) << Q_SCALING_FACTOR) / SAMPLE_RATE; // octave scaling achieved at note level

      // changed = false; // for eventual performance improvement of pitch fixing. 
    }
    void noteOff (void) {
      gate = false; // wouldn't be needed if core moved
      ADSR.triggerRelease();
    }
    void noteStopped (void) {
      active = false;

      note = 0;
      frequency = 0;

      // reset - this may cause issues with wave form continuity
      phaseIncrement = 0;
      phaseAccumulator = 0;
    }
    bool isActive (void) {
      return active;
    }
    bool isGate (void) {
      return gate;
    }
    
    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};
  };

  extern Oscillators channels[POLYPHONY];

  void voiceOn (uint8_t voice, uint8_t note);
  void voiceOff (uint8_t voice);
  bool isVoiceActive (uint8_t voice);
  bool isGateActive (uint8_t voice);
  bool noteCheck (uint8_t slot, uint8_t note);
  
  uint16_t process();
  bool is_audio_playing();
  void init ();

  void setWaveshape (uint16_t shape);
  void setWavevector (uint16_t vector);
  void setOctave (uint16_t octave);
  void setPitchBend (uint16_t scale);
  uint16_t get_pitch_log (uint16_t index);

  void setAttack (uint16_t attack);
  void setDecay (uint16_t decay);
  void setSustain (uint16_t sustain);
  void setRelease (uint16_t release);
  
  uint32_t calculateEndFrame (uint32_t milliseconds);

  void modulateVibrato (uint16_t vibrato);
  void modulateTremelo (uint16_t tremelo);
  void modulateVector (uint16_t vector_mod);

}
