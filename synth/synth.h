#pragma once

#include "../config.h"             

#include "../random.h"
#include "../queue.h"

#include "adsr.h"

#include "wavetable.h"
#include "resources.h"



namespace SYNTH {

  namespace {
    //  Q8 midi note freqs
    constexpr uint8_t Q_SCALING_FACTOR = 12;
  }

  extern uint16_t   volume;

  // variables for the wavetable oscillator
  extern uint16_t   currentWaveShape;
  extern uint16_t   currentWaveVector;

  extern uint32_t   currentAttack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint32_t   currentDecay;      // decay period
  extern uint32_t   currentSustain;   // sustain volume
  extern uint32_t   currentRelease;      // release period
  extern uint16_t   lastAttack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint16_t   lastDecay;      // decay period
  extern uint16_t   lastSustain;   // sustain volume
  extern uint16_t   lastRelease;      // release period

  extern int16_t    modVibrato;
  extern uint16_t   modTremelo;
  extern uint16_t   modVector;

  extern uint16_t   currentPitchBend;

  extern uint8_t    currentOctave;

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

    void noteOn (uint8_t inputNote) {
      gate = true; // wouldn't be needed if core moved
      active = true;

      // Original octave code - updates whenever its changed
      note = inputNote;

      // Newer octave code - updates only with note on call
      // note = (inputNote + (currentOctave * 12)); // sets the octave at the outset of the note...

      frequency = getFrequency(note);
    
      ADSR.triggerAttack();
      
      // changed = true; // for eventual performance improvement of pitch fixing. 
    }
    inline void calcIncrement (void) {
      // if (!changed) return; // if the frequency or pitch hasn't changed, return

      // Original octave code - updates whenever its changed
      phaseIncrement = ((((frequency * currentPitchBend) >> 10) << currentOctave) << Q_SCALING_FACTOR) / SAMPLE_RATE;

      // Newer octave code - updates only with note on call
      // phaseIncrement = (((frequency * currentPitchBend) >> 10) << Q_SCALING_FACTOR) / SAMPLE_RATE; // octave scaling achieved at note level

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

      phaseIncrement = 0;
      phaseAccumulator = 0;
    }
    bool isActive (void) {
      return active;
    }
    bool isGate (void) {
      return gate;
    }
    
    ADSREnvelope ADSR{currentAttack, currentDecay, currentSustain, currentRelease};
  };

  extern Oscillators channels[POLYPHONY];

  void voiceOn (uint8_t voice, uint8_t note);
  void voiceOff (uint8_t voice);
  // bool isVoiceActive (uint8_t voice);
  // bool isGateActive (uint8_t voice);
  // bool noteCheck (uint8_t slot, uint8_t note);
  
  void init ();
  uint16_t process();
  // bool is_audio_playing();

  void setWaveShape (uint16_t input);
  void setWaveVector (uint16_t input);
  void setOctave (uint16_t input);
  void setPitchBend (uint16_t input);
  // uint16_t get_pitch_log (uint16_t index);

  void setAttack (uint16_t input);
  void setDecay (uint16_t input);
  void setSustain (uint16_t input);
  void setRelease (uint16_t input);
  
  uint32_t calculateEndFrame (uint32_t milliseconds);

  void modulateVibrato (uint16_t input);
  void modulateTremelo (uint16_t input);
  void modulateVector (uint16_t input);

}
