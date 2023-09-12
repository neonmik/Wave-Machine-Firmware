#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../debug.h"
#include "../config.h"               


#include "adsr.h"

#include "wavetable.h"
#include "resources.h"

#include "../queue.h"


namespace SYNTH {

  namespace {
    //  Q8 midi note freqs
    const uint32_t note2freq[128] = {
        2093,					  2217,			  		2349,		  			2489,					  2637,		  			2793,		  			2959,			  		3135,
        3322,				  	3520,		  			3729,			  		3951,				  	4186,			  		4434,	  				4698,				  	4978,
        5274,				  	5587,	  				5919,				  	6271,			  		6644,				  	7040, 					7458,					  7902,
        8372,					  8869, 					9397,					  9956,		  			10548,					11175,					11839,					12543,
        13289,					14080,					14917,					15804,					16744,					17739,					18794,					19912,
        21096,					22350,					23679,					25087,					26579,					28160,					29834,					31608,
        33488,					35479,					37589,					39824,					42192,					44701,					47359,					50175,
        53159,					56320,					59668,					63217,					66976,					70958,					75178,					79648,
        84384,					89402,					94718,					100350,					106318,					112640,					119337,					126434,
        133952,					141917,					150356,					159297,					168769,					178804,					189437,					200701,
        212636,					225280,					238675,					252868,					267904,					283835,					300712,					318594,
        337538,					357609,					378874,					401403,					425272,					450560,					477351,					505736,
        535809,					567670,					601425,					637188,					675077,					715219,					757748,					802806,
        850544,					901120,					954703,					1011473,				1071618,				1135340,				1202850,				1274376,
        1350154,				1430438,				1515497,				1605613,				1701088,				1802240,				1909406,				2022946,
        2143236,				2270680,				2405701,				2548752,				2700308,				2860877,				3030994,				3211226,
    };
    inline uint32_t getFreq(uint8_t note) {
        return note2freq[note];
    }
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
  extern uint16_t   _last_attack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint16_t   _last_decay;      // decay period
  extern uint16_t   _last_sustain;   // sustain volume
  extern uint16_t   _last_release;      // release period

  extern int16_t    _vibrato;
  extern uint16_t   _tremelo;

  extern uint16_t   pitchBend;

  extern uint8_t    _octave;

  static bool _soft_start = true;
  static uint8_t _soft_start_index = 0;
  static int16_t _soft_start_sample = -32768;

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

      note = (input_note + (_octave * 12)); // sets the octave at the outset of the note...
      // note = input_note;
      frequency = getFreq(note);
    
      ADSR.trigger_attack();
      
      // changed = true; // for eventual performance improvement of pitch fixing. 
    }
    inline void calcIncrement (void) {
      // if (!changed) return; // if the frequency or pitch hasn't changed, return

      // phaseIncrement = ((((frequency * pitchBend) >> 10) << _octave) << Q_SCALING_FACTOR) / SAMPLE_RATE;
      phaseIncrement = (((frequency * pitchBend) >> 10) << Q_SCALING_FACTOR) / SAMPLE_RATE; // octave scaling achieved at note level
      // changed = false; // for eventual performance improvement of pitch fixing. 
    }
    void noteOff (void) {
      gate = false; // wouldn't be needed if core moved
      ADSR.trigger_release();
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

  void voice_on (uint8_t voice, uint8_t note);
  void voice_off (uint8_t voice);
  bool isVoiceActive (uint8_t voice);
  bool isGateActive (uint8_t voice);
  bool noteCheck (uint8_t slot, uint8_t note);
  
  uint16_t process();
  bool is_audio_playing();
  void Init ();

  void set_waveshape (uint16_t shape);
  void set_wavevector (uint16_t vector);
  void set_octave (uint16_t octave);
  void set_pitch_scale (uint16_t scale);
  uint16_t get_pitch_log (uint16_t index);

  void set_attack (uint16_t attack);
  void set_decay (uint16_t decay);
  void set_sustain (uint16_t sustain);
  void set_release (uint16_t release);
  
  uint32_t calc_end_frame (uint32_t milliseconds);

  void modulate_vibrato (uint16_t vibrato);
  void modulate_tremelo (uint16_t tremelo);
  void modulate_vector (uint16_t vector_mod);

}
