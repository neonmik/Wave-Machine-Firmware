#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "adsr.h"
// #include "modulation.h"
#include "filter.h"
#include "wavetable.h"
#include "log_table.h"

#include "../mailbox.h"


namespace SYNTH {

  #define MAX_VOICES 8

  // constexpr float pi = 3.14159265358979323846f;

  extern uint16_t volume;



  enum Oscillator {
    WAVETABLE = 256,
    NOISE     = 128,
    SQUARE    = 64,
    SAW       = 32,
    TRIANGLE  = 16,
    SINE      = 8,
    WAVE      = 1
  };

  // enum class ADSRPhase : uint8_t {
  //   ATTACK,
  //   DECAY,
  //   SUSTAIN,
  //   RELEASE,
  //   OFF
  // };

  extern uint32_t   _sample_rate;
  
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

  extern uint16_t   _pitch_scale;

  extern uint8_t _octave;

  static bool _soft_start = true;
  static uint8_t  _soft_start_index = 0;
  static int16_t _soft_start_sample = -32768;

  // extern bool      filter_enable;
  // extern uint16_t  filter_cutoff_frequency;

  struct Voices {

    uint16_t  volume        = 0x7fff;    // channel volume (default 50%) - also could be called velocity


    bool      _gate         = false;  // used for tracking a note that's released, but not finished.
    bool      _active       = false;  // used for whole duration of note, from the very start of attack right up until the voise is finished
    uint32_t  activation_time  = 0;

    uint8_t   _note         = 0;
    uint16_t  _frequency    = 0;    // frequency of the voice (Hz)

    uint16_t  pulse_width   = 0x7fff; // duty cycle of square wave (default 50%)
    int16_t   noise         = 0;      // current noise value

    uint32_t  waveform_offset  = 0;   // voice offset (Q8)

    // int32_t   filter_last_sample;

    uint8_t   wave_buf_pos  = 0;      //
    int16_t   wave_buffer[64];        // buffer for arbitrary waveforms. small as it's filled by user callback

    void *user_data = nullptr;
    void (*wave_buffer_callback)(Voices &channel);

    void note_on (uint8_t note, uint16_t frequency) {
      _gate = true;
      _active = true;

      _note = note;
      _frequency = frequency;
      
      activation_time = to_ms_since_boot(get_absolute_time());

      ADSR.trigger_attack();
    }
    void note_off (void) {
      _gate = false;
      ADSR.trigger_release();
    }
    void note_stopped (void) {
      _active = false;
      _note = 0;
      _frequency = 0;
    }
    
    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};
  };

  extern Voices channels[MAX_VOICES];

  void voice_on (uint8_t voice, uint8_t note, uint16_t frequency);
  void voice_off (uint8_t voice);
  
  uint16_t get_audio_frame();
  bool is_audio_playing();
  void init (uint32_t _sample_rate);

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

  void update (void);
}
