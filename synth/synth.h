#pragma once

#include "../config.h"             

#include "../random.h"
#include "../queue.h"

#include "adsr.h"

#include "wavetable.h"
#include "resources.h"



namespace SYNTH {
  
  namespace {
    uint16_t    currentWaveShape;
    uint16_t    lastWaveShape;

    uint16_t    currentWaveVector;
    uint16_t    lastWaveVector;

    uint16_t    currentPitchBend = 511;
    uint16_t    lastPitchBend;

    uint8_t     currentOctave = 0;
    uint16_t    lastOctave;

    int16_t     currentDetune;
    int16_t     lastDetune;

    int16_t     modVibrato;
    uint16_t    modTremelo;
    uint16_t    modVector;

    uint16_t    subLevel = 1023;
    uint16_t    noiseLevel = 0;

    uint16_t    osc2Wave;
    uint16_t    lastOsc2Wave;


    uint16_t volume = 0xFFFF;

    ADSRControls  envelopeControls(SAMPLE_RATE);

  }


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
      // changed = true; // for eventual performance improvement of pitch fixing. 

      frequency = getFrequency(note);
    
      ampEnvelope.triggerAttack();
      
    }
    inline void calcIncrement (void) {
      // if (!changed) return; // if the frequency or pitch hasn't changed, return
  
      // Original octave code - updates octave whenever its changed
      phaseIncrement = ((((frequency * currentPitchBend) >> 10) << currentOctave) << Q_SCALING_FACTOR) / SAMPLE_RATE;

      // Newer octave code - sets octave only with noteOn call
      // phaseIncrement = (((frequency * currentPitchBend) >> 10) << Q_SCALING_FACTOR) / SAMPLE_RATE; // octave scaling achieved at note level
      // changed = false; // for eventual performance improvement of pitch fixing. 
    }
    void noteOff (void) {
      gate = false; // wouldn't be needed if core moved
      ampEnvelope.triggerRelease();
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
    
    ADSREnvelope ampEnvelope{envelopeControls.getAttack(), envelopeControls.getDecay(), envelopeControls.getSustain(), envelopeControls.getRelease()};
  };

  extern Oscillators channels[POLYPHONY];

  void voiceOn (uint8_t voice, uint8_t note);
  void voiceOff (uint8_t voice);
  
  void init ();
  uint16_t process();

  void setWaveShape (uint16_t input);
  void setWaveVector (uint16_t input);
  void setOctave (uint16_t input);
  void setPitchBend (uint16_t input);

  void setAttack (uint16_t input);
  void setDecay (uint16_t input);
  void setSustain (uint16_t input);
  void setRelease (uint16_t input);
  

  void modulateVibrato (uint16_t input);
  void modulateTremelo (uint16_t input);
  void modulateVector (uint16_t input);

  void setSub (uint16_t input);
  void setNoise (uint16_t input);

  void setDetune (uint16_t input);
  void setOsc2Wave (uint16_t input);
}
