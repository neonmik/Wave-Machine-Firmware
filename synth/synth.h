#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "wavetable.h"
#include "log_table.h"


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

  enum class ADSRPhase : uint8_t {
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    OFF
  };

  extern uint32_t   _sample_rate;
  
  // used oscillator types, this can use multiple oscillatros, although can't be currently adjusted by the hardware
  extern uint16_t   _oscillator;      // bitmask for enabled oscillator types (see Oscillator enum for values)

  // variables for the wavetable oscillator
  extern uint16_t   _wave_shape;
  extern uint16_t   _wave_vector;
  extern uint16_t   _vector_mod;

  extern uint16_t   _attack_ms;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint16_t   _decay_ms;      // decay period
  extern uint16_t   _sustain;   // sustain volume
  extern uint16_t   _release_ms;      // release period

  extern int16_t    _vibrato;
  extern uint16_t   _tremelo;

  extern uint16_t _pitch_scale;

  extern uint8_t _octave;


  // extern bool      filter_enable;
  // extern uint16_t  filter_cutoff_frequency;

  struct Voices {
    
    // ADSR envelope;

    uint16_t  volume        = 0x7fff;    // channel volume (default 50%) - also could be called velocity

    bool      _gate         = false;  // used for tracking a note that's released, but not finished.
    bool      _active       = false;  // used for whole duration of note, from the very start of attack right up until the voise is finished
    uint64_t  activation_time  = 0;

    uint8_t   _note         = 0;
    uint16_t  _frequency    = 0;    // frequency of the voice (Hz)

    uint16_t  pulse_width   = 0x7fff; // duty cycle of square wave (default 50%)
    int16_t   noise         = 0;      // current noise value

    uint32_t  waveform_offset  = 0;   // voice offset (Q8)

    // int32_t   filter_last_sample;

    uint32_t  adsr_frame    = 0;      // number of frames into the current ADSR phase
    uint32_t  adsr_end_frame = 0;     // frame target at which the ADSR changes to the next phase
    uint32_t  adsr          = 0;
    int32_t   adsr_step     = 0;
    ADSRPhase adsr_phase    = ADSRPhase::OFF;
    uint64_t  adsr_activation_time = 0;

    uint8_t   wave_buf_pos  = 0;      //
    int16_t   wave_buffer[64];        // buffer for arbitrary waveforms. small as it's filled by user callback

    void *user_data = nullptr;
    void (*wave_buffer_callback)(Voices &channel);

    void note_on (uint8_t note, uint16_t frequency) {
      _gate = true;
      _active = true;

      _note = note;
      _frequency = frequency;
      
      trigger_attack();
    }
    void note_off () {
      _gate = false;
      trigger_release();
    }
    
    void clear () {
      _active = false;
      _note = 0;
      _frequency = 0;
    }
    
    void trigger_attack()  {
      adsr_activation_time = to_ms_since_boot(get_absolute_time());

      adsr_frame = 0;
      adsr_phase = ADSRPhase::ATTACK;
      adsr_end_frame = (_attack_ms * _sample_rate) / 1000;
      adsr_step = (int32_t(0xffffff) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void trigger_decay() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::DECAY;
      adsr_end_frame = (_decay_ms * _sample_rate) / 1000;
      adsr_step = (int32_t(_sustain << 8) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void trigger_sustain() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::SUSTAIN;
      adsr_end_frame = 0;
      adsr_step = 0;
    }
    void trigger_release() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::RELEASE;
      adsr_end_frame = (_release_ms * _sample_rate) / 1000;
      adsr_step = (int32_t(0) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void stopped() { 
      clear();

      adsr_activation_time = 0;
      adsr_frame = 0;
      adsr_phase = ADSRPhase::OFF;
      adsr_end_frame = 0;
      adsr_step = 0;
    }
    
    
  };

  extern Voices channels[MAX_VOICES];

  
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
  
}
